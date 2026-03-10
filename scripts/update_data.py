import pandas as pd
import yfinance as yf
from concurrent.futures import ThreadPoolExecutor, as_completed
from tqdm import tqdm
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parent.parent
OUTPUT_PATH = BASE_DIR / "data" / "nasdaq100_fundamentals_full.csv"

tickers = [
    "ADBE","ADP","AMD","ABNB","ALNY","GOOGL","GOOG","AMZN","AEP","AMGN",
    "ADI","AAPL","AMAT","APP","ARM","ASML","TEAM","ADSK","AXON","BKR",
    "BKNG","AVGO","CDNS","CHTR","CTAS","CSCO","CCEP","CTSH","CMCSA","CEG",
    "CPRT","CSGP","COST","CRWD","CSX","DDOG","DXCM","FANG","DASH","EA",
    "EXC","FAST","FER","FTNT","GEHC","GILD","HON","IDXX","INSM","INTC",
    "INTU","ISRG","KDP","KLAC","KHC","LRCX","LIN","MAR","MRVL","MELI",
    "META","MCHP","MU","MSFT","MSTR","MDLZ","MPWR","MNST","NFLX","NVDA",
    "NXPI","ODFL","ORLY","PCAR","PLTR","PANW","PAYX","PYPL","PDD","PEP",
    "QCOM","REGN","ROP","ROST","STX","SHOP","SBUX","SNPS","TTWO","TSLA",
    "TXN","TRI","TMUS","VRSK","VRTX","WMT","WBD","WDC","WDAY","XEL","ZS"
]

MAX_WORKERS = 8


def first_existing(df, names):
    if df is None or df.empty:
        return None
    for name in names:
        if name in df.index:
            try:
                val = df.loc[name].iloc[0]
                if pd.notna(val):
                    return val
            except Exception:
                continue
    return None


def get_price(ticker_obj):
    try:
        fast_info = ticker_obj.fast_info
        if fast_info is not None:
            for key in ("lastPrice", "last_price", "regularMarketPrice"):
                try:
                    price = fast_info.get(key)
                    if price is not None and pd.notna(price):
                        return float(price)
                except Exception:
                    pass
    except Exception:
        pass

    try:
        hist = ticker_obj.history(period="5d", auto_adjust=False)
        if hist is not None and not hist.empty:
            return float(hist["Close"].dropna().iloc[-1])
    except Exception:
        pass

    return None


def extract_row(ticker):
    t = yf.Ticker(ticker)

    income = t.income_stmt
    balance = t.balance_sheet
    cashflow = t.cash_flow

    info = {}
    try:
        info = t.info or {}
    except Exception:
        info = {}

    shares = info.get("sharesOutstanding")

    name = (
        info.get("longName")
        or info.get("shortName")
        or ticker
    )

    sector = (
        info.get("sector")
        or info.get("industry")
        or None
    )

    price = get_price(t)

    row = {
        "ticker": ticker,
        "name": name,
        "sector": sector,
        "price": price,
        "shares_outstanding": shares,
        "revenue": first_existing(income, ["Total Revenue", "Operating Revenue"]),
        "gross_profit": first_existing(income, ["Gross Profit"]),
        "operating_income": first_existing(income, [
            "Operating Income",
            "Total Operating Income As Reported"
        ]),
        "net_income": first_existing(income, [
            "Net Income",
            "Net Income Common Stockholders"
        ]),
        "operating_cash_flow": first_existing(cashflow, [
            "Operating Cash Flow",
            "Cash Flow From Continuing Operating Activities"
        ]),
        "capital_expenditures": first_existing(cashflow, [
            "Capital Expenditure",
            "Net PPE Purchase And Sale",
            "Purchase Of PPE"
        ]),
        "total_debt": first_existing(balance, [
            "Total Debt",
            "Net Debt"
        ]),
        "cash": first_existing(balance, [
            "Cash And Cash Equivalents",
            "Cash Cash Equivalents And Short Term Investments"
        ]),
        "total_equity": first_existing(balance, [
            "Stockholders Equity",
            "Total Equity Gross Minority Interest"
        ]),
        "total_assets": first_existing(balance, ["Total Assets"]),
        "current_assets": first_existing(balance, ["Current Assets"]),
        "current_liabilities": first_existing(balance, ["Current Liabilities"]),
    }

    return row


rows = []

with ThreadPoolExecutor(max_workers=MAX_WORKERS) as executor:
    futures = [executor.submit(extract_row, ticker) for ticker in tickers]

    for future in tqdm(as_completed(futures), total=len(futures), desc="Processing tickers"):
        try:
            row = future.result()
            rows.append(row)
        except Exception:
            pass


df = pd.DataFrame(rows)

for col in df.columns:
    if col not in ("ticker", "name", "sector"):
        df[col] = pd.to_numeric(df[col], errors="coerce")


df["market_cap"] = df["price"] * df["shares_outstanding"]

df["free_cash_flow"] = df["operating_cash_flow"] - df["capital_expenditures"].abs()

df["gross_margin"] = df["gross_profit"] / df["revenue"]
df["operating_margin"] = df["operating_income"] / df["revenue"]
df["net_margin"] = df["net_income"] / df["revenue"]

df["current_ratio"] = df["current_assets"] / df["current_liabilities"]
df["debt_to_equity"] = df["total_debt"] / df["total_equity"]

df["book_value_per_share"] = df["total_equity"] / df["shares_outstanding"]
df["revenue_per_share"] = df["revenue"] / df["shares_outstanding"]
df["eps"] = df["net_income"] / df["shares_outstanding"]
df["fcf_per_share"] = df["free_cash_flow"] / df["shares_outstanding"]

df["pe_ratio"] = df["price"] / df["eps"]
df["pb_ratio"] = df["price"] / df["book_value_per_share"]
df["ps_ratio"] = df["price"] / df["revenue_per_share"]
df["fcf_yield"] = df["free_cash_flow"] / df["market_cap"]

df["enterprise_value"] = df["market_cap"] + df["total_debt"] - df["cash"]

df["ev_to_ebit"] = df["enterprise_value"] / df["operating_income"]
df["ev_to_sales"] = df["enterprise_value"] / df["revenue"]
df["ev_to_fcf"] = df["enterprise_value"] / df["free_cash_flow"]

df["roe"] = df["net_income"] / df["total_equity"]
df["roa"] = df["net_income"] / df["total_assets"]

df.replace([float("inf"), -float("inf")], pd.NA, inplace=True)

df = df.sort_values("ticker").reset_index(drop=True)

required_columns = [
    "ticker",
    "name",
    "sector",
    "price",
    "shares_outstanding",
    "revenue",
    "gross_profit",
    "operating_income",
    "net_income",
    "operating_cash_flow",
    "capital_expenditures",
    "total_debt",
    "cash",
    "total_equity",
    "total_assets",
    "current_assets",
    "current_liabilities",
    "market_cap",
    "free_cash_flow",
    "gross_margin",
    "operating_margin",
    "net_margin",
    "current_ratio",
    "debt_to_equity",
    "book_value_per_share",
    "revenue_per_share",
    "eps",
    "fcf_per_share",
    "pe_ratio",
    "pb_ratio",
    "ps_ratio",
    "fcf_yield",
    "enterprise_value",
    "ev_to_ebit",
    "ev_to_sales",
    "ev_to_fcf",
    "roe",
    "roa",
]

df = df.dropna(subset=required_columns).reset_index(drop=True)

df.to_csv(OUTPUT_PATH, index=False, float_format="%.6f")
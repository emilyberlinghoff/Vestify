#include "ui/StockPrinter.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>

namespace
{
    const std::string GREEN = "\033[32m";
    const std::string RED = "\033[31m";
    const std::string YELLOW = "\033[33m";
    const std::string RESET = "\033[0m";
}

std::string StockPrinter::formatMoney(double value)
{
    const double trillion = 1e12;
    const double billion = 1e9;
    const double million = 1e6;
    const double thousand = 1e3;

    std::ostringstream out;
    out << std::fixed << std::setprecision(2);

    if (std::abs(value) >= trillion)
    {
        out << value / trillion << "T";
    }
    else if (std::abs(value) >= billion)
    {
        out << value / billion << "B";
    }
    else if (std::abs(value) >= million)
    {
        out << value / million << "M";
    }
    else if (std::abs(value) >= thousand)
    {
        out << value / thousand << "K";
    }
    else
    {
        out << value;
    }

    return out.str();
}

std::string StockPrinter::colorizeMetric(double value, double goodThreshold, double weakThreshold)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(4) << value;

    if (value >= goodThreshold)
    {
        return GREEN + out.str() + RESET;
    }
    if (value <= weakThreshold)
    {
        return RED + out.str() + RESET;
    }
    return YELLOW + out.str() + RESET;
}

std::string StockPrinter::colorizeLowIsBetter(double value, double goodThreshold, double badThreshold)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << value;

    if (value <= goodThreshold)
    {
        return GREEN + out.str() + RESET;
    }
    if (value >= badThreshold)
    {
        return RED + out.str() + RESET;
    }
    return YELLOW + out.str() + RESET;
}

void StockPrinter::printStocks(const std::vector<Stock> &stocks)
{
    std::cout << std::fixed;
    std::cout.imbue(std::locale(""));

    for (const auto &stock : stocks)
    {
        std::cout << "\n============================================================\n";
        std::cout << stock.ticker << " | " << stock.name << "\n";
        std::cout << "Sector: " << stock.sector << "\n";
        std::cout << "============================================================\n\n";

        std::cout << std::left
                  << std::setw(22) << "Metric"
                  << "Value\n";
        std::cout << "------------------------------------------------------------\n";

        std::cout << std::setw(22) << "Price" << "$" << std::setprecision(2) << stock.price << "\n";
        std::cout << std::setw(22) << "Market Cap" << "$" << formatMoney(stock.market_cap) << "\n";
        std::cout << std::setw(22) << "Revenue" << "$" << formatMoney(stock.revenue) << "\n";
        std::cout << std::setw(22) << "Net Income" << "$" << formatMoney(stock.net_income) << "\n";
        std::cout << std::setw(22) << "Free Cash Flow" << "$" << formatMoney(stock.free_cash_flow) << "\n";

        std::cout << "\nBalance Sheet\n";
        std::cout << "------------------------------------------------------------\n";
        std::cout << std::setw(22) << "Cash" << "$" << formatMoney(stock.cash) << "\n";
        std::cout << std::setw(22) << "Total Debt" << "$" << formatMoney(stock.total_debt) << "\n";
        std::cout << std::setw(22) << "Equity" << "$" << formatMoney(stock.total_equity) << "\n";

        std::cout << "\nProfitability\n";
        std::cout << "------------------------------------------------------------\n";
        std::cout << std::setw(22) << "ROE" << colorizeMetric(stock.roe, 0.15, 0.05) << "\n";
        std::cout << std::setw(22) << "ROA" << colorizeMetric(stock.roa, 0.08, 0.03) << "\n";
        std::cout << std::setw(22) << "Gross Margin" << colorizeMetric(stock.gross_margin, 0.40, 0.20) << "\n";
        std::cout << std::setw(22) << "Operating Margin" << colorizeMetric(stock.operating_margin, 0.20, 0.08) << "\n";
        std::cout << std::setw(22) << "Net Margin" << colorizeMetric(stock.net_margin, 0.15, 0.05) << "\n";

        std::cout << "\nValuation\n";
        std::cout << "------------------------------------------------------------\n";
        std::cout << std::setw(22) << "PE" << colorizeLowIsBetter(stock.pe_ratio, 20.0, 40.0) << "\n";
        std::cout << std::setw(22) << "PB" << colorizeLowIsBetter(stock.pb_ratio, 3.0, 10.0) << "\n";
        std::cout << std::setw(22) << "PS" << colorizeLowIsBetter(stock.ps_ratio, 3.0, 10.0) << "\n";
        std::cout << std::setw(22) << "EV/EBIT" << colorizeLowIsBetter(stock.ev_to_ebit, 15.0, 30.0) << "\n";
        std::cout << std::setw(22) << "EV/FCF" << colorizeLowIsBetter(stock.ev_to_fcf, 20.0, 40.0) << "\n";
    }
}

void StockPrinter::printAvailableStocks(const std::vector<Stock> &stocks)
{
    if (stocks.empty())
    {
        std::cout << "No stock data loaded.\n";
        return;
    }

    std::cout << "\nAvailable Stocks\n";
    std::cout << "-----------------------------------------------------------------\n";
    std::cout << std::left
              << std::setw(10) << "Ticker"
              << std::setw(25) << "Sector"
              << "Company\n";
    std::cout << "-----------------------------------------------------------------\n";

    for (const auto &stock : stocks)
    {
        std::cout << std::left
                  << std::setw(10) << stock.ticker
                  << std::setw(25) << stock.sector
                  << stock.name
                  << "\n";
    }
}

void StockPrinter::printTopEVFCF(std::vector<Stock> stocks, std::size_t count)
{
    if (stocks.empty())
    {
        std::cout << "No stock data loaded.\n";
        return;
    }

    std::sort(
        stocks.begin(),
        stocks.end(),
        [](const Stock &a, const Stock &b)
        {
            return a.ev_to_fcf < b.ev_to_fcf;
        });

    std::cout << "\nTop " << count << " Cheapest Stocks (EV/FCF)\n";
    std::cout << "-----------------------------------------------------------------\n";
    std::cout << std::left
              << std::setw(10) << "Ticker"
              << std::setw(40) << "Company"
              << "EV/FCF\n";
    std::cout << "-----------------------------------------------------------------\n";

    for (std::size_t i = 0; i < std::min(count, stocks.size()); ++i)
    {
        std::cout << std::setw(10) << stocks[i].ticker
                  << std::setw(40) << stocks[i].name
                  << std::fixed << std::setprecision(2)
                  << stocks[i].ev_to_fcf
                  << "\n";
    }
}
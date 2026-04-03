# Vestify

Vestify is a C++17 stock analysis application that loads financial data from CSV files or live APIs and provides scoring, screening, backtesting, and portfolio tracking through an interactive command-line interface.

The project is organized into modular components separating data ingestion, core logic, scoring models, persistence, and user interface.

---

## Features

- **Stock Data Loading** -- Load NASDAQ 100 fundamental data from a bundled CSV file with 38 financial metrics per stock.
- **Live Data Fetching** -- Retrieve real-time stock quotes from Alpha Vantage via the REST API.
- **Stock Scoring and Ranking** -- Rank stocks using five factor-based scoring models (Value, Growth, Momentum, Quality, Efficiency) with predefined or custom-weighted strategy presets.
- **Score Breakdown** -- Inspect per-factor contributions for any ranked stock to understand why it scored the way it did.
- **Stock Screening** -- Filter stocks by sector, valuation, profitability, and other financial criteria.
- **Historical Backtesting** -- Simulate portfolio performance over historical price data with periodic rebalancing using any scoring strategy.
- **Watchlist Management** -- Create, rename, and delete watchlists. Add and remove tickers. Watchlists persist to disk as JSON files.
- **Data Updates** -- Refresh fundamental data by running the bundled Python script, which fetches from Yahoo Finance.

---

## Requirements

- C++17 compatible compiler (GCC, Clang, or MSVC)
- CMake 3.16 or higher
- libcurl (for live API requests)
- nlohmann-json (for JSON parsing)
- Python 3 (for data update scripts)

---

## System Dependencies

### Ubuntu / WSL

```bash
sudo apt update
sudo apt install -y build-essential cmake libcurl4-openssl-dev nlohmann-json3-dev libssl-dev python3 python3-pip
```

### macOS

```bash
brew install cmake curl nlohmann-json python3
```

### MSYS2 (Windows)

```bash
pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-curl mingw-w64-x86_64-nlohmann-json
```

---

## Python Setup

Vestify includes Python scripts for fetching and updating stock data. Install dependencies from the project root:

```bash
pip3 install -r scripts/requirements.txt
```

On newer Ubuntu/WSL systems that restrict pip, use:

```bash
pip3 install --break-system-packages -r scripts/requirements.txt
```

---

## Environment Variables

Live data fetching requires an Alpha Vantage API key. Create a `.env` file in the `Vestify/` directory:

```
ALPHAVANTAGE_API_KEY=your_api_key_here
```

The `.env` file is listed in `.gitignore` and must never be committed to version control.

To load the key in your terminal session:

```bash
# Linux / macOS / WSL
set -a && source .env && set +a

# PowerShell (Windows)
$env:ALPHAVANTAGE_API_KEY="your_api_key_here"
```

---

## Build and Run

### Using Make

```bash
make run
```

This configures the project with CMake, builds the executable, and runs it.

### Using CMake directly

```bash
cmake -S . -B build
cmake --build build
./build/vestify
```

### Clean build

```bash
make clean
```

---

## Running Tests

Tests are built automatically with the project. Run them from the build directory:

```bash
cd build
ctest --output-on-failure
```

Alternatively, run individual test executables directly:

```bash
./build/tests/scoring_tests
./build/tests/backtest_tests
./build/tests/watchlist_tests
./build/tests/csv_load_tests
./build/tests/stock_screener_tests
./build/tests/stock_printer_tests
./build/tests/screening_menu_tests
```

If CMake is not available, scoring tests can be compiled and run directly:

```bash
g++ -std=c++17 -Wall -Wextra -I include -o scoring_tests tests/test_scoring.cpp
./scoring_tests
```

### Test Coverage

| Test Suite | What It Verifies |
|---|---|
| scoring_tests | Strategy presets, model scoring, ranking order, factor breakdowns, missing data handling |
| backtest_tests | Equity curve generation, rebalancing intervals, missing data error handling |
| watchlist_tests | Watchlist persistence, JSON save/load, corrupted file recovery |
| csv_load_tests | CSV parsing, malformed row handling, missing file errors |
| stock_screener_tests | Filtering by sector, valuation, and financial metrics |
| stock_printer_tests | Output formatting and display correctness |
| screening_menu_tests | Screening menu interaction flow |

---

## Usage

When the application starts, it presents an interactive menu:

```
Vestify
1. Load stock data
2. Update stock data
3. Print stock data
4. Search ticker
5. Print available stocks
6. Top 10 cheapest (EV/FCF)
7. Watchlist Menu
8. Score and Rank Stocks
9. Screen Stocks
10. Backtest (Historical)
11. Exit
```

A typical workflow is to load stock data (option 1), then score and rank (option 8) to see which stocks rank highest under different strategies.

### Scoring Strategies

The scoring system uses five factor models, each evaluating different aspects of a stock:

| Model | What It Measures | Key Metrics Used |
|---|---|---|
| Value | Whether a stock is underpriced | P/E ratio, dividend yield, EV/FCF, FCF yield |
| Growth | Earnings and revenue growth potential | P/E ratio, operating margin, ROE, dividend yield |
| Momentum | Profitability trends and cash flow strength | ROA, FCF yield, net margin |
| Quality | Financial health and balance sheet strength | Current ratio, debt-to-equity, gross margin, ROE |
| Efficiency | Capital efficiency and operational performance | ROA, operating margin, EV/EBIT |

Predefined strategy presets combine these models with different weights:

| Strategy | Focus | Dominant Model Weight |
|---|---|---|
| Value | Finding cheap stocks | Value at 45% |
| Growth | Finding high-growth companies | Growth at 45% |
| Momentum | Finding strong profitability trends | Momentum at 40% |
| Quality | Finding financially safe companies | Quality at 45% |
| Balanced | No single bias | All models at 20% |

Users can also define custom weights at runtime.

---

## Repository Structure

```
Vestify/
├── include/
│   ├── backtest/          # Backtest engine
│   ├── core/              # Stock, StockRepository, WatchList
│   ├── data/              # CSVLoader, LiveDataProvider
│   ├── persistence/       # WatchListRepo (JSON persistence)
│   ├── scoring/           # ScoringModel, strategy presets, 5 scoring models
│   ├── screening/         # StockScreener
│   └── ui/                # InteractiveMenu, ScreeningMenu, StockPrinter
├── src/
│   ├── backtest/          # BacktestEngine implementation
│   ├── core/              # StockRepository, WatchList implementations
│   ├── data/              # LiveDataProvider implementation
│   ├── persistence/       # WatchlistRepo implementation
│   ├── screening/         # StockScreener implementation
│   ├── ui/                # Menu and display implementations
│   └── main.cpp           # Application entry point
├── tests/                 # Unit and integration tests
├── scripts/               # Python data update scripts
├── data/                  # Stock CSV data and historical prices
├── CMakeLists.txt
├── Makefile
└── README.md
```

---

## Notes

- The `.env` file, `build/` directory, and Python virtual environments are excluded from version control via `.gitignore`.
- Stock data is bundled in `data/nasdaq100_fundamentals_full.csv` so the application works offline without API access.
- Historical price data for backtesting is stored in `data/historical/` as per-ticker CSV files.
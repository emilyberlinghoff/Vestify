# Vestify

Vestify is a C++17 stock analysis application for Linux/WSL that loads bundled stock fundamentals from CSV, lets users screen and rank stocks interactively, manages persistent watchlists, and runs historical backtests with periodic rebalancing.

The current application launches through a single interactive terminal menu. The older command-line argument mode has been removed.

---

## Features

- Load bundled stock fundamentals from `data/nasdaq100_fundamentals_full.csv`
- Screen stocks with multiple criteria at once
- Choose sectors from a generated list instead of typing them manually
- Print the current screening filters before running a screen
- Rank stocks with five factor models: Value, Growth, Momentum, Quality, and Efficiency
- Use predefined strategy presets or create custom scoring weights at runtime
- Inspect per-factor score breakdowns for ranked stocks
- Manage multiple persistent watchlists stored as JSON
- Run historical backtests with periodic rebalancing
- Refresh the fundamentals CSV with the bundled Python update script

---

## Platform Support

Vestify is intended to be built and run on:

- Linux
- WSL

The codebase and build instructions below assume a Linux-style environment. Windows-native and macOS setup instructions have been removed because they are no longer the target deployment path.

---

## Requirements

- C++17-compatible compiler
- CMake 3.16 or newer
- `make`
- libcurl development package
- nlohmann-json development package
- OpenSSL development package
- Python 3 with `pip`

---

## New Device Setup

These steps are enough to get the project working on a fresh Ubuntu or WSL machine.

### 1. Install system packages

```bash
sudo apt update
sudo apt install -y build-essential cmake make libcurl4-openssl-dev nlohmann-json3-dev libssl-dev python3 python3-pip
```

### 2. Clone the repository

```bash
git clone <your-repo-url>
cd Vestify
```

### 3. Install Python dependencies for the data update script

```bash
pip3 install -r scripts/requirements.txt
```

If your WSL or Linux environment blocks system-wide `pip` installs, use:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r scripts/requirements.txt
```

If you are intentionally installing outside a virtual environment on newer Ubuntu/WSL systems, this may also work:

```bash
pip3 install --break-system-packages -r scripts/requirements.txt
```

### 4. Build the project

```bash
cmake -S . -B build
cmake --build build
```

### 5. Run the application

```bash
./build/vestify
```

---

## Optional Environment Variables

Vestify can fetch historical price data from Alpha Vantage for backtesting. To enable that, create a `.env` file in the project root:

```env
ALPHAVANTAGE_API_KEY=your_api_key_here
```

The `.env` file is already ignored by Git and should not be committed.

To load it manually in a Linux/WSL shell:

```bash
set -a
source .env
set +a
```

If you do not set an API key, the app can still:

- load the bundled CSV
- screen stocks
- rank stocks
- manage watchlists

Backtesting that depends on missing historical data may fail until cached price files exist or an API key is provided.

---

## Build and Run

### Using Make

```bash
make build
make run
```

`make run` builds the project and starts the interactive menu.

### Using CMake directly

```bash
cmake -S . -B build
cmake --build build
./build/vestify
```

### Clean the build directory

```bash
make clean
```

---

## Running Tests

Build the project first:

```bash
cmake -S . -B build
cmake --build build
```

Then run the full suite:

```bash
ctest --test-dir build --output-on-failure
```

Run a single suite:

```bash
ctest --test-dir build --output-on-failure -R stock_screener_tests
```

You can also run the test executables directly:

```bash
./build/tests/scoring_tests
./build/tests/watchlist_tests
./build/tests/backtest_tests
./build/tests/csv_load_tests
./build/tests/stock_screener_tests
./build/tests/stock_printer_tests
./build/tests/screening_menu_tests
```

### Test Coverage

| Test Suite | What It Verifies |
|---|---|
| `scoring_tests` | Strategy presets, factor scores, ranking behavior, custom weights |
| `watchlist_tests` | Watchlist CRUD behavior and JSON persistence |
| `backtest_tests` | Equity curve generation, rebalance handling, error cases |
| `csv_load_tests` | CSV import behavior and malformed-row handling |
| `stock_screener_tests` | Multi-criteria screening logic |
| `stock_printer_tests` | Stock display and ranking output formatting |
| `screening_menu_tests` | Interactive screening menu flow |

---

## Usage

Start the app with:

```bash
./build/vestify
```

The application opens an interactive menu similar to:

```text
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

A typical workflow is:

1. Load the bundled CSV data
2. Screen or rank stocks
3. Save interesting tickers into watchlists
4. Run a backtest on a chosen strategy

---

## Scoring Models

Vestify includes five factor models:

| Model | Main Focus | Example Metrics |
|---|---|---|
| Value | Cheap valuation and cash generation | `P/E`, dividend yield, `EV/FCF`, `FCF yield` |
| Growth | Reinvestment and scalable profitability | `P/E`, operating margin, `ROE`, dividend yield |
| Momentum | Profitability and cash-flow strength proxies | `ROA`, `FCF yield`, net margin |
| Quality | Balance-sheet health and operating quality | current ratio, debt-to-equity, gross margin, `ROE` |
| Efficiency | Capital and operating efficiency | `ROA`, operating margin, `EV/EBIT` |

Predefined strategy presets combine those models with different weights, and users can also create a custom weighting interactively.

---

## Stock Screening

The screening flow supports combining multiple filters with AND semantics. Current filters include:

- sector
- price range
- max `P/E`
- max `P/B`
- max `P/S`
- max `EV/FCF`
- min `FCF yield`
- min `ROE`
- min operating margin
- max debt-to-equity
- min current ratio
- min dividend yield

The screening menu also supports:

- selecting sectors from a generated list
- printing the current active filter set before running the screen

---

## Data Update Script

The `Update stock data` menu option runs:

```bash
python3 scripts/update_data.py
```

That script uses `yfinance` to rebuild:

```text
data/nasdaq100_fundamentals_full.csv
```

Python dependencies for that script are listed in:

```text
scripts/requirements.txt
```

---

## Project Structure

```text
Vestify/
|-- include/
|   |-- backtest/       # Backtest engine interfaces
|   |-- core/           # Stock, StockRepository, WatchList
|   |-- data/           # CSVLoader, LiveDataProvider
|   |-- persistence/    # WatchListRepo
|   |-- scoring/        # ScoringModel, presets, factor models
|   |-- screening/      # StockScreener
|   `-- ui/             # InteractiveMenu, ScreeningMenu, StockPrinter
|-- src/
|   |-- backtest/
|   |-- core/
|   |-- data/
|   |-- persistence/
|   |-- screening/
|   |-- ui/
|   `-- main.cpp
|-- tests/
|-- scripts/
|-- data/
|-- build/              # Generated locally after configuration
|-- CMakeLists.txt
|-- Makefile
`-- README.md
```

---

## Notes

- The project is intended for Linux/WSL use.
- `build/`, `.env`, and virtual environments are ignored by Git.
- Bundled fundamentals data allows most of the application to work offline.
- Historical price files are stored under `data/historical/`.
- Backtesting is most reliable after historical data has been cached or when an Alpha Vantage API key is available.

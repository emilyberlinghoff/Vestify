# Vestify

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Live Quotes (Alpha Vantage)

Vestify uses Alpha Vantage for live quotes. You must provide an API key via an environment variable.

1. Create a `.env` file (ignored by git):

```bash
ALPHAVANTAGE_API_KEY=your_api_key_here
```

2. Load the key into your shell (once per terminal session):

```bash
set -a
source .env
set +a
```

3. Run the app:

```bash
./build/vestify --live AAPL,MSFT
```

If you open a new terminal, repeat step 2 before running the app.

## Tests

Run tests from the build directory:

```bash
cd build
ctest
```

Current tests cover the CSV loader acceptance criteria:
- Valid CSV path loads stocks with correct fields.
- Missing file returns a clear error without crashing.
- Malformed row is skipped and an error is reported.

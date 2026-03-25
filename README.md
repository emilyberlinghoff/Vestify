# Vestify

Vestify is a C++ stock analysis tool that loads stock data from CSV files or live APIs and provides screening and analysis through a command-line interface.

The project separates data providers, core logic, services, and UI to make the system modular and extensible.

---

# Requirements

- C++17 compatible compiler
- CMake ≥ 3.16
- Python 3
- libcurl (for live data requests)
- nlohmann-json3-dev (for JSON parsing)

---

# System Dependencies

Before building Vestify, install the required system packages.

### Ubuntu / WSL

```bash
sudo apt update
sudo apt install build-essential cmake libcurl4-openssl-dev nlohmann-json3-dev
```

These packages provide:

- **libcurl** — used for live API requests
- **nlohmann-json3-dev** — JSON parsing library used for API responses and watchlist persistence

If this package is missing, CMake may fail with an error similar to:

```
Could not find a package configuration file provided by "nlohmann_json"
```

Installing `nlohmann-json3-dev` resolves this issue.

---

## Quick WSL build helper

If you're building under WSL (Ubuntu), there's a small helper script to configure and build the project quickly.

From the project root run:

```bash
./scripts/build_wsl.sh
```

This script will create the `build/` directory, run CMake, and build the project using all available CPU cores. After it finishes you can run the program with:

```bash
./build/vestify
```


# Python Setup (for data update scripts)

Vestify includes Python scripts used to fetch and update stock data.

## 1. Create a virtual environment

From the project root:

```bash
python3 -m venv .venv
```

## 2. Activate the environment

```bash
source .venv/bin/activate
```

Your shell prompt should now start with:

```
(.venv)
```

## 3. Install Python dependencies

```bash
pip install -r scripts/requirements.txt
```

---

# Build and Run

Vestify uses CMake and a simple Makefile wrapper.

## Run the program

```bash
make run
```

This will:

- Configure the project with CMake
- Build the executable
- Run the application

## Clean the build directory

```bash
make clean
```

This removes the `build/` directory so the project can be rebuilt from scratch.

---

# Running Tests

Tests are built automatically when the project is compiled.

Run them with:

```bash
cd build
ctest
```

Current tests verify:

- CSV files load correctly
- Missing CSV files return a clear error
- Malformed rows are skipped without crashing

---

# Repository Structure

```
Vestify/
├── include/        # Header files
├── src/
│   ├── core/       # Core business logic
│   ├── data/       # Data providers
│   ├── services/   # Application services
│   └── ui/         # Command line interface
├── scripts/        # Python scripts for data updates
├── tests/          # Unit tests
├── build/          # Build output (ignored by git)
├── CMakeLists.txt
├── Makefile
└── README.md
```

---

# Notes

- `.venv` and `build/` are ignored by Git.
- Always activate the Python virtual environment before running scripts that depend on Python packages.

If you open a new terminal, run:

```bash
source .venv/bin/activate
```

again before using the scripts.

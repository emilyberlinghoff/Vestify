#include <iostream>
#include <string>
#include <vector>

#include "core/StockRepository.hpp"
#include "data/LiveDataProvider.hpp"

/**
 * @brief Print CLI usage instructions.
 *
 * @param exe Program name.
 */
static void printUsage(const std::string& exe) {
    std::cout << "Usage:\n";
    std::cout << "  " << exe << " --load-csv <path>\n";
    std::cout << "  " << exe << " --live <ticker[,ticker...]>\n";
    std::cout << "  " << exe << " --help\n";
    std::cout << "Environment:\n";
    std::cout << "  ALPHAVANTAGE_API_KEY=your_api_key\n";
}

/**
 * @brief Split a comma-separated ticker list.
 *
 * @param input Comma-separated tickers.
 * @return Vector of ticker symbols.
 */
static std::vector<std::string> splitTickers(const std::string& input) {
    std::vector<std::string> tickers;
    std::string current;
    for (char c : input) {
        if (c == ',') {
            if (!current.empty()) {
                tickers.push_back(current);
                current.clear();
            }
            continue;
        }
        current.push_back(c);
    }
    if (!current.empty()) {
        tickers.push_back(current);
    }
    return tickers;
}

/**
 * @brief Print loaded stock details to stdout.
 *
 * @param stocks Vector of stocks to print.
 */
static void printStocks(const std::vector<Stock>& stocks) {
    for (const auto& stock : stocks) {
        std::cout << "Ticker: " << stock.ticker << "\n";
        if (!stock.name.empty()) {
            std::cout << "  Name: " << stock.name << "\n";
        }
        if (!stock.sector.empty()) {
            std::cout << "  Sector: " << stock.sector << "\n";
        }
        if (stock.price > 0.0) {
            std::cout << "  Price: " << stock.price << "\n";
        } else {
            std::cout << "  Price: N/A\n";
        }
        if (stock.market_cap > 0.0) {
            std::cout << "  Market Cap: " << stock.market_cap << "\n";
        }
        if (stock.pe_ratio > 0.0) {
            std::cout << "  PE Ratio: " << stock.pe_ratio << "\n";
        }
        if (stock.dividend_yield > 0.0) {
            std::cout << "  Dividend Yield: " << stock.dividend_yield << "\n";
        }
    }
}

/**
 * @brief Vestify entry point.
 *
 * Prints a minimal CLI menu stub to satisfy initial build/run acceptance tests.
 *
 * @return Exit status code.
 */
int main(int argc, char** argv) {
    if (argc >= 2) {
        std::string arg = argv[1];
        if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
        if (arg == "--load-csv") {
            if (argc < 3) {
                std::cerr << "Missing CSV path.\n";
                printUsage(argv[0]);
                return 1;
            }

            StockRepository repo;
            auto result = repo.loadFromCsv(argv[2]);

            for (const auto& error : result.errors) {
                std::cerr << error << "\n";
            }

            std::cout << "Loaded " << result.stocks.size() << " stocks.\n";
            return result.errors.empty() ? 0 : 2;
        }
        if (arg == "--live") {
            if (argc < 3) {
                std::cerr << "Missing ticker list.\n";
                printUsage(argv[0]);
                return 1;
            }

            LiveDataProvider provider;
            auto result = provider.fetchQuotes(splitTickers(argv[2]));

            for (const auto& error : result.errors) {
                std::cerr << error << "\n";
            }

            std::cout << "Loaded " << result.stocks.size() << " live quotes.\n";
            printStocks(result.stocks);
            return result.errors.empty() ? 0 : 2;
        }
    }

    std::cout << "Vestify\n";
    std::cout << "1. Load stock data (CSV)\n";
    std::cout << "2. View watchlist\n";
    std::cout << "3. View portfolio\n";
    std::cout << "4. Exit\n";
    std::cout << "> ";

    return 0;
}

#include <iostream>
#include <string>
#include <vector>

#include "data/CSVLoader.hpp"
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

            CSVLoader loader;
            auto result = loader.loadFile(argv[2]);

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

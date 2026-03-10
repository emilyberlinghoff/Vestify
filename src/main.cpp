#include <iostream>
#include <string>

#include "data/CSVLoader.hpp"
#include "data/LiveDataProvider.hpp"
#include "ui/InteractiveMenu.hpp"
#include "ui/StockPrinter.hpp"

static void printUsage(const std::string& exe) {
    std::cout << "Usage:\n";
    std::cout << "  " << exe << " --load-csv <path>\n";
    std::cout << "  " << exe << " --live <ticker[,ticker...]>\n";
    std::cout << "  " << exe << " --help\n";
    std::cout << "Environment:\n";
    std::cout << "  ALPHAVANTAGE_API_KEY=your_api_key\n";
}

static std::vector<std::string> splitTickers(const std::string& input) {
    std::vector<std::string> tickers;
    std::string current;

    for (char c : input) {
        if (c == ',') {
            if (!current.empty()) {
                tickers.push_back(current);
                current.clear();
            }
        } else {
            current.push_back(c);
        }
    }

    if (!current.empty()) {
        tickers.push_back(current);
    }

    return tickers;
}

static int handleLiveQuotes(const std::string& tickerList) {
    LiveDataProvider provider;
    auto result = provider.fetchQuotes(splitTickers(tickerList));

    for (const auto& error : result.errors) {
        std::cerr << error << "\n";
    }

    std::cout << "\nLoaded " << result.stocks.size() << " live quotes.\n";
    StockPrinter::printStocks(result.stocks);

    return result.errors.empty() ? 0 : 2;
}

static int handleLoadCsvCli(const std::string& path) {
    CSVLoader loader;
    auto result = loader.loadFile(path);

    std::cout << "\nLoaded " << result.stocks.size() << " stocks.\n";
    StockPrinter::printStocks(result.stocks);

    for (const auto& error : result.errors) {
        std::cerr << error << "\n";
    }

    return result.errors.empty() ? 0 : 2;
}

int main(int argc, char** argv) {
    if (argc >= 2) {
        std::string arg = argv[1];

        if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }

        if (arg == "--load-csv") {
            if (argc < 3) {
                std::cerr << "--load-csv requires a path\n";
                return 1;
            }
            return handleLoadCsvCli(argv[2]);
        }

        if (arg == "--live") {
            if (argc < 3) {
                std::cerr << "--live requires a ticker list\n";
                return 1;
            }
            return handleLiveQuotes(argv[2]);
        }

        std::cerr << "Unknown argument: " << arg << "\n";
        printUsage(argv[0]);
        return 1;
    }

    InteractiveMenu menu;
    menu.run();
    return 0;
}
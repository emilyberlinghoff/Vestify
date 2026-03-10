#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "core/StockRepository.hpp"
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

    std::cout << std::fixed;

    for (const auto& stock : stocks) {

        std::cout << "\n========================================\n";
        std::cout << stock.ticker << " | " << stock.name << "\n";
        std::cout << "Sector: " << stock.sector << "\n";
        std::cout << "========================================\n";

        std::cout << std::setprecision(2);
        std::cout << "Price: $" << stock.price << "\n";

        std::cout << std::setprecision(0);
        std::cout << "Market Cap: $" << stock.market_cap << "\n";
        std::cout << "Revenue: $" << stock.revenue << "\n";
        std::cout << "Net Income: $" << stock.net_income << "\n";
        std::cout << "Free Cash Flow: $" << stock.free_cash_flow << "\n";

        std::cout << "\nBalance Sheet\n";
        std::cout << "Cash: $" << stock.cash << "\n";
        std::cout << "Total Debt: $" << stock.total_debt << "\n";
        std::cout << "Equity: $" << stock.total_equity << "\n";

        std::cout << "\nProfitability\n";
        std::cout << std::setprecision(4);
        std::cout << "ROE: " << stock.roe << "\n";
        std::cout << "ROA: " << stock.roa << "\n";
        std::cout << "Gross Margin: " << stock.gross_margin << "\n";

        std::cout << "\nValuation\n";
        std::cout << std::setprecision(2);
        std::cout << "PE: " << stock.pe_ratio << "\n";
        std::cout << "PB: " << stock.pb_ratio << "\n";
        std::cout << "PS: " << stock.ps_ratio << "\n";
        std::cout << "EV/EBIT: " << stock.ev_to_ebit << "\n";
        std::cout << "EV/FCF: " << stock.ev_to_fcf << "\n";
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
                std::cerr << "--load-csv requires a path\n";
                return 1;
            }

            CSVLoader loader;
            auto result = loader.loadFile(argv[2]);

            std::cout << "Loaded " << result.stocks.size() << " stocks.\n\n";
            printStocks(result.stocks);

            for (const auto& error : result.errors) {
                std::cerr << error << "\n";
            }

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
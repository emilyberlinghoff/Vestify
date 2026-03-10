#include "ui/InteractiveMenu.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>

#include "data/CSVLoader.hpp"
#include "data/LiveDataProvider.hpp"
#include "ui/StockPrinter.hpp"

std::string InteractiveMenu::toUpper(std::string str)
{
    std::transform(
        str.begin(),
        str.end(),
        str.begin(),
        [](unsigned char c)
        { return static_cast<char>(std::toupper(c)); });
    return str;
}

std::vector<std::string> InteractiveMenu::splitTickers(const std::string &input)
{
    std::vector<std::string> tickers;
    std::string current;

    for (char c : input)
    {
        if (c == ',')
        {
            if (!current.empty())
            {
                tickers.push_back(current);
                current.clear();
            }
        }
        else
        {
            current.push_back(c);
        }
    }

    if (!current.empty())
    {
        tickers.push_back(current);
    }

    return tickers;
}

int InteractiveMenu::handleLoadCsv(const std::string &path)
{
    CSVLoader loader;
    auto result = loader.loadFile(path);

    loadedStocks_ = result.stocks;

    std::cout << "\nLoaded " << loadedStocks_.size() << " stocks.\n";

    for (const auto &error : result.errors)
    {
        std::cerr << error << "\n";
    }

    return result.errors.empty() ? 0 : 2;
}

int InteractiveMenu::handleLiveQuotes(const std::string &tickerList)
{
    LiveDataProvider provider;
    auto result = provider.fetchQuotes(splitTickers(tickerList));

    for (const auto &error : result.errors)
    {
        std::cerr << error << "\n";
    }

    std::cout << "\nLoaded " << result.stocks.size() << " live quotes.\n";
    StockPrinter::printStocks(result.stocks);

    return result.errors.empty() ? 0 : 2;
}

void InteractiveMenu::updateStockData()
{
    std::cout << "\nUpdating stock data...\n";

    int result = std::system("python3 scripts/update_data.py");

    if (result == 0)
    {
        std::cout << "Stock data updated successfully.\n";
    }
    else
    {
        std::cout << "Stock data update failed.\n";
    }
}

void InteractiveMenu::searchTicker()
{
    if (loadedStocks_.empty())
    {
        std::cout << "No stock data loaded.\n";
        return;
    }

    std::string ticker;
    std::cout << "Enter ticker: ";
    std::cin >> ticker;

    ticker = toUpper(ticker);

    for (const auto &stock : loadedStocks_)
    {
        if (toUpper(stock.ticker) == ticker)
        {
            StockPrinter::printStocks({stock});
            return;
        }
    }

    std::cout << "Ticker not found.\n";
}

void InteractiveMenu::run()
{
    bool running = true;

    while (running)
    {
        std::cout << "\nVestify\n";
        std::cout << "1. Load stock data\n";
        std::cout << "2. Update stock data\n";
        std::cout << "3. Print stock data\n";
        std::cout << "4. Search ticker\n";
        std::cout << "5. Print available stocks\n";
        std::cout << "6. Top 10 cheapest (EV/FCF)\n";
        std::cout << "7. Exit\n";
        std::cout << "> ";

        int choice;
        std::cin >> choice;

        if (!std::cin)
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input.\n";
            continue;
        }

        switch (choice)
        {
        case 1:
            handleLoadCsv("data/nasdaq100_fundamentals_full.csv");
            break;

        case 2:
            updateStockData();
            break;

        case 3:
            if (loadedStocks_.empty())
            {
                std::cout << "No stock data loaded.\n";
            }
            else
            {
                StockPrinter::printStocks(loadedStocks_);
            }
            break;

        case 4:
            searchTicker();
            break;

        case 5:
            StockPrinter::printAvailableStocks(loadedStocks_);
            break;

        case 6:
            StockPrinter::printTopEVFCF(loadedStocks_, 10);
            break;

        case 7:
            std::cout << "Exiting Vestify.\n";
            running = false;
            break;

        default:
            std::cout << "Invalid option.\n";
            break;
        }
    }
}
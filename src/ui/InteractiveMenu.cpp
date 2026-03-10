#include "ui/InteractiveMenu.hpp"

#include <cstdlib>
#include <iostream>

#include "data/LiveDataProvider.hpp"
#include "ui/StockPrinter.hpp"

void InteractiveMenu::printUsage(const std::string &exe)
{
    std::cout << "Usage:\n";
    std::cout << "  " << exe << " --load-csv <path>\n";
    std::cout << "  " << exe << " --live <ticker[,ticker...]>\n";
    std::cout << "  " << exe << " --help\n";
    std::cout << "Environment:\n";
    std::cout << "  ALPHAVANTAGE_API_KEY=your_api_key\n";
}

std::string InteractiveMenu::toUpper(std::string str)
{
    for (char &c : str)
    {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
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

int InteractiveMenu::handleCommandLine(int argc, char **argv)
{
    std::string arg = argv[1];

    if (arg == "--help")
    {
        printUsage(argv[0]);
        return 0;
    }

    if (arg == "--load-csv")
    {
        if (argc < 3)
        {
            std::cerr << "--load-csv requires a path\n";
            return 1;
        }
        return handleLoadCsv(argv[2]);
    }

    if (arg == "--live")
    {
        if (argc < 3)
        {
            std::cerr << "--live requires a ticker list\n";
            return 1;
        }
        return handleLiveQuotes(argv[2]);
    }

    std::cerr << "Unknown argument: " << arg << "\n";
    printUsage(argv[0]);
    return 1;
}

int InteractiveMenu::handleLoadCsv(const std::string &path)
{
    auto result = repository_.loadFromCsv(path);

    std::cout << "\nLoaded " << repository_.getAll().size() << " stocks.\n";

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

    repository_.setStocks(result.stocks);

    for (const auto &error : result.errors)
    {
        std::cerr << error << "\n";
    }

    std::cout << "\nLoaded " << repository_.getAll().size() << " live quotes.\n";
    StockPrinter::printStocks(repository_.getAll());

    return result.errors.empty() ? 0 : 2;
}

void InteractiveMenu::printLoadedStocks() const
{
    if (repository_.getAll().empty())
    {
        std::cout << "No stock data loaded.\n";
        return;
    }

    StockPrinter::printStocks(repository_.getAll());
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

void InteractiveMenu::searchTicker() const
{
    if (repository_.getAll().empty())
    {
        std::cout << "No stock data loaded.\n";
        return;
    }

    std::string ticker;
    std::cout << "Enter ticker: ";
    std::cin >> ticker;

    const Stock *stock = repository_.findByTicker(ticker);

    if (stock == nullptr)
    {
        std::cout << "Ticker not found.\n";
        return;
    }

    StockPrinter::printStocks({*stock});
}

void InteractiveMenu::runInteractive()
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
            printLoadedStocks();
            break;
        case 4:
            searchTicker();
            break;
        case 5:
            StockPrinter::printAvailableStocks(repository_.getAll());
            break;
        case 6:
            StockPrinter::printTopEVFCF(repository_.getAll(), 10);
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

int InteractiveMenu::run(int argc, char **argv)
{
    if (argc >= 2)
    {
        return handleCommandLine(argc, argv);
    }

    runInteractive();
    return 0;
}
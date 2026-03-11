/**
 * @file InteractiveMenu.cpp
 * @brief Implementation of the interactive command-line interface for the Vestify application.
 *
 * This file contains the implementation of the InteractiveMenu class which provides
 * both command-line and interactive menu interfaces for the stock analysis application.
 * It handles user input, coordinates data loading and display operations, and manages
 * the application workflow.
 */

#include "ui/InteractiveMenu.hpp"

#include <cstdlib>
#include <iostream>
#include <limits>

#include "data/LiveDataProvider.hpp"
#include "ui/StockPrinter.hpp"

/**
 * @brief Construct the interactive menu and initialize watchlist persistence.
 *
 * Creates the watchlist repository using the demo watchlist JSON path and
 * attempts to load any persisted watchlist data into memory.
 */
InteractiveMenu::InteractiveMenu()
    : watchlistRepo_("src/persistence/demo_watchlist.json")
{
    loadDemoWatchlist();
}

/**
 * @brief Prints command-line usage information to stdout.
 *
 * Displays the available command-line options and required environment variables
 * for running the Vestify application from the command line.
 *
 * @param exe The executable name/path to display in usage examples.
 */
void InteractiveMenu::printUsage(const std::string &exe)
{
    std::cout << "Usage:\n";
    std::cout << "  " << exe << " --load-csv <path>\n";
    std::cout << "  " << exe << " --live <ticker[,ticker...]>\n";
    std::cout << "  " << exe << " --help\n";
    std::cout << "Environment:\n";
    std::cout << "  ALPHAVANTAGE_API_KEY=your_api_key\n";
}

/**
 * @brief Converts a string to uppercase.
 *
 * This utility function converts all characters in the input string to uppercase
 * using the standard library toupper function.
 *
 * @param str The input string to convert.
 * @return A new string with all characters converted to uppercase.
 */
std::string InteractiveMenu::toUpper(std::string str)
{
    for (char &c : str)
    {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return str;
}

/**
 * @brief Reads an integer value from standard input.
 *
 * Prompts the user for input and attempts to read an integer. If the input is
 * invalid, it clears the error state and ignores the rest of the line, then
 * prompts again until a valid integer is entered.
 *
 * @return The integer value entered by the user.
 */
int InteractiveMenu::readInt()
{
    int value;
    std::cin >> value;

    while (!std::cin)
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Enter a number: ";
        std::cin >> value;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}

/**
 * @brief Reads a full line of text from standard input.
 *
 * Displays the provided prompt and reads the entire user input line,
 * including spaces, until a newline is encountered.
 *
 * @param prompt Message displayed before reading input.
 * @return The line entered by the user.
 */
std::string InteractiveMenu::readLine(const std::string &prompt)
{
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
}

/**
 * @brief Splits a comma-separated string of tickers into individual ticker symbols.
 *
 * Parses a comma-delimited string and extracts individual ticker symbols,
 * trimming any whitespace and handling empty entries.
 *
 * @param input A comma-separated string of ticker symbols.
 * @return A vector of individual ticker symbol strings.
 */
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

/**
 * @brief Processes command-line arguments and executes the appropriate action.
 *
 * Parses command-line arguments and dispatches to the appropriate handler
 * function based on the provided options (--help, --load-csv, --live).
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return Exit code (0 for success, non-zero for errors).
 */
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

/**
 * @brief Loads stock data from a CSV file.
 *
 * Attempts to load stock data from the specified CSV file path using the
 * stock repository. Reports the number of stocks loaded and any errors
 * that occurred during loading.
 *
 * @param path Path to the CSV file containing stock data.
 * @return Exit code (0 for success, 2 if errors occurred during loading).
 */
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

/**
 * @brief Fetches live stock quotes for the specified tickers.
 *
 * Uses the LiveDataProvider to fetch current stock quotes for the given
 * list of ticker symbols. Updates the repository with the fetched data
 * and displays the results.
 *
 * @param tickerList Comma-separated string of ticker symbols.
 * @return Exit code (0 for success, 2 if errors occurred during fetching).
 */
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

/**
 * @brief Prints detailed information for all loaded stocks.
 *
 * Displays comprehensive stock data for all stocks currently loaded in the
 * repository. If no stocks are loaded, displays an appropriate message.
 */
void InteractiveMenu::printLoadedStocks() const
{
    if (repository_.getAll().empty())
    {
        std::cout << "No stock data loaded.\n";
        return;
    }

    StockPrinter::printStocks(repository_.getAll());
}

/**
 * @brief Updates stock data by running the update script.
 *
 * Executes the Python script responsible for updating stock data and reports
 * the success or failure of the operation.
 */
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

/**
 * @brief Searches for and displays information about a specific stock ticker.
 *
 * Prompts the user to enter a ticker symbol, searches the repository for it,
 * and displays detailed information if found. If no stocks are loaded or the
 * ticker is not found, displays appropriate error messages.
 */
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

    ticker = toUpper(ticker);

    const Stock *stock = repository_.findByTicker(ticker);

    if (stock == nullptr)
    {
        std::cout << "Ticker not found.\n";
        return;
    }

    StockPrinter::printStocks({*stock});
}

/**
 * @brief Load the persisted demo watchlist from disk.
 *
 * Reads the configured watchlist file using the watchlist repository and
 * appends the loaded watchlist to the in-memory collection if it contains
 * either a name or ticker data.
 */
void InteractiveMenu::loadDemoWatchlist()
{
    auto result = watchlistRepo_.load();

    for (const auto &error : result.errors)
    {
        std::cerr << error << "\n";
    }

    if (!result.watchlist.empty() || !result.watchlist.getName().empty())
    {
        watchlists_.push_back(result.watchlist);
    }
}

/**
 * @brief Save the current demo watchlist to disk.
 *
 * Persists the first watchlist in the in-memory collection using the
 * watchlist repository. If no watchlists are present, no save is attempted.
 */
void InteractiveMenu::saveDemoWatchlist() const
{
    if (watchlists_.empty())
    {
        return;
    }

    auto result = watchlistRepo_.save(watchlists_[0]);
    if (!result.ok)
    {
        std::cerr << result.errMsg << "\n";
    }
}

/**
 * @brief Print the names of all available watchlists.
 *
 * Displays each watchlist in the in-memory collection with a 1-based index.
 * If no watchlists exist, an informative message is printed instead.
 */
void InteractiveMenu::printAllWatchlists() const
{
    if (watchlists_.empty())
    {
        std::cout << "No watchlists created.\n";
        return;
    }

    std::cout << "\nWatchlists:\n";
    for (std::size_t i = 0; i < watchlists_.size(); ++i)
    {
        std::cout << i + 1 << ". " << watchlists_[i].getName() << "\n";
    }
}

/**
 * @brief Prompt the user to choose a watchlist by index.
 *
 * Displays all watchlists, reads the user's numeric choice, and validates
 * that the selection falls within the current range.
 *
 * @return Zero-based index of the selected watchlist, or -1 on failure.
 */
int InteractiveMenu::selectWatchlistIndex() const
{
    if (watchlists_.empty())
    {
        std::cout << "No watchlists available.\n";
        return -1;
    }

    printAllWatchlists();
    std::cout << "Select watchlist number: ";
    int choice = readInt();

    if (choice < 1 || choice > static_cast<int>(watchlists_.size()))
    {
        std::cout << "Invalid watchlist selection.\n";
        return -1;
    }

    return choice - 1;
}

/**
 * @brief Create a new watchlist.
 *
 * Prompts the user for a watchlist name and creates a new watchlist if the
 * name is non-empty and not already used by another watchlist.
 *
 * @return True if the watchlist was created, false otherwise.
 */
bool InteractiveMenu::createWatchlist()
{
    std::string name = readLine("Enter watchlist name: ");

    if (name.empty())
    {
        std::cout << "Watchlist name cannot be empty.\n";
        return false;
    }

    for (const auto &watchlist : watchlists_)
    {
        if (watchlist.getName() == name)
        {
            std::cout << "A watchlist with that name already exists.\n";
            return false;
        }
    }

    watchlists_.push_back(WatchList(name));
    std::cout << "Watchlist created.\n";
    return true;
}

/**
 * @brief Rename an existing watchlist.
 *
 * Prompts the user to select a watchlist and supply a new name. The rename
 * succeeds only if the selection is valid and the new name is non-empty and unique.
 *
 * @return True if the watchlist was renamed, false otherwise.
 */
bool InteractiveMenu::renameWatchlist()
{
    int index = selectWatchlistIndex();
    if (index == -1)
    {
        return false;
    }

    std::string newName = readLine("Enter new watchlist name: ");

    if (newName.empty())
    {
        std::cout << "Watchlist name cannot be empty.\n";
        return false;
    }

    for (std::size_t i = 0; i < watchlists_.size(); ++i)
    {
        if (static_cast<int>(i) != index && watchlists_[i].getName() == newName)
        {
            std::cout << "A watchlist with that name already exists.\n";
            return false;
        }
    }

    watchlists_[index].setName(newName);
    std::cout << "Watchlist renamed.\n";
    return true;
}

/**
 * @brief Delete an existing watchlist.
 *
 * Prompts the user to select a watchlist and removes it from the
 * in-memory collection if the selection is valid.
 *
 * @return True if the watchlist was deleted, false otherwise.
 */
bool InteractiveMenu::deleteWatchlist()
{
    int index = selectWatchlistIndex();
    if (index == -1)
    {
        return false;
    }

    std::cout << "Deleted watchlist: " << watchlists_[index].getName() << "\n";
    watchlists_.erase(watchlists_.begin() + index);
    return true;
}

/**
 * @brief Add a stock ticker to a selected watchlist.
 *
 * Prompts the user to choose a watchlist and enter a ticker symbol. The ticker
 * must exist in the repository and not already be present in the watchlist.
 *
 * @return True if the ticker was added, false otherwise.
 */
bool InteractiveMenu::addStockToWatchlist()
{
    if (repository_.getAll().empty())
    {
        std::cout << "No stock data loaded.\n";
        return false;
    }

    int index = selectWatchlistIndex();
    if (index == -1)
    {
        return false;
    }

    std::string ticker = readLine("Enter ticker to add: ");
    ticker = toUpper(ticker);

    const Stock *stock = repository_.findByTicker(ticker);
    if (stock == nullptr)
    {
        std::cout << "Ticker not found in loaded stock data.\n";
        return false;
    }

    if (!watchlists_[index].add(ticker))
    {
        std::cout << ticker << " is already in " << watchlists_[index].getName() << ".\n";
        return false;
    }

    std::cout << ticker << " added to " << watchlists_[index].getName() << ".\n";
    return true;
}

/**
 * @brief Remove a stock ticker from a selected watchlist.
 *
 * Prompts the user to choose a watchlist and enter a ticker symbol. The ticker
 * is removed only if it currently exists in the selected watchlist.
 *
 * @return True if the ticker was removed, false otherwise.
 */
bool InteractiveMenu::removeStockFromWatchlist()
{
    int index = selectWatchlistIndex();
    if (index == -1)
    {
        return false;
    }

    std::string ticker = readLine("Enter ticker to remove: ");
    ticker = toUpper(ticker);

    if (!watchlists_[index].remove(ticker))
    {
        std::cout << ticker << " was not found in " << watchlists_[index].getName() << ".\n";
        return false;
    }

    std::cout << ticker << " removed from " << watchlists_[index].getName() << ".\n";
    return true;
}

/**
 * @brief Print the stock contents of a single watchlist.
 *
 * Looks up each ticker in the supplied watchlist against the stock repository
 * and prints any matching stock records. If the watchlist is empty or none of
 * its tickers exist in the loaded stock data, an explanatory message is shown.
 *
 * @param watchlist The watchlist to display.
 */
void InteractiveMenu::printWatchlist(const WatchList &watchlist) const
{
    const auto &tickers = watchlist.getAll();

    if (tickers.empty())
    {
        std::cout << "Watchlist \"" << watchlist.getName() << "\" is empty.\n";
        return;
    }

    std::vector<Stock> stocksToPrint;

    for (const auto &ticker : tickers)
    {
        const Stock *stock = repository_.findByTicker(ticker);
        if (stock != nullptr)
        {
            stocksToPrint.push_back(*stock);
        }
    }

    if (stocksToPrint.empty())
    {
        std::cout << "No valid stock data found for this watchlist.\n";
        return;
    }

    std::cout << "\nWatchlist: " << watchlist.getName() << "\n";
    StockPrinter::printStocks(stocksToPrint);
}

/**
 * @brief Print the contents of all watchlists.
 *
 * Iterates through every watchlist currently stored in memory and displays
 * the stocks contained in each one.
 */
void InteractiveMenu::printAllWatchlistContents() const
{
    if (watchlists_.empty())
    {
        std::cout << "No watchlists created.\n";
        return;
    }

    for (const auto &watchlist : watchlists_)
    {
        printWatchlist(watchlist);
        std::cout << "\n";
    }
}

/**
 * @brief Open and display a single selected watchlist.
 *
 * Prompts the user to choose a watchlist and displays its contents if a
 * valid selection is made.
 */
void InteractiveMenu::openSingleWatchlist() const
{
    int index = selectWatchlistIndex();
    if (index == -1)
    {
        return;
    }

    printWatchlist(watchlists_[index]);
}

/**
 * @brief Run the watchlist modification submenu.
 *
 * Displays options for adding or removing stocks, renaming a watchlist,
 * deleting a watchlist, or returning to the previous menu. Successful
 * modifications trigger a save of the persisted demo watchlist.
 */
void InteractiveMenu::modifyWatchlistMenu()
{
    bool modifying = true;

    while (modifying)
    {
        std::cout << "\nModify Watchlist\n";
        std::cout << "1. Add stock\n";
        std::cout << "2. Remove stock\n";
        std::cout << "3. Rename watchlist\n";
        std::cout << "4. Delete watchlist\n";
        std::cout << "5. Return\n";
        std::cout << "> ";

        int choice = readInt();

        switch (choice)
        {
        case 1:
            if (addStockToWatchlist())
            {
                saveDemoWatchlist();
            }
            break;
        case 2:
            if (removeStockFromWatchlist())
            {
                saveDemoWatchlist();
            }
            break;
        case 3:
            if (renameWatchlist())
            {
                saveDemoWatchlist();
            }
            break;
        case 4:
            if (deleteWatchlist())
            {
                saveDemoWatchlist();
            }
            break;
        case 5:
            modifying = false;
            break;
        default:
            std::cout << "Invalid option.\n";
            break;
        }
    }
}

/**
 * @brief Run the main watchlist menu.
 *
 * Displays options for creating, modifying, printing, or opening watchlists.
 * Successful creation triggers a save of the persisted demo watchlist.
 */
void InteractiveMenu::watchlistMenu()
{
    bool inWatchlistMenu = true;

    while (inWatchlistMenu)
    {
        std::cout << "\nWatchlist Menu\n";
        std::cout << "1. Create watchlist\n";
        std::cout << "2. Modify watchlist\n";
        std::cout << "3. Print all watchlists\n";
        std::cout << "4. Open a watchlist\n";
        std::cout << "5. Return\n";
        std::cout << "> ";

        int choice = readInt();

        switch (choice)
        {
        case 1:
            if (createWatchlist())
            {
                saveDemoWatchlist();
            }
            break;
        case 2:
            modifyWatchlistMenu();
            break;
        case 3:
            printAllWatchlistContents();
            break;
        case 4:
            openSingleWatchlist();
            break;
        case 5:
            inWatchlistMenu = false;
            break;
        default:
            std::cout << "Invalid option.\n";
            break;
        }
    }
}

/**
 * @brief Runs the interactive menu loop.
 *
 * Displays a menu of options and processes user input in a loop until the
 * user chooses to exit. Provides access to all major application features
 * through numbered menu options.
 */
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
        std::cout << "7. Watchlist Menu\n";
        std::cout << "8. Exit\n";
        std::cout << "> ";

        int choice = readInt();

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
            watchlistMenu();
            saveDemoWatchlist();
            break;
        case 8:
            std::cout << "Exiting Vestify.\n";
            running = false;
            break;
        default:
            std::cout << "Invalid option.\n";
            break;
        }
    }
}

/**
 * @brief Main entry point for the InteractiveMenu system.
 *
 * Determines whether to run in command-line mode (if arguments provided)
 * or interactive mode (if no arguments). This is the primary interface
 * between the application and the user.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return Exit code (0 for success, non-zero for errors).
 */
int InteractiveMenu::run(int argc, char **argv)
{
    if (argc >= 2)
    {
        return handleCommandLine(argc, argv);
    }

    runInteractive();
    return 0;
}
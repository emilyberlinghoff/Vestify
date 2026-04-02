/**
 * @file InteractiveMenu.cpp
 * @brief Implementation of the interactive terminal interface for the Vestify application.
 *
 * This file contains the implementation of the InteractiveMenu class, which
 * drives the interactive menu for the stock analysis application. It handles
 * user input, coordinates data loading and display operations, and manages
 * the application workflow.
 */

#include "ui/InteractiveMenu.hpp"

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <fstream>

#include "backtest/BacktestEngine.hpp"
#include "data/LiveDataProvider.hpp"
#include "scoring/GrowthScoringModel.hpp"
#include "scoring/MomentumScoringModel.hpp"
#include "scoring/StrategyPresent.hpp"
#include "scoring/ValueScoringModel.hpp"
#include "scoring/QualityScoringModel.hpp"
#include "scoring/EfficiencyScoringModel.hpp"
#include "ui/ScreeningMenu.hpp"
#include "ui/StockPrinter.hpp"

/**
 * @brief Construct the interactive menu and initialize watchlist persistence.
 *
 * Creates the watchlist repository using the demo watchlist JSON path and
 * attempts to load any persisted watchlist data into memory.
 */
static void loadEnvFromFile(const std::string &path)
{
    std::ifstream infile(path);
    if (!infile.is_open())
    {
        return;
    }

    std::string line;
    while (std::getline(infile, line))
    {
        if (line.empty() || line[0] == '#')
        {
            continue;
        }
        auto pos = line.find('=');
        if (pos == std::string::npos)
        {
            continue;
        }
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        if (key.empty() || value.empty())
        {
            continue;
        }
        if (std::getenv(key.c_str()) == nullptr)
        {
            setenv(key.c_str(), value.c_str(), 1);
        }
    }
}

InteractiveMenu::InteractiveMenu()
    : watchlistRepo_("src/persistence/demo_watchlist.json")
{
    loadEnvFromFile(".env");
    //loadDemoWatchlist();
    loadWatchlists();
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
                std::string trimmed = current;
                const auto first = trimmed.find_first_not_of(" \t\r\n");
                if (first == std::string::npos)
                {
                    trimmed.clear();
                }
                else
                {
                    trimmed.erase(0, first);
                    const auto last = trimmed.find_last_not_of(" \t\r\n");
                    if (last != std::string::npos)
                    {
                        trimmed.erase(last + 1);
                    }
                }
                if (!trimmed.empty())
                {
                    tickers.push_back(trimmed);
                }
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
        std::string trimmed = current;
        const auto first = trimmed.find_first_not_of(" \t\r\n");
        if (first == std::string::npos)
        {
            trimmed.clear();
        }
        else
        {
            trimmed.erase(0, first);
            const auto last = trimmed.find_last_not_of(" \t\r\n");
            if (last != std::string::npos)
            {
                trimmed.erase(last + 1);
            }
        }
        if (!trimmed.empty())
        {
            tickers.push_back(trimmed);
        }
    }

    return tickers;
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

void InteractiveMenu::screenStocks()
{
    ScreeningMenu::run(
        repository_.getAll(),
        []()
        {
            return InteractiveMenu::readInt();
        },
        [](const std::string &prompt)
        {
            return InteractiveMenu::readLine(prompt);
        });
}

/**
 * @brief Load the persisted demo watchlist from disk.
 *
 * Reads the configured watchlist file using the watchlist repository and
 * appends the loaded watchlist to the in-memory collection if it contains
 * either a name or ticker data.
 */
// void InteractiveMenu::loadDemoWatchlist()
// {
//     auto result = watchlistRepo_.load();

//     for (const auto &error : result.errors)
//     {
//         std::cerr << error << "\n";
//     }

//     if (!result.watchlist.empty() || !result.watchlist.getName().empty())
//     {
//         watchlists_.push_back(result.watchlist);
//     }
// }

void InteractiveMenu::loadWatchlists()
{
    auto result = watchlistRepo_.loadAll();

    if (!result.errors.empty())
    {
        for (const auto& err : result.errors)
        {
            std::cerr << err << "\n";
        }
    }

    watchlists_ = result.watchlists;

    // Optional: ensure at least one exists
    if (watchlists_.empty())
    {
        watchlists_.push_back(WatchList("Default"));
    }
}

/**
 * @brief Save the current demo watchlist to disk.
 *
 * Persists the first watchlist in the in-memory collection using the
 * watchlist repository. If no watchlists are present, no save is attempted.
 */
// void InteractiveMenu::saveDemoWatchlist() const
// {
//     if (watchlists_.empty())
//     {
//         return;
//     }

//     auto result = watchlistRepo_.save(watchlists_[0]);
//     if (!result.ok)
//     {
//         std::cerr << result.errMsg << "\n";
//     }
// }

void InteractiveMenu::saveWatchlists() const
{
    auto result = watchlistRepo_.saveAll(watchlists_);

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
        std::cout << "Default watchlist created.\n";
        watchlists_.push_back(WatchList("Watchlist " + std::to_string(watchlists_.size() + 1)));
        return true;
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
bool InteractiveMenu::renameWatchlist(int index)
{
    if (index < 0 || index >= static_cast<int>(watchlists_.size()))
    {
        std::cout << "Invalid watchlist selection.\n";
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
bool InteractiveMenu::deleteWatchlist(int index)
{
    if (index < 0 || index >= static_cast<int>(watchlists_.size()))
    {
        std::cout << "Invalid watchlist selection.\n";
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
bool InteractiveMenu::addStockToWatchlist(int index)
{
    if (repository_.getAll().empty())
    {
        std::cout << "No stock data loaded.\n";
        return false;
    }

    if (index < 0 || index >= static_cast<int>(watchlists_.size()))
    {
        std::cout << "Invalid watchlist selection.\n";
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
bool InteractiveMenu::removeStockFromWatchlist(int index)
{
    if (index < 0 || index >= static_cast<int>(watchlists_.size()))
    {
        std::cout << "Invalid watchlist selection.\n";
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
    if (watchlists_.empty())
    {
        std::cout << "No watchlists available to modify.\n";
        return;
    }

    printAllWatchlists();
    std::cout << "\nSelect a watchlist to modify (0 to cancel): ";
    int selection = readInt();

    if (selection == 0)
    {
        return;
    }

    if (selection < 1 || selection > static_cast<int>(watchlists_.size()))
    {
        std::cout << "Invalid watchlist selection.\n";
        return;
    }

    int index = selection - 1;
    bool modifying = true;

    while (modifying)
    {
        if (watchlists_.empty() || index < 0 || index >= static_cast<int>(watchlists_.size()))
        {
            break;
        }

        std::cout << "\nModify Watchlist: " << watchlists_[index].getName() << "\n";
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
            if (addStockToWatchlist(index))
            {
                saveWatchlists();
            }
            break;

        case 2:
            if (removeStockFromWatchlist(index))
            {
                saveWatchlists();
            }
            break;

        case 3:
            if (renameWatchlist(index))
            {
                saveWatchlists();
            }
            break;

        case 4:
            if (deleteWatchlist(index))
            {
                saveWatchlists();
                modifying = false;
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
                //saveDemoWatchlist();
                saveWatchlists();
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
 * @brief Score and rank loaded stocks using a weighted scoring strategy.
 *
 * Allows the user to select a predefined scoring strategy, view ranked results,
 * and inspect a breakdown of factor contributions for a selected stock.
 */
void InteractiveMenu::scoreAndRankStocks()
{
    if (repository_.getAll().empty())
    {
        std::cout << "No stock data loaded.\n";
        return;
    }

    StrategyManager manager;
    ValueScoringModel valueModel;
    GrowthScoringModel growthModel;
    MomentumScoringModel momentumModel;
    QualityScoringModel qualityModel;
    EfficiencyScoringModel efficiencyModel;
    manager.registerModel(&qualityModel);
    manager.registerModel(&efficiencyModel);
    manager.registerModel(&valueModel);
    manager.registerModel(&growthModel);
    manager.registerModel(&momentumModel);

    bool rerun = true;
    while (rerun)
    {
        auto strategies = manager.getAvailableStrategies();

        std::cout << "\nScoring Strategies:\n";
        for (std::size_t i = 0; i < strategies.size(); ++i)
        {
            const auto &strategy = manager.getStrategy(strategies[i]);
            std::cout << i + 1 << ". " << strategy.name << " - " << strategy.description << "\n";
        }
        std::cout << "C. Custom weights\n";

        std::string selection = readLine(
            "Select strategy by number or name (blank for default \"" +
            manager.getActiveStrategyName() + "\"): ");

        if (!selection.empty())
        {
            std::stringstream ss(selection);
            int index = 0;
            if (ss >> index)
            {
                if (index >= 1 && index <= static_cast<int>(strategies.size()))
                {
                    manager.setActiveStrategy(strategies[index - 1]);
                }
                else
                {
                    std::cout << "Invalid selection. Using default.\n";
                }
            }
            else
            {
                std::string upper = toUpper(selection);
                if (upper == "C" || upper == "CUSTOM")
                {
                    std::string name = readLine("Custom strategy name (default \"Custom\"): ");
                    if (name.empty())
                    {
                        name = "Custom";
                    }

                    const auto &current = manager.getActiveStrategy().weights;
                    auto readWeight = [&](const std::string &label, double currentValue) {
                        std::ostringstream prompt;
                        prompt << "Enter weight for " << label << " (current "
                               << std::fixed << std::setprecision(2) << currentValue << "): ";
                        std::string input = readLine(prompt.str());
                        if (input.empty())
                        {
                            return currentValue;
                        }
                        std::stringstream weightStream(input);
                        double value = currentValue;
                        if (weightStream >> value)
                        {
                            if (value < 0.0)
                            {
                                std::cout << "Weight cannot be negative. Using 0.\n";
                                return 0.0;
                            }
                            return value;
                        }
                        std::cout << "Invalid input. Using current value.\n";
                        return currentValue;
                    };

                    double valueWeight = current.count("Value") ? current.at("Value") : 0.0;
                    double growthWeight = current.count("Growth") ? current.at("Growth") : 0.0;
                    double momentumWeight = current.count("Momentum") ? current.at("Momentum") : 0.0;

                    valueWeight = readWeight("Value", valueWeight);
                    growthWeight = readWeight("Growth", growthWeight);
                    momentumWeight = readWeight("Momentum", momentumWeight);

                    double sum = valueWeight + growthWeight + momentumWeight;
                    if (sum < 0.95 || sum > 1.05)
                    {
                        std::cout << "Warning: weights sum to " << std::fixed << std::setprecision(2)
                                  << sum << " (expected ~1.00).\n";
                    }

                    std::unordered_map<std::string, double> weights = {
                        {"Value", valueWeight},
                        {"Growth", growthWeight},
                        {"Momentum", momentumWeight}
                    };

                    if (!manager.setCustomStrategy(
                            name,
                            "User-defined weights for Value/Growth/Momentum.",
                            weights))
                    {
                        std::cout << "Failed to set custom strategy. Using default.\n";
                    }
                }
                else
                {
                bool found = false;
                for (const auto &name : strategies)
                {
                    if (toUpper(name) == upper)
                    {
                        manager.setActiveStrategy(name);
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    std::cout << "Unknown strategy. Using default.\n";
                }
                }
            }
        }

        auto ranked = manager.rankStocks(repository_.getAll());
        if (ranked.empty())
        {
            std::cout << "No stocks available to score.\n";
            return;
        }

        std::size_t displayCount = std::min<std::size_t>(10, ranked.size());
        std::string countInput = readLine("Enter number of results to display (default 10): ");
        if (!countInput.empty())
        {
            std::stringstream ss(countInput);
            int count = 0;
            if (ss >> count && count > 0)
            {
                displayCount = std::min<std::size_t>(static_cast<std::size_t>(count), ranked.size());
            }
        }

        std::cout << "\nRanked Stocks (" << manager.getActiveStrategyName() << "):\n";
        std::cout << std::left << std::setw(6) << "Rank"
                  << std::setw(8) << "Ticker"
                  << std::setw(30) << "Company"
                  << std::setw(8) << "Score"
                  << "\n";

        std::cout << std::string(52, '-') << "\n";

        std::cout << std::fixed << std::setprecision(2);
        for (std::size_t i = 0; i < displayCount; ++i)
        {
            const auto &entry = ranked[i];
            std::string name = entry.stock.name;
            if (name.size() > 28)
            {
                name = name.substr(0, 28) + "..";
            }
            std::cout << std::left << std::setw(6) << (i + 1)
                      << std::setw(8) << entry.stock.ticker
                      << std::setw(30) << name
                      << std::setw(8) << entry.composite
                      << "\n";
        }

        std::string breakdownInput = readLine(
            "\nView score breakdown by rank number or ticker (blank to return): ");
        if (!breakdownInput.empty())
        {
            const ScoredStock *selected = nullptr;
            std::stringstream ss(breakdownInput);
            int index = 0;
            if (ss >> index)
            {
                if (index >= 1 && index <= static_cast<int>(ranked.size()))
                {
                    selected = &ranked[index - 1];
                }
                else
                {
                    std::cout << "Invalid rank selection.\n";
                }
            }
            else
            {
                std::string ticker = toUpper(breakdownInput);
                for (const auto &entry : ranked)
                {
                    if (entry.stock.ticker == ticker)
                    {
                        selected = &entry;
                        break;
                    }
                }
                if (selected == nullptr)
                {
                    std::cout << "Ticker not found in ranked list.\n";
                }
            }

            if (selected != nullptr)
            {
                const auto &strategy = manager.getActiveStrategy();
                std::vector<std::string> factors;
                factors.reserve(strategy.weights.size());
                for (const auto &pair : strategy.weights)
                {
                    factors.push_back(pair.first);
                }
                std::sort(factors.begin(), factors.end());

                std::cout << "\nBreakdown for " << selected->stock.ticker
                          << " - " << selected->stock.name << "\n";
                std::cout << std::left << std::setw(12) << "Factor"
                          << std::setw(10) << "Weight"
                          << std::setw(10) << "Score"
                          << std::setw(14) << "Contribution"
                          << "\n";
                std::cout << std::string(46, '-') << "\n";

                double totalWeight = 0.0;
                double weightedSum = 0.0;
                for (const auto &factor : factors)
                {
                    double weight = strategy.weights.at(factor);
                    totalWeight += weight;
                    auto it = selected->subscores.find(factor);
                    double subscore = (it != selected->subscores.end()) ? it->second : 0.0;
                    double contribution = weight * subscore;
                    weightedSum += contribution;

                    std::cout << std::left << std::setw(12) << factor
                              << std::setw(10) << weight
                              << std::setw(10) << subscore
                              << std::setw(14) << contribution
                              << "\n";
                }

                double composite = (totalWeight > 0.0) ? (weightedSum / totalWeight) : 0.0;
                std::cout << "Composite score: " << composite << "\n";
            }
        }

        std::string again = readLine("Run scoring with another strategy? (y/n): ");
        rerun = (!again.empty() && (again[0] == 'y' || again[0] == 'Y'));
    }
}

void InteractiveMenu::runBacktest()
{
    if (repository_.getAll().empty())
    {
        std::cout << "No stock data loaded. Load fundamentals first.\n";
        return;
    }

    std::string tickerInput = readLine("Enter tickers to backtest (comma-separated): ");
    auto tickers = splitTickers(tickerInput);
    if (tickers.empty())
    {
        std::cout << "No tickers provided.\n";
        return;
    }

    std::string startDate = readLine("Start date (YYYY-MM-DD, blank for earliest): ");
    std::string endDate = readLine("End date (YYYY-MM-DD, blank for latest): ");

    int rebalanceInterval = 20;
    std::string intervalInput = readLine("Rebalance interval in trading days (default 20): ");
    if (!intervalInput.empty())
    {
        std::stringstream ss(intervalInput);
        int value = 0;
        if (ss >> value && value > 0)
        {
            rebalanceInterval = value;
        }
    }

    std::size_t topN = std::min<std::size_t>(5, tickers.size());
    std::string topInput = readLine("Top N holdings to keep (default 5): ");
    if (!topInput.empty())
    {
        std::stringstream ss(topInput);
        int value = 0;
        if (ss >> value && value > 0)
        {
            topN = std::min<std::size_t>(static_cast<std::size_t>(value), tickers.size());
        }
    }

    std::string fullInput = readLine("Use full history (slower, API rate limits)? (y/n): ");
    std::string outputsize = (!fullInput.empty() && (fullInput[0] == 'y' || fullInput[0] == 'Y'))
                                 ? "full"
                                 : "compact";

    StrategyManager manager;
    ValueScoringModel valueModel;
    GrowthScoringModel growthModel;
    MomentumScoringModel momentumModel;
    QualityScoringModel qualityModel;
    EfficiencyScoringModel efficiencyModel;
    manager.registerModel(&qualityModel);
    manager.registerModel(&efficiencyModel);
    manager.registerModel(&valueModel);
    manager.registerModel(&growthModel);
    manager.registerModel(&momentumModel);

    /// Strategy selection (presets or custom weights).
    auto strategies = manager.getAvailableStrategies();
    std::cout << "\nScoring Strategies:\n";
    for (std::size_t i = 0; i < strategies.size(); ++i)
    {
        const auto &strategy = manager.getStrategy(strategies[i]);
        std::cout << i + 1 << ". " << strategy.name << " - " << strategy.description << "\n";
    }
    std::cout << "C. Custom weights\n";

    std::string selection = readLine(
        "Select strategy by number or name (blank for default \"" +
        manager.getActiveStrategyName() + "\"): ");

    if (!selection.empty())
    {
        std::stringstream ss(selection);
        int index = 0;
        if (ss >> index)
        {
            if (index >= 1 && index <= static_cast<int>(strategies.size()))
            {
                manager.setActiveStrategy(strategies[index - 1]);
            }
            else
            {
                std::cout << "Invalid selection. Using default.\n";
            }
        }
        else
        {
            std::string upper = toUpper(selection);
            if (upper == "C" || upper == "CUSTOM")
            {
                std::string name = readLine("Custom strategy name (default \"Custom\"): ");
                if (name.empty())
                {
                    name = "Custom";
                }

                const auto &current = manager.getActiveStrategy().weights;
                auto readWeight = [&](const std::string &label, double currentValue) {
                    std::ostringstream prompt;
                    prompt << "Enter weight for " << label << " (current "
                           << std::fixed << std::setprecision(2) << currentValue << "): ";
                    std::string input = readLine(prompt.str());
                    if (input.empty())
                    {
                        return currentValue;
                    }
                    std::stringstream weightStream(input);
                    double value = currentValue;
                    if (weightStream >> value)
                    {
                        if (value < 0.0)
                        {
                            std::cout << "Weight cannot be negative. Using 0.\n";
                            return 0.0;
                        }
                        return value;
                    }
                    std::cout << "Invalid input. Using current value.\n";
                    return currentValue;
                };

                double valueWeight = current.count("Value") ? current.at("Value") : 0.0;
                double growthWeight = current.count("Growth") ? current.at("Growth") : 0.0;
                double momentumWeight = current.count("Momentum") ? current.at("Momentum") : 0.0;

                valueWeight = readWeight("Value", valueWeight);
                growthWeight = readWeight("Growth", growthWeight);
                momentumWeight = readWeight("Momentum", momentumWeight);

                double sum = valueWeight + growthWeight + momentumWeight;
                if (sum < 0.95 || sum > 1.05)
                {
                    std::cout << "Warning: weights sum to " << std::fixed << std::setprecision(2)
                              << sum << " (expected ~1.00).\n";
                }

                std::unordered_map<std::string, double> weights = {
                    {"Value", valueWeight},
                    {"Growth", growthWeight},
                    {"Momentum", momentumWeight}
                };

                if (!manager.setCustomStrategy(
                        name,
                        "User-defined weights for Value/Growth/Momentum.",
                        weights))
                {
                    std::cout << "Failed to set custom strategy. Using default.\n";
                }
            }
            else
            {
                bool found = false;
                for (const auto &name : strategies)
                {
                    if (toUpper(name) == upper)
                    {
                        manager.setActiveStrategy(name);
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    std::cout << "Unknown strategy. Using default.\n";
                }
            }
        }
    }

    std::unordered_map<std::string, std::vector<BacktestEngine::PricePoint>> priceSeries;
    std::vector<Stock> stocksForBacktest;

    LiveDataProvider provider;
    for (const auto &rawTicker : tickers)
    {
        std::string ticker = toUpper(rawTicker);
        const Stock *stock = repository_.findByTicker(ticker);
        if (stock == nullptr)
        {
            std::cout << "Ticker not found in fundamentals: " << ticker << "\n";
            return;
        }

        /// Use unadjusted daily series to avoid premium endpoint limitations.
        auto result = provider.fetchDailySeries(ticker, outputsize, false);
        if (!result.errors.empty())
        {
            for (const auto &error : result.errors)
            {
                std::cerr << error << "\n";
            }
            std::cout << "Backtest aborted due to missing historical data.\n";
            return;
        }

        if (result.points.empty())
        {
            std::cout << "No historical data for " << ticker << ".\n";
            return;
        }

        stocksForBacktest.push_back(*stock);
        std::vector<BacktestEngine::PricePoint> points;
        points.reserve(result.points.size());
        for (const auto &pt : result.points)
        {
            points.push_back({pt.date, pt.close});
        }
        priceSeries[ticker] = std::move(points);
    }

    BacktestEngine engine;
    BacktestEngine::Config config;
    config.stocks = stocksForBacktest;
    config.prices = priceSeries;
    config.start_date = startDate;
    config.end_date = endDate;
    config.rebalance_interval = rebalanceInterval;
    config.top_n = topN;

    auto backtest = engine.run(manager, config);
    if (!backtest.ok)
    {
        std::cout << "Backtest failed: " << backtest.error << "\n";
        return;
    }

    if (!backtest.equity_curve.empty())
    {
        const auto &first = backtest.equity_curve.front();
        const auto &last = backtest.equity_curve.back();
        double totalReturn = ((backtest.final_value / backtest.initial_value) - 1.0) * 100.0;
        std::cout << "\nBacktest Results\n";
        std::cout << "Strategy: " << manager.getActiveStrategyName() << "\n";
        std::cout << "Start: " << first.date << "  End: " << last.date << "\n";
        std::cout << "Initial: " << backtest.initial_value << "  Final: " << backtest.final_value << "\n";
        std::cout << "Total return: " << std::fixed << std::setprecision(2) << totalReturn << "%\n";
        std::cout << "Rebalances: " << backtest.rebalances << "\n";
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
        std::cout << "8. Score and Rank Stocks\n";
        std::cout << "9. Screen Stocks\n";
        std::cout << "10. Backtest (Historical)\n";
        std::cout << "11. Exit\n";
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
            //saveDemoWatchlist();
            saveWatchlists();
            break;
        case 8:
            scoreAndRankStocks();
            break;
        case 9:
            screenStocks();
            break;
        case 10:
            runBacktest();
            break;
        case 11:
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
 * Launches the application directly into the interactive menu loop.
 * This is the primary interface between the application and the user.
 *
 * @return Exit code (0 for success, non-zero for errors).
 */
int InteractiveMenu::run()
{
    runInteractive();
    return 0;
}

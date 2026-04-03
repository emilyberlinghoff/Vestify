/**
 * @file InteractiveMenu.hpp
 * @brief Header for the InteractiveMenu class providing the interactive terminal interface.
 *
 * This header defines the InteractiveMenu class which serves as the main user interface
 * for the Vestify stock analysis application. It provides an interactive menu system
 * for data loading, analysis, and display operations.
 *
 * @author Group 13
 */

#pragma once

#include <string>
#include <vector>

#include "core/StockRepository.hpp"
#include "core/WatchList.hpp"
#include "persistence/WatchListRepo.hpp"

/**
 * @brief Main user interface class for the Vestify application.
 *
 * The InteractiveMenu class provides the interactive terminal interface for the
 * stock analysis application. It coordinates between user input, data operations,
 * and display output through the StockRepository and various display components.
 * It acts as the application's orchestration layer, delegating specialized work
 * to screening, watchlist, scoring, printing, and backtesting modules.
 *
 * @author Group 13
 */
class InteractiveMenu
{
public:

    /**
     * @brief Construct the interactive menu and load the demo watchlist.
     *
     * Initializes the repository-backed state, configures the watchlist
     * persistence file, and loads any saved watchlists so the session starts
     * from the latest stored data.
     */
    InteractiveMenu();

    /**
     * @brief Main entry point for the interactive menu system.
     *
     * Launches the interactive menu loop and serves as the primary terminal
     * interface between the application and the user.
     *
     * @return Exit code (0 for success, non-zero for errors).
     */
    int run();

private:
    /** @brief Repository for managing stock data. */
    StockRepository repository_;

    /** @brief In-memory collection of watchlists. */
    std::vector<WatchList> watchlists_;

    /** @brief Repository responsible for loading and saving persisted watchlists. */
    WatchListRepo watchlistRepo_;

    /**
     * @brief Convert a string to uppercase.
     *
     * Utility function that converts all characters in a string to uppercase
     * using standard library functions.
     *
     * @param str Input string to convert.
     * @return New string with all characters converted to uppercase.
     */
    static std::string toUpper(std::string str);

    /**
     * @brief Split a comma-separated string into individual ticker symbols.
     *
     * Parses a comma-delimited string and extracts individual ticker symbols,
     * trimming whitespace and handling empty entries appropriately.
     *
     * @param input Comma-separated string of ticker symbols.
     * @return Vector of individual ticker symbol strings.
     */
    static std::vector<std::string> splitTickers(const std::string &input);

    /**
     * @brief Load stock data from a CSV file.
     *
     * Attempts to load stock data from the specified CSV file path using the
     * repository's loadFromCsv method. Reports success/failure and any errors.
     *
     * @param path Path to the CSV file containing stock data.
     * @return Exit code (0 for success, 2 for loading errors).
     */
    int handleLoadCsv(const std::string &path);

    /**
     * @brief Display detailed information for all loaded stocks.
     *
     * Prints comprehensive stock data for all stocks currently stored in the
     * repository. Shows an appropriate message if no stocks are loaded.
     */
    void printLoadedStocks() const;

    /**
     * @brief Update stock data by running the external update script.
     *
     * Executes the Python script responsible for updating stock data and
     * reports the success or failure of the update operation.
     */
    void updateStockData();

    /**
     * @brief Search for and display information about a specific stock.
     *
     * Prompts the user to enter a ticker symbol, searches the repository,
     * and displays detailed information for the found stock or an error message.
     */
    void searchTicker() const;

    /**
     * @brief Load the persisted demo watchlist from disk.
     *
     * Attempts to read the watchlist JSON file through the WatchListRepo and
     * populate the in-memory watchlist collection. Any parsing or file errors
     * are reported to the console but do not terminate the program.
     */
    //void loadDemoWatchlist();

    void loadWatchlists();

    /**
     * @brief Save the current demo watchlist to disk.
     *
     * Serializes the first watchlist in memory and writes it to the JSON file
     * through the WatchListRepo. This is typically called after modifications
     * such as adding or removing stocks.
     */
    //void saveDemoWatchlist() const;

    void saveWatchlists() const;

    /**
     * @brief Display and manage the watchlist menu.
     *
     * Presents the main watchlist interface where users can create watchlists,
     * modify them, view their contents, or open a specific watchlist.
     * Runs in a loop until the user chooses to return to the main menu.
     */
    void watchlistMenu();

    /**
     * @brief Score and rank loaded stocks using a weighted scoring model.
     *
     * Allows the user to select a scoring strategy, view ranked results,
     * and inspect a breakdown of factor contributions for a chosen stock.
     */
    void scoreAndRankStocks();

    /**
     * @brief Screen loaded stocks using one or more user-selected criteria.
     *
     * Forwards control to the dedicated screening menu so the top-level menu
     * remains focused on routing rather than filter-building details.
     */
    void screenStocks();

    /**
     * @brief Run a historical backtest with periodic rebalancing.
     *
     * Collects a backtest configuration from the user, loads the required
     * historical prices, and prints the resulting performance summary.
     */
    void runBacktest();

    /**
     * @brief Display the watchlist modification submenu.
     *
     * Allows the user to modify an existing watchlist by adding or removing
     * stocks, renaming the watchlist, or deleting it entirely.
     */
    void modifyWatchlistMenu();

    /**
     * @brief Read a validated integer from standard input.
     *
     * Reads a number entered by the user and ensures the input stream is valid.
     * If invalid input is entered, the stream is cleared and the user is prompted again.
     *
     * @return Integer value entered by the user.
     */
    static int readInt();

    /**
     * @brief Read a full line of input from the user.
     *
     * Prompts the user with a message and reads a complete line from standard input.
     * Useful for inputs containing spaces such as watchlist names.
     *
     * @param prompt Message displayed before reading input.
     * @return The entered string.
     */
    static std::string readLine(const std::string &prompt);



    //New member variables supporting multiple watchlists

    // int selectedWatchlistIndex_ = -1;
    // bool lockWatchlistSelection_ = false;

    /**
     * @brief Create a new watchlist.
     *
     * Prompts the user for a watchlist name and adds a new WatchList object
     * to the in-memory collection if the name is valid and not already used.
     *
     * @return True if a watchlist was created, false otherwise.
     */
    bool createWatchlist();

    /**
    * @brief Rename an existing watchlist.
    *
    * Allows the user to assign a new name to the specified watchlist.
    * Ensures the new name is not empty and does not conflict with existing names.
    *
    * @param index Zero-based index of the watchlist to rename.
    * @return True if the watchlist was renamed, false otherwise.
    */
    bool renameWatchlist(int index);

    /**
    * @brief Delete a watchlist.
    *
    * Removes the specified watchlist from the in-memory collection.
    *
    * @param index Zero-based index of the watchlist to delete.
    * @return True if a watchlist was deleted, false otherwise.
    */
    bool deleteWatchlist(int index);

    /**
    * @brief Add a stock ticker to a watchlist.
    *
    * Prompts the user for a ticker symbol and adds it to the specified
    * watchlist if the ticker exists in the repository and is not already present.
    *
    * @param index Zero-based index of the watchlist to modify.
    * @return True if the ticker was added, false otherwise.
    */
    bool addStockToWatchlist(int index);

    /**
    * @brief Remove a stock ticker from a watchlist.
    *
    * Prompts the user for a ticker symbol and removes it from the
    * specified watchlist if it exists.
    *
    * @param index Zero-based index of the watchlist to modify.
    * @return True if the ticker was removed, false otherwise.
    */
    bool removeStockFromWatchlist(int index);

    /**
     * @brief Display all available watchlists.
     *
     * Prints the names of all watchlists currently stored in memory
     * along with their index numbers for selection.
     */
    void printAllWatchlists() const;

    /**
     * @brief Display the contents of all watchlists.
     *
     * Iterates through every watchlist in memory and prints the
     * associated stocks for each list.
     */
    void printAllWatchlistContents() const;

    /**
     * @brief Open and display a specific watchlist.
     *
     * Allows the user to select a watchlist and prints all stocks
     * contained within that list.
     */
    void openSingleWatchlist() const;

    /**
     * @brief Display the stocks contained in a watchlist.
     *
     * Prints the ticker symbols stored in the specified watchlist
     * and retrieves the corresponding stock data from the repository
     * for display.
     *
     * @param watchlist The watchlist to display.
     */
    void printWatchlist(const WatchList &watchlist) const;

    /**
     * @brief Prompt the user to select a watchlist index.
     *
     * Displays the list of available watchlists and asks the user to
     * choose one by number.
     *
     * @return Index of the selected watchlist, or -1 if selection fails.
     */
    int selectWatchlistIndex() const;

    /**
     * @brief Run the interactive menu loop.
     *
     * Displays a numbered menu of options and processes user input in a loop
     * until the user chooses to exit. Provides access to all major application
     * features through the menu interface.
     */
    void runInteractive();
};

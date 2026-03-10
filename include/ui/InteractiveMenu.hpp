/**
 * @file InteractiveMenu.hpp
 * @brief Header for the InteractiveMenu class providing command-line and interactive interfaces.
 *
 * This header defines the InteractiveMenu class which serves as the main user interface
 * for the Vestify stock analysis application. It provides both command-line argument
 * processing and an interactive menu system for data loading, analysis, and display operations.
 */

#pragma once

#include <string>
#include <vector>

#include "core/StockRepository.hpp"

/**
 * @brief Main user interface class for the Vestify application.
 *
 * The InteractiveMenu class provides a complete command-line interface for the stock
 * analysis application, supporting both batch processing via command-line arguments
 * and interactive menu-driven operation. It coordinates between user input, data
 * operations, and display output through the StockRepository and various display components.
 */
class InteractiveMenu
{
public:
    /**
     * @brief Main entry point for the interactive menu system.
     *
     * Processes command-line arguments if provided, otherwise launches the interactive
     * menu loop. This method serves as the primary interface between the application
     * and the user, handling both automated and manual operation modes.
     *
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line argument strings.
     * @return Exit code (0 for success, non-zero for errors).
     */
    int run(int argc, char **argv);

private:
    /** @brief Repository for managing stock data. */
    StockRepository repository_;

    /**
     * @brief Print command-line usage information.
     *
     * Displays available command-line options, arguments, and environment variables
     * required for running the application from the command line.
     *
     * @param exe The executable name/path for usage examples.
     */
    static void printUsage(const std::string &exe);

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
     * @brief Process command-line arguments and execute appropriate actions.
     *
     * Parses command-line arguments and dispatches to the corresponding handler
     * functions based on the provided options (--help, --load-csv, --live).
     *
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line argument strings.
     * @return Exit code (0 for success, non-zero for errors).
     */
    int handleCommandLine(int argc, char **argv);

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
     * @brief Fetch live stock quotes for specified tickers.
     *
     * Uses the live data provider to fetch current quotes for the given list
     * of ticker symbols, updates the repository, and displays the results.
     *
     * @param tickerList Comma-separated string of ticker symbols.
     * @return Exit code (0 for success, 2 for fetching errors).
     */
    int handleLiveQuotes(const std::string &tickerList);

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
     * @brief Run the interactive menu loop.
     *
     * Displays a numbered menu of options and processes user input in a loop
     * until the user chooses to exit. Provides access to all major application
     * features through the menu interface.
     */
    void runInteractive();
};
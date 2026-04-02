/**
 * @file StockRepository.hpp
 * @brief Header for the StockRepository class that manages stock data collections.
 *
 * This header defines the StockRepository class which serves as the central
 * data store for stock information in the Vestify application. It provides
 * methods for loading stock data from CSV files and retrieving stocks by
 * various criteria.
 */

#pragma once

#include <string>
#include <vector>

#include "core/Stock.hpp"

/**
 * @brief Repository class for managing collections of stock data.
 *
 * The StockRepository class provides a centralized interface for stock data
 * management, including loading from external sources, in-memory storage,
 * and efficient retrieval operations. It serves as the data access layer
 * between data providers and the rest of the application.
 */
class StockRepository
{
public:
    /**
     * @brief Result structure for data loading operations.
     *
     * Contains the successfully loaded stocks and any errors that occurred
     * during the loading process.
     */
    struct LoadResult
    {
        /** @brief Successfully loaded stock data. */
        std::vector<Stock> stocks;
        /** @brief Error messages from the loading process. */
        std::vector<std::string> errors;
    };

    /**
     * @brief Load stock data from a CSV file.
     *
     * Parses the specified CSV file and loads stock data into the repository.
     * Returns a LoadResult containing both successfully loaded stocks and
     * any parsing errors that occurred.
     *
     * @param path File path to the CSV file containing stock data.
     * @return LoadResult with loaded stocks and any error messages.
     */
    LoadResult loadFromCsv(const std::string &path);

    /**
     * @brief Get a reference to all stocks in the repository.
     *
     * Provides read-only access to the complete collection of stocks.
     * The returned reference remains valid as long as the repository exists
     * and no operations modify the stock collection.
     *
     * @return Const reference to the vector of all stored stocks.
     */
    const std::vector<Stock> &getAll() const;

    /**
     * @brief Find a stock by its ticker symbol.
     *
     * Searches for a stock with the specified ticker symbol (case-insensitive).
     * Returns a pointer to the found stock, or nullptr if no matching stock
     * is found.
     *
     * @param ticker The ticker symbol to search for.
     * @return Pointer to the found Stock object, or nullptr if not found.
     */
    const Stock *findByTicker(const std::string &ticker) const;

private:
    /** @brief Internal storage for the stock collection. */
    std::vector<Stock> stocks_;
};

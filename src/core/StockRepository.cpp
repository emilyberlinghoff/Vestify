/**
 * @file StockRepository.cpp
 * @brief Implementation of the StockRepository class for managing stock data.
 *
 * This file contains the implementation of the StockRepository class which serves
 * as the central data store for stock information in the Vestify application.
 * It provides methods for loading stock data from CSV files and retrieving
 * stocks by various criteria.
 *
 * @author Emily Berlinghoff
 */

#include "core/StockRepository.hpp"

#include <algorithm>
#include <cctype>

#include "data/CSVLoader.hpp"

namespace
{
    /**
     * @brief Converts a string to uppercase.
     *
     * Creates a copy of the input string with all characters converted to uppercase
     * using the standard library toupper function.
     *
     * @param str The input string to convert.
     * @return A new string with all characters in uppercase.
     */
    std::string toUpperCopy(std::string str)
    {
        std::transform(
            str.begin(),
            str.end(),
            str.begin(),
            [](unsigned char c)
            { return static_cast<char>(std::toupper(c)); });
        return str;
    }
}

/**
 * @brief Loads stock data from a CSV file.
 *
 * Uses the CSVLoader to parse the specified file and stores the loaded
 * stocks in the repository. Returns a LoadResult containing both the
 * successfully loaded stocks and any errors that occurred during loading.
 *
 * @param path Path to the CSV file containing stock data.
 * @return LoadResult structure with loaded stocks and any error messages.
 */
StockRepository::LoadResult StockRepository::loadFromCsv(const std::string &path)
{
    CSVLoader loader;
    auto result = loader.loadFile(path);

    stocks_ = result.stocks;

    LoadResult out;
    out.stocks = stocks_;
    out.errors = result.errors;
    return out;
}

/**
 * @brief Returns a reference to all stocks in the repository.
 *
 * Provides read-only access to the complete collection of stocks currently
 * stored in the repository. The returned reference is valid as long as
 * the repository exists and no operations modify the stock collection.
 *
 * @return Const reference to the vector of all stored stocks.
 */
const std::vector<Stock> &StockRepository::getAll() const
{
    return stocks_;
}

/**
 * @brief Finds a stock by its ticker symbol.
 *
 * Searches through the stored stocks to find one with a matching ticker symbol.
 * The search is case-insensitive. Returns a pointer to the found stock, or
 * nullptr if no stock with the given ticker is found.
 *
 * @param ticker The ticker symbol to search for (case-insensitive).
 * @return Pointer to the found Stock object, or nullptr if not found.
 */
const Stock *StockRepository::findByTicker(const std::string &ticker) const
{
    std::string target = toUpperCopy(ticker);

    for (const auto &stock : stocks_)
    {
        if (toUpperCopy(stock.ticker) == target)
        {
            return &stock;
        }
    }

    return nullptr;
}

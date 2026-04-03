/**
 * @file WatchList.cpp
 * @brief Implementation of the WatchList container for tracked ticker symbols.
 * @author Group 13
 */

#include "core/Watchlist.hpp"

#include <algorithm>
#include <cctype>

/**
 * @brief Construct a watchlist with an optional display name.
 *
 * @param name Watchlist name.
 */
WatchList::WatchList(const std::string &name)
    : name_(name)
{
}

/**
 * @brief Add a ticker symbol if it is not already present.
 *
 * @param sym Raw ticker symbol.
 * @return True if the symbol was added, false if it already existed.
 */
bool WatchList::add(const std::string &sym)
{
    std::string normalized = normalizeSymbol(sym);

    if (has(normalized))
    {
        return false;
    }

    symbols_.push_back(normalized);
    return true;
}

/**
 * @brief Remove a ticker symbol from the watchlist.
 *
 * @param sym Raw ticker symbol.
 * @return True if the symbol was removed, false if it was not found.
 */
bool WatchList::remove(const std::string &sym)
{
    std::string normalized = normalizeSymbol(sym);

    auto pos = std::find(symbols_.begin(), symbols_.end(), normalized);
    if (pos == symbols_.end())
    {
        return false;
    }

    symbols_.erase(pos);
    return true;
}

/**
 * @brief Check whether a ticker symbol is currently tracked.
 *
 * @param sym Raw ticker symbol.
 * @return True if the symbol exists in the watchlist.
 */
bool WatchList::has(const std::string &sym) const
{
    std::string normalized = normalizeSymbol(sym);
    return std::find(symbols_.begin(), symbols_.end(), normalized) != symbols_.end();
}

/**
 * @brief Get all tracked symbols.
 *
 * @return Const reference to the internal symbol vector.
 */
const std::vector<std::string> &WatchList::getAll() const
{
    return symbols_;
}

/**
 * @brief Get the number of tracked symbols.
 *
 * @return Number of tickers in the watchlist.
 */
size_t WatchList::size() const
{
    return symbols_.size();
}

/**
 * @brief Check whether the watchlist contains no symbols.
 *
 * @return True if the watchlist is empty.
 */
bool WatchList::empty() const
{
    return symbols_.empty();
}

/**
 * @brief Remove all tracked symbols from the watchlist.
 */
void WatchList::clear()
{
    symbols_.clear();
}

/**
 * @brief Get the watchlist display name.
 *
 * @return Const reference to the current watchlist name.
 */
const std::string &WatchList::getName() const
{
    return name_;
}

/**
 * @brief Update the watchlist display name.
 *
 * @param name New watchlist name.
 */
void WatchList::setName(const std::string &name)
{
    name_ = name;
}

/**
 * @brief Normalize a ticker symbol to uppercase form.
 *
 * @param sym Raw ticker symbol.
 * @return Uppercase ticker symbol.
 */
std::string WatchList::normalizeSymbol(const std::string &sym)
{
    std::string result = sym;

    for (char &c : result)
    {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }

    return result;
}

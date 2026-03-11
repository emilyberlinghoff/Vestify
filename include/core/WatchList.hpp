#pragma once

#include <string>
#include <vector>

/**
 * @brief Keeps track of stocks the user wants to watch.
 */
class WatchList
{
public:
    /**
     * @brief Construct a watchlist with a name.
     *
     * @param name Watchlist name.
     */
    WatchList(const std::string& name = "");

    /**
     * @brief Add a ticker to the list.
     *
     * Won't add if it is already there.
     *
     * @param sym Ticker symbol like "AAPL".
     * @return True if it got added, false if duplicate.
     */
    bool add(const std::string& sym);

    /**
     * @brief Take a ticker off the list.
     *
     * @param sym Ticker to remove.
     * @return True if it was there and got removed.
     */
    bool remove(const std::string& sym);

    /**
     * @brief Check if a ticker is on the watchlist.
     *
     * @param sym Ticker to look for.
     * @return True if it is in there.
     */
    bool has(const std::string& sym) const;

    /**
     * @brief Get all the tickers.
     *
     * @return Reference to the stored symbols.
     */
    const std::vector<std::string>& getAll() const;

    /**
     * @brief How many tickers are being tracked.
     *
     * @return Count of tickers.
     */
    size_t size() const;

    /**
     * @brief Check if there is nothing in the watchlist.
     *
     * @return True if empty.
     */
    bool empty() const;

    /**
     * @brief Wipe the whole watchlist.
     */
    void clear();

    /**
     * @brief Get the watchlist name.
     *
     * @return The watchlist name.
     */
    const std::string& getName() const;

    /**
     * @brief Change the watchlist name.
     *
     * @param name New watchlist name.
     */
    void setName(const std::string& name);

private:
    /**
     * @brief Convert ticker symbols to uppercase.
     *
     * @param sym Input symbol.
     * @return Uppercase version.
     */
    static std::string normalizeSymbol(const std::string& sym);

private:
    std::string name_;
    std::vector<std::string> symbols_;
};
#pragma once

#include <algorithm>
#include <string>
#include <vector>

/**
 * @brief Keeps track of stocks the user wants to watch.
 */
class Watchlist {
public:
    /**
     * @brief Add a ticker to the list.
     *
     * Wont add if its already there.
     *
     * @param sym Ticker symbol like "AAPL".
     * @return True if it got added, false if duplicate.
     */
    bool add(const std::string& sym) {
        if (has(sym)) {
            return false;
        }
        symbols_.push_back(sym);
        return true;
    }

    /**
     * @brief Take a ticker off the list.
     *
     * @param sym Ticker to remove.
     * @return True if it was there and got removed.
     */
    bool remove(const std::string& sym) {
        auto pos = std::find(symbols_.begin(), symbols_.end(), sym);
        if (pos == symbols_.end()) {
            return false;
        }
        symbols_.erase(pos);
        return true;
    }

    /**
     * @brief Check if a ticker is on the watchlist.
     *
     * @param sym Ticker to look for.
     * @return True if its in there.
     */
    bool has(const std::string& sym) const {
        return std::find(symbols_.begin(), symbols_.end(), sym) != symbols_.end();
    }

    /**
     * @brief Get all the tickers.
     *
     * @return Reference to the stored symbols.
     */
    const std::vector<std::string>& getAll() const {
        return symbols_;
    }

    /**
     * @brief How many tickers are being tracked.
     *
     * @return Count of tickers.
     */
    size_t size() const {
        return symbols_.size();
    }

    /**
     * @brief Check if theres nothing in the watchlist.
     *
     * @return True if empty.
     */
    bool empty() const {
        return symbols_.empty();
    }

    /**
     * @brief Wipe the whole watchlist.
     */
    void clear() {
        symbols_.clear();
    }

private:
    /** @brief The tickers we're storing. */
    std::vector<std::string> symbols_;
};
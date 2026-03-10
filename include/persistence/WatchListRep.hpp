#pragma once

#include <fstream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "core/Watchlist.hpp"

/**
 * @brief Reads and writes the watchlist to a JSON file.
 *
 * JSON format: { "tickers": ["AAPL", "MSFT", "GOOG"] }
 */
class WatchlistRepo {
public:
    /**
     * @brief Holds the watchlist and any errors from loading.
     */
    struct LoadResult {
        /** @brief Watchlist that was loaded. */
        Watchlist watchlist;
        /** @brief Any errors that came up. */
        std::vector<std::string> errors;
    };

    /**
     * @brief Holds whether the save worked or not.
     */
    struct SaveResult {
        /** @brief True if save went through. */
        bool ok = false;
        /** @brief Error message if something went wrong. */
        std::string errMsg;
    };

    /**
     * @brief Set up the repo with a file path.
     *
     * @param filepath Where the JSON file lives on disk.
     */
    explicit WatchlistRepo(std::string filepath)
        : filepath_(std::move(filepath)) {}

    /**
     * @brief Load watchlist from the file.
     *
     * Returns empty watchlist if file doesnt exist or if the
     * JSON is messed up. Errors go into the result.
     *
     * @return LoadResult with the watchlist and errors if any.
     */
    LoadResult load() const;

    /**
     * @brief Write the watchlist out to the file.
     *
     * @param wl Watchlist to save.
     * @return SaveResult with ok flag and error msg.
     */
    SaveResult save(const Watchlist& wl) const;

private:
    /** @brief File path for the watchlist JSON. */
    std::string filepath_;
};
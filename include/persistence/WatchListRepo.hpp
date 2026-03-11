#pragma once

#include <string>
#include <vector>

#include "core/Watchlist.hpp"

/**
 * @brief Handles loading and saving watchlists to disk.
 *
 * Uses JSON files for persistence.
 */
class WatchListRepo
{
public:

    /**
     * @brief Result returned when loading a watchlist.
     */
    struct LoadResult
    {
        WatchList watchlist;
        std::vector<std::string> errors;
    };

    /**
     * @brief Result returned when saving a watchlist.
     */
    struct SaveResult
    {
        bool ok = false;
        std::string errMsg;
    };

public:

    /**
     * @brief Create a repo bound to a file path.
     *
     * @param filepath JSON file path for persistence.
     */
    explicit WatchListRepo(const std::string& filepath)
        : filepath_(filepath) {}

    /**
     * @brief Load the watchlist from disk.
     *
     * @return LoadResult containing the watchlist and any errors.
     */
    LoadResult load() const;

    /**
     * @brief Save the watchlist to disk.
     *
     * @param wl Watchlist to save.
     * @return SaveResult indicating success or failure.
     */
    SaveResult save(const WatchList& wl) const;

private:

    /**
     * @brief File path used for persistence.
     */
    std::string filepath_;
};
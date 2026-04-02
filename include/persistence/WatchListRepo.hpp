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
     * @brief Result returned when loading watchlists from disk.
     */
    struct LoadResult
    {
        std::vector<WatchList> watchlists;
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
     * @brief Load the first watchlist from disk.
     *
     * Preserves the old single-watchlist style interface.
     *
     * @return The first watchlist found, or an empty/default one if none exist.
     */
    LoadResult load() const;

    /**
     * @brief Load all watchlists from disk.
     *
     * @return LoadResult containing all watchlists and any errors.
     */
    LoadResult loadAll() const;

    /**
     * @brief Save a single watchlist to disk.
     *
     * Preserves the old single-watchlist style interface.
     * This writes just one watchlist to the file.
     *
     * @param wl Watchlist to save.
     * @return SaveResult indicating success or failure.
     */
    SaveResult save(const WatchList& wl) const;

    /**
     * @brief Save all watchlists to disk.
     *
     * @param watchlists Watchlists to save.
     * @return SaveResult indicating success or failure.
     */
    SaveResult saveAll(const std::vector<WatchList>& watchlists) const;

private:
    /**
     * @brief File path used for persistence.
     */
    std::string filepath_;
};
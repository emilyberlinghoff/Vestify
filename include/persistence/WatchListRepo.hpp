/**
 * @file WatchListRepo.hpp
 * @brief Header for JSON-based watchlist persistence.
 */

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
        /** @brief Watchlists loaded from disk. */
        std::vector<WatchList> watchlists;
        /** @brief Human-readable load/parsing errors. */
        std::vector<std::string> errors;
    };

    /**
     * @brief Result returned when saving a watchlist.
     */
    struct SaveResult
    {
        /** @brief Whether the save operation succeeded. */
        bool ok = false;
        /** @brief Error message when saving fails. */
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

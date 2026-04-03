/**
 * @file WatchListRepo.hpp
 * @brief Header for JSON-based watchlist persistence.
 * @author Zack Goodman
 */

#pragma once

#include <string>
#include <vector>

#include "core/Watchlist.hpp"

/**
 * @brief Handles loading and saving watchlists to disk.
 *
 * Uses JSON files for persistence.
 * The repository hides file-format details from the rest of the application so
 * the UI and domain logic can work directly with `WatchList` objects.
 *
 * @author Zack Goodman
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
     * @brief Load all watchlists from disk.
     *
     * @return LoadResult containing all watchlists and any errors.
     */
    LoadResult loadAll() const;

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

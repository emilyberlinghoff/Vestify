#pragma once

#include <string>
#include <vector>

#include "core/Stock.hpp"

/**
 * @brief In-memory repository for loaded stocks.
 */
class StockRepository {
public:
    /**
     * @brief Result of a repository load operation.
     */
    struct LoadResult {
        /** @brief Loaded stocks. */
        std::vector<Stock> stocks;
        /** @brief Errors encountered during load. */
        std::vector<std::string> errors;
    };

    /**
     * @brief Load stocks from a CSV file and store them in the repository.
     *
     * @param path CSV file path.
     * @return LoadResult with loaded stocks and errors.
     */
    LoadResult loadFromCsv(const std::string& path);

    /**
     * @brief Get all loaded stocks.
     *
     * @return Const reference to loaded stocks.
     */
    const std::vector<Stock>& getAll() const;

private:
    /** @brief Stored stocks. */
    std::vector<Stock> stocks_;
};

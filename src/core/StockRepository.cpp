#include "core/StockRepository.hpp"

#include "data/CSVLoader.hpp"

/**
 * @brief Load stocks from CSV and store them in the repository.
 *
 * @param path CSV file path.
 * @return LoadResult with loaded stocks and errors.
 */
StockRepository::LoadResult StockRepository::loadFromCsv(const std::string& path) {
    CSVLoader loader;
    auto result = loader.loadFile(path);

    stocks_ = result.stocks;

    LoadResult out;
    out.stocks = stocks_;
    out.errors = result.errors;
    return out;
}

/**
 * @brief Get all loaded stocks.
 *
 * @return Const reference to loaded stocks.
 */
const std::vector<Stock>& StockRepository::getAll() const {
    return stocks_;
}

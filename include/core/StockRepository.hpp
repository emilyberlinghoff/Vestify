#pragma once

#include <string>
#include <vector>

#include "core/Stock.hpp"

class StockRepository
{
public:
    struct LoadResult
    {
        std::vector<Stock> stocks;
        std::vector<std::string> errors;
    };

    LoadResult loadFromCsv(const std::string &path);

    void setStocks(const std::vector<Stock> &stocks);
    const std::vector<Stock> &getAll() const;
    const Stock *findByTicker(const std::string &ticker) const;

private:
    std::vector<Stock> stocks_;
};
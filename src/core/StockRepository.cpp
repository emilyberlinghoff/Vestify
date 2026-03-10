#include "core/StockRepository.hpp"

#include <algorithm>
#include <cctype>

#include "data/CSVLoader.hpp"

namespace
{
    std::string toUpperCopy(std::string str)
    {
        std::transform(
            str.begin(),
            str.end(),
            str.begin(),
            [](unsigned char c)
            { return static_cast<char>(std::toupper(c)); });
        return str;
    }
}

StockRepository::LoadResult StockRepository::loadFromCsv(const std::string &path)
{
    CSVLoader loader;
    auto result = loader.loadFile(path);

    stocks_ = result.stocks;

    LoadResult out;
    out.stocks = stocks_;
    out.errors = result.errors;
    return out;
}

void StockRepository::setStocks(const std::vector<Stock> &stocks)
{
    stocks_ = stocks;
}

const std::vector<Stock> &StockRepository::getAll() const
{
    return stocks_;
}

const Stock *StockRepository::findByTicker(const std::string &ticker) const
{
    std::string target = toUpperCopy(ticker);

    for (const auto &stock : stocks_)
    {
        if (toUpperCopy(stock.ticker) == target)
        {
            return &stock;
        }
    }

    return nullptr;
}
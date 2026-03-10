#pragma once

#include <string>
#include <vector>

#include "core/Stock.hpp"

class InteractiveMenu {
public:
    InteractiveMenu() = default;
    void run();

private:
    std::vector<Stock> loadedStocks_;

    static std::string toUpper(std::string str);
    static std::vector<std::string> splitTickers(const std::string& input);

    int handleLoadCsv(const std::string& path);
    int handleLiveQuotes(const std::string& tickerList);
    void updateStockData();
    void searchTicker();
};
#pragma once

#include <string>
#include <vector>

#include "core/StockRepository.hpp"

class InteractiveMenu
{
public:
    int run(int argc, char **argv);

private:
    StockRepository repository_;

    static void printUsage(const std::string &exe);
    static std::string toUpper(std::string str);
    static std::vector<std::string> splitTickers(const std::string &input);

    int handleCommandLine(int argc, char **argv);
    int handleLoadCsv(const std::string &path);
    int handleLiveQuotes(const std::string &tickerList);

    void printLoadedStocks() const;
    void updateStockData();
    void searchTicker() const;
    void runInteractive();
};
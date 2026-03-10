#pragma once

#include <string>
#include <vector>

#include "core/Stock.hpp"

class StockPrinter {
public:
    static void printStocks(const std::vector<Stock>& stocks);
    static void printAvailableStocks(const std::vector<Stock>& stocks);
    static void printTopEVFCF(std::vector<Stock> stocks, std::size_t count = 10);

private:
    static std::string formatMoney(double value);
    static std::string colorizeMetric(double value, double goodThreshold, double weakThreshold);
    static std::string colorizeLowIsBetter(double value, double goodThreshold, double badThreshold);
};
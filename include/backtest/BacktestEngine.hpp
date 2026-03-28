#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "core/Stock.hpp"
#include "scoring/StrategyPresent.hpp"

/**
 * @brief Simple backtest engine with periodic rebalancing.
 */
class BacktestEngine {
public:
    struct PricePoint {
        std::string date;
        double close = 0.0;
    };

    struct Config {
        std::vector<Stock> stocks;
        std::unordered_map<std::string, std::vector<PricePoint>> prices;
        std::string start_date;
        std::string end_date;
        std::size_t top_n = 5;
        int rebalance_interval = 20; // trading days
        double initial_capital = 10000.0;
    };

    struct Result {
        bool ok = false;
        std::string error;
        std::vector<PricePoint> equity_curve;
        double initial_value = 0.0;
        double final_value = 0.0;
        int rebalances = 0;
    };

    /**
     * @brief Run a backtest using the provided strategy.
     */
    Result run(const StrategyManager& strategy, const Config& config) const;

private:
    static bool isWithinRange(const std::string& date,
                              const std::string& start,
                              const std::string& end);
};

/**
 * @file BacktestEngine.hpp
 * @brief Backtest engine with periodic rebalancing support.
 */

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
    /**
     * @brief Single dated price point used for simulation.
     */
    struct PricePoint {
        std::string date;
        double close = 0.0;
    };

    /**
     * @brief Configuration parameters for a backtest run.
     */
    struct Config {
        std::vector<Stock> stocks;
        std::unordered_map<std::string, std::vector<PricePoint>> prices;
        std::string start_date;
        std::string end_date;
        std::size_t top_n = 5;
        int rebalance_interval = 20; /**< Trading days. */
        double initial_capital = 10000.0;
    };

    /**
     * @brief Output results for a backtest run.
     */
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
     *
     * @param strategy Scoring strategy to rank holdings.
     * @param config Backtest configuration.
     * @return Result with equity curve and summary values.
     */
    Result run(const StrategyManager& strategy, const Config& config) const;

private:
    /**
     * @brief Check if a date is within the start/end range.
     *
     * @param date Date to check.
     * @param start Inclusive start date (YYYY-MM-DD) or empty.
     * @param end Inclusive end date (YYYY-MM-DD) or empty.
     * @return True if date is within range.
     */
    static bool isWithinRange(const std::string& date,
                              const std::string& start,
                              const std::string& end);
};

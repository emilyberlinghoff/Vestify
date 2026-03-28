/**
 * @file BacktestEngine.cpp
 * @brief Implementation of the backtest engine.
 */

#include "backtest/BacktestEngine.hpp"

#include <algorithm>
#include <unordered_set>

bool BacktestEngine::isWithinRange(const std::string& date,
                                   const std::string& start,
                                   const std::string& end) {
    if (!start.empty() && date < start) {
        return false;
    }
    if (!end.empty() && date > end) {
        return false;
    }
    return true;
}

BacktestEngine::Result BacktestEngine::run(const StrategyManager& strategy,
                                           const Config& config) const {
    Result out;

    if (config.prices.empty()) {
        out.error = "No historical price data provided.";
        return out;
    }

    if (config.stocks.empty()) {
        out.error = "No stock fundamentals available for scoring.";
        return out;
    }

    if (config.top_n == 0 || config.top_n > config.prices.size()) {
        out.error = "Invalid top N selection for backtest.";
        return out;
    }

    if (config.rebalance_interval <= 0) {
        out.error = "Rebalance interval must be positive.";
        return out;
    }

    /// Build a date list from the first series.
    const auto& firstSeries = config.prices.begin()->second;
    std::vector<std::string> dates;
    dates.reserve(firstSeries.size());
    for (const auto& point : firstSeries) {
        if (isWithinRange(point.date, config.start_date, config.end_date)) {
            dates.push_back(point.date);
        }
    }

    if (dates.size() < 2) {
        out.error = "Insufficient historical data in the selected date range.";
        return out;
    }

    /// Build fast lookup maps for each ticker and validate completeness.
    std::unordered_map<std::string, std::unordered_map<std::string, double>> priceByDate;
    for (const auto& [ticker, series] : config.prices) {
        std::unordered_map<std::string, double> map;
        for (const auto& point : series) {
            if (isWithinRange(point.date, config.start_date, config.end_date)) {
                map[point.date] = point.close;
            }
        }
        priceByDate[ticker] = std::move(map);
    }

    for (const auto& date : dates) {
        for (const auto& [ticker, map] : priceByDate) {
            if (map.find(date) == map.end()) {
                out.error = "Missing historical data for " + ticker + " on " + date + ".";
                return out;
            }
        }
    }

    /// Filter stocks to only those with price data.
    std::vector<Stock> candidates;
    candidates.reserve(config.prices.size());
    std::unordered_set<std::string> priceTickers;
    for (const auto& [ticker, _] : config.prices) {
        priceTickers.insert(ticker);
    }
    for (const auto& stock : config.stocks) {
        if (priceTickers.count(stock.ticker)) {
            candidates.push_back(stock);
        }
    }

    if (candidates.empty()) {
        out.error = "No matching stocks found for the provided tickers.";
        return out;
    }

    /// Precompute ranking order (strategy does not change during a run).
    auto ranked = strategy.rankStocks(candidates);
    if (ranked.empty()) {
        out.error = "Unable to score stocks for backtest.";
        return out;
    }

    std::vector<std::string> topTickers;
    for (std::size_t i = 0; i < config.top_n; ++i) {
        topTickers.push_back(ranked[i].stock.ticker);
    }

    std::unordered_map<std::string, double> shares;
    double portfolioValue = config.initial_capital;
    out.initial_value = portfolioValue;

    auto rebalance = [&](const std::string& date) {
        double equalAlloc = portfolioValue / static_cast<double>(topTickers.size());
        shares.clear();
        for (const auto& ticker : topTickers) {
            double price = priceByDate[ticker][date];
            shares[ticker] = (price > 0.0) ? (equalAlloc / price) : 0.0;
        }
        out.rebalances += 1;
    };

    rebalance(dates.front());

    for (std::size_t i = 0; i < dates.size(); ++i) {
        const std::string& date = dates[i];

        if (i > 0 && (i % static_cast<std::size_t>(config.rebalance_interval) == 0)) {
            /// Update portfolio value before rebalancing.
            double value = 0.0;
            for (const auto& [ticker, qty] : shares) {
                value += qty * priceByDate[ticker][date];
            }
            portfolioValue = value;
            rebalance(date);
        }

        double value = 0.0;
        for (const auto& [ticker, qty] : shares) {
            value += qty * priceByDate[ticker][date];
        }
        portfolioValue = value;
        out.equity_curve.push_back({date, portfolioValue});
    }

    out.final_value = portfolioValue;
    out.ok = true;
    return out;
}

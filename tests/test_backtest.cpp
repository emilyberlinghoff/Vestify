/**
 * @file test_backtest.cpp
 * @brief Acceptance tests for the backtest engine.
 * @author Emily Berlinghoff
 */

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "backtest/BacktestEngine.hpp"
#include "scoring/GrowthScoringModel.hpp"
#include "scoring/MomentumScoringModel.hpp"
#include "scoring/StrategyPresent.hpp"
#include "scoring/ValueScoringModel.hpp"

static int passed = 0;
static int failed = 0;

static void CHECK(bool condition, const std::string &test_name)
{
    if (condition)
    {
        std::cout << "  [PASS] " << test_name << "\n";
        ++passed;
    }
    else
    {
        std::cout << "  [FAIL] " << test_name << "\n";
        ++failed;
    }
}

static StrategyManager makeStrategyManager()
{
    StrategyManager mgr;
    static ValueScoringModel valueModel;
    static GrowthScoringModel growthModel;
    static MomentumScoringModel momentumModel;
    mgr.registerModel(&valueModel);
    mgr.registerModel(&growthModel);
    mgr.registerModel(&momentumModel);
    return mgr;
}

static std::vector<std::string> makeDates()
{
    return {"2026-01-01", "2026-01-02", "2026-01-03", "2026-01-04", "2026-01-05"};
}

static std::vector<Stock> makeStocks()
{
    Stock a{"AAA", "Alpha", "Tech", 100.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10.0, 0.0};
    a.pe_ratio = 10.0;
    a.dividend_yield = 0.02;
    a.price = 100.0;

    Stock b{"BBB", "Beta", "Tech", 50.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30.0, 0.0};
    b.pe_ratio = 30.0;
    b.dividend_yield = 0.0;
    b.price = 50.0;

    Stock c{"CCC", "Gamma", "Tech", 200.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20.0, 0.0};
    c.pe_ratio = 20.0;
    c.dividend_yield = 0.01;
    c.price = 200.0;

    return {a, b, c};
}

static std::unordered_map<std::string, std::vector<BacktestEngine::PricePoint>> makePrices()
{
    auto dates = makeDates();
    std::unordered_map<std::string, std::vector<BacktestEngine::PricePoint>> prices;

    std::vector<BacktestEngine::PricePoint> a;
    std::vector<BacktestEngine::PricePoint> b;
    std::vector<BacktestEngine::PricePoint> c;

    double aPrice = 100.0;
    double bPrice = 50.0;
    double cPrice = 200.0;

    for (const auto &date : dates)
    {
        a.push_back({date, aPrice});
        b.push_back({date, bPrice});
        c.push_back({date, cPrice});
        aPrice += 1.0;
        bPrice += 0.5;
        cPrice -= 1.0;
    }

    prices["AAA"] = a;
    prices["BBB"] = b;
    prices["CCC"] = c;
    return prices;
}

static void testBacktestRuns()
{
    std::cout << "\n--- Test: Backtest runs and produces equity curve ---\n";

    BacktestEngine engine;
    BacktestEngine::Config config;
    config.stocks = makeStocks();
    config.prices = makePrices();
    config.top_n = 2;
    config.rebalance_interval = 2;
    config.initial_capital = 10000.0;

    auto mgr = makeStrategyManager();
    auto result = engine.run(mgr, config);

    CHECK(result.ok, "Backtest completes successfully");
    CHECK(result.equity_curve.size() == makeDates().size(),
          "Equity curve matches date count");
    CHECK(result.initial_value == 10000.0, "Initial value recorded");
    CHECK(result.final_value > 0.0, "Final value computed");
    CHECK(result.rebalances == 3, "Rebalances counted correctly");
}

static void testRebalanceIntervalAffectsResults()
{
    std::cout << "\n--- Test: Rebalance interval changes results ---\n";

    BacktestEngine engine;
    BacktestEngine::Config config;
    config.stocks = makeStocks();
    config.prices = makePrices();
    config.top_n = 2;
    config.rebalance_interval = 1;
    config.initial_capital = 10000.0;

    auto mgr = makeStrategyManager();
    auto result1 = engine.run(mgr, config);
    config.rebalance_interval = 2;
    auto result2 = engine.run(mgr, config);

    CHECK(result1.ok && result2.ok, "Both backtests complete");
    CHECK(result1.final_value != result2.final_value,
          "Different rebalance intervals produce different results");
}

static void testMissingDataHandled()
{
    std::cout << "\n--- Test: Missing data handled gracefully ---\n";

    BacktestEngine engine;
    BacktestEngine::Config config;
    config.stocks = makeStocks();
    config.prices = makePrices();
    config.top_n = 2;

    /// Remove one date from BBB to simulate missing data.
    if (!config.prices["BBB"].empty())
    {
        config.prices["BBB"].pop_back();
    }

    auto mgr = makeStrategyManager();
    auto result = engine.run(mgr, config);

    CHECK(!result.ok, "Backtest fails when data is missing");
    bool hasError = !result.error.empty();
    bool mentionsMissing = result.error.find("Missing historical data") != std::string::npos;
    bool mentionsInsufficient = result.error.find("Insufficient historical data") != std::string::npos;
    bool mentionsNoData = result.error.find("No historical price data") != std::string::npos;
    CHECK(hasError, "Error message is provided");
    CHECK(mentionsMissing || mentionsInsufficient || mentionsNoData,
          "Error message indicates missing or insufficient data");
    if (!hasError) {
        std::cout << "    Error was empty\n";
    } else {
        std::cout << "    Error: " << result.error << "\n";
    }
}

int main()
{
    std::cout << "============================================\n";
    std::cout << "  Backtest - Acceptance Test Suite\n";
    std::cout << "============================================\n";

    testBacktestRuns();
    testRebalanceIntervalAffectsResults();
    testMissingDataHandled();

    std::cout << "\n============================================\n";
    std::cout << "  Results: " << passed << " passed, " << failed << " failed\n";
    std::cout << "============================================\n";

    return (failed > 0) ? 1 : 0;
}

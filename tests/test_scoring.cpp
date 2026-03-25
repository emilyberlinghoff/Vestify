/**
 * @file test_scoring.cpp
 * @brief Test suite for scoring strategy functionality.
 *
 * This file contains comprehensive tests for the scoring system including:
 * - Strategy manager functionality
 * - Individual scoring models (Value, Growth, Momentum)
 * - Strategy switching and weight management
 * - Ranking consistency across different strategies
 */

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "core/Stock.hpp"
#include "scoring/ScoringModel.hpp"
#include "scoring/StrategyPresent.hpp"
#include "scoring/ValueScoringModel.hpp"
#include "scoring/GrowthScoringModel.hpp"
#include "scoring/MomentumScoringModel.hpp"

static int passed = 0;
static int failed = 0;

/**
 * @brief Test assertion helper function.
 *
 * Checks a condition and reports pass/fail status with test name.
 * Updates global pass/fail counters.
 *
 * @param condition Boolean condition to test.
 * @param test_name Descriptive name of the test.
 */
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

/**
 * @brief Approximate equality check for floating-point values.
 *
 * Compares two double values within a specified epsilon tolerance.
 *
 * @param a First value to compare.
 * @param b Second value to compare.
 * @param eps Tolerance for comparison (default: 0.001).
 * @return True if values are approximately equal.
 */
static bool approxEqual(double a, double b, double eps = 0.001)
{
    return std::fabs(a - b) < eps;
}

/**
 * @brief Creates a vector of sample stocks for testing.
 *
 * Generates a predefined set of stock objects with realistic data
 * for testing scoring algorithms and strategy comparisons.
 *
 * @return Vector of sample Stock objects.
 */
static std::vector<Stock> makeSampleStocks()
{
    // dividend_yield stored as fraction per your Stock struct
    return {
        {"AAPL", "Apple Inc.", "Technology", 185.0, 2.9e12, 29.0, 0.005},
        {"JNJ", "Johnson & Johnson", "Healthcare", 155.0, 3.7e11, 15.0, 0.030},
        {"XOM", "Exxon Mobil", "Energy", 105.0, 4.5e11, 10.0, 0.035},
        {"TSLA", "Tesla Inc.", "Automotive", 250.0, 7.9e11, 65.0, 0.000},
        {"KO", "Coca-Cola Co.", "Consumer", 60.0, 2.6e11, 24.0, 0.031},
    };
}

/**
 * @brief Test that multiple predefined strategies are available.
 *
 * Verifies that the strategy manager has at least 3 predefined strategies
 * (Value, Growth, Momentum, Balanced) and that each has proper weights
 * and descriptions defined.
 */
static void testMultiplePresetsExist()
{
    std::cout << "\n--- Test: Multiple predefined strategies exist ---\n";

    StrategyManager mgr;
    auto names = mgr.getAvailableStrategies();

    CHECK(names.size() >= 3, "At least 3 presets registered");
    CHECK(std::find(names.begin(), names.end(), "Value") != names.end(), "Value preset exists");
    CHECK(std::find(names.begin(), names.end(), "Growth") != names.end(), "Growth preset exists");
    CHECK(std::find(names.begin(), names.end(), "Momentum") != names.end(), "Momentum preset exists");
    CHECK(std::find(names.begin(), names.end(), "Balanced") != names.end(), "Balanced preset exists");

    for (const auto &name : names)
    {
        const auto &strat = mgr.getStrategy(name);
        CHECK(!strat.weights.empty(), name + " has weights defined");
        CHECK(!strat.description.empty(), name + " has a description");
    }
}

/**
 * @brief Test that selecting a strategy properly updates weights.
 *
 * Verifies that changing the active strategy updates the weights correctly
 * and that invalid strategy names are rejected without changing the active strategy.
 */
static void testSelectingStrategyUpdatesWeights()
{
    std::cout << "\n--- Test: Selecting a strategy updates weights ---\n";

    StrategyManager mgr;

    bool ok = mgr.setActiveStrategy("Growth");
    CHECK(ok, "setActiveStrategy(\"Growth\") returns true");
    CHECK(mgr.getActiveStrategyName() == "Growth", "Active strategy is now Growth");

    const auto &gw = mgr.getActiveStrategy().weights;
    CHECK(gw.at("Growth") > gw.at("Value"), "Growth strategy weights Growth > Value");

    ok = mgr.setActiveStrategy("Momentum");
    CHECK(ok, "setActiveStrategy(\"Momentum\") returns true");
    CHECK(mgr.getActiveStrategyName() == "Momentum", "Active strategy is now Momentum");

    const auto &mw = mgr.getActiveStrategy().weights;
    CHECK(mw.at("Momentum") > mw.at("Value"), "Momentum strategy weights Momentum > Value");

    ok = mgr.setActiveStrategy("NonExistent");
    CHECK(!ok, "setActiveStrategy with invalid name returns false");
    CHECK(mgr.getActiveStrategyName() == "Momentum", "Active strategy unchanged after invalid selection");
}

/**
 * @brief Test that rankings change when strategy changes.
 *
 * Verifies that different strategies produce different stock rankings
 * and that the rankings reflect the strategy's focus (value vs growth vs momentum).
 */
static void testRankingsChangeWithStrategy()
{
    std::cout << "\n--- Test: Rankings change when strategy changes ---\n";

    StrategyManager mgr;
    ValueScoringModel value_model;
    GrowthScoringModel growth_model;
    MomentumScoringModel momentum_model;
    mgr.registerModel(&value_model);
    mgr.registerModel(&growth_model);
    mgr.registerModel(&momentum_model);

    auto stocks = makeSampleStocks();

    mgr.setActiveStrategy("Value");
    auto value_ranked = mgr.rankStocks(stocks);
    std::string value_top = value_ranked.front().stock.ticker;

    mgr.setActiveStrategy("Growth");
    auto growth_ranked = mgr.rankStocks(stocks);
    std::string growth_top = growth_ranked.front().stock.ticker;

    mgr.setActiveStrategy("Momentum");
    auto momentum_ranked = mgr.rankStocks(stocks);
    std::string momentum_top = momentum_ranked.front().stock.ticker;

    std::cout << "    Value top pick:    " << value_top << "\n";
    std::cout << "    Growth top pick:   " << growth_top << "\n";
    std::cout << "    Momentum top pick: " << momentum_top << "\n";

    CHECK(value_top == "XOM" || value_top == "JNJ",
          "Value strategy top pick is a value stock (XOM or JNJ)");
    CHECK(growth_top == "TSLA",
          "Growth strategy top pick is a growth stock (TSLA)");
    CHECK(value_top != growth_top || growth_top != momentum_top,
          "Different strategies produce different rankings");

    for (const auto &ss : value_ranked)
    {
        CHECK(ss.composite >= 0.0, ss.stock.ticker + " has non-negative composite score");
    }
}

/**
 * @brief Test that a default strategy is applied on startup.
 *
 * Verifies that the StrategyManager initializes with a valid default strategy
 * ("Value") and that it has proper weights defined.
 */
static void testDefaultStrategyOnStartup()
{
    std::cout << "\n--- Test: Default strategy applied on startup ---\n";

    StrategyManager mgr;

    CHECK(!mgr.getActiveStrategyName().empty(), "Active strategy name is not empty on construction");
    CHECK(mgr.getActiveStrategyName() == "Value", "Default strategy is Value");

    const auto &weights = mgr.getActiveStrategy().weights;
    CHECK(!weights.empty(), "Default strategy has weights");
    CHECK(weights.count("Value") > 0, "Default strategy includes Value weight");
}

/**
 * @brief Test switching strategies without restarting the application.
 *
 * Verifies that strategies can be switched dynamically and that the same stock
 * receives different composite scores under different strategies.
 */
static void testSwitchWithoutRestart()
{
    std::cout << "\n--- Test: Switch strategies without restart ---\n";

    StrategyManager mgr;
    ValueScoringModel value_model;
    GrowthScoringModel growth_model;
    MomentumScoringModel momentum_model;
    mgr.registerModel(&value_model);
    mgr.registerModel(&growth_model);
    mgr.registerModel(&momentum_model);

    auto stocks = makeSampleStocks();

    mgr.setActiveStrategy("Value");
    auto ranked1 = mgr.rankStocks(stocks);
    double xom1 = 0.0;
    for (const auto &ss : ranked1)
    {
        if (ss.stock.ticker == "XOM")
        {
            xom1 = ss.composite;
            break;
        }
    }

    mgr.setActiveStrategy("Growth");
    auto ranked2 = mgr.rankStocks(stocks);
    double xom2 = 0.0;
    for (const auto &ss : ranked2)
    {
        if (ss.stock.ticker == "XOM")
        {
            xom2 = ss.composite;
            break;
        }
    }

    CHECK(!approxEqual(xom1, xom2), "XOM composite differs between Value and Growth");

    mgr.setActiveStrategy("Balanced");
    auto ranked3 = mgr.rankStocks(stocks);
    CHECK(!ranked3.empty(), "Balanced ranking returns results without restart");

    std::cout << "    XOM composite (Value):  " << xom1 << "\n";
    std::cout << "    XOM composite (Growth): " << xom2 << "\n";
}

/**
 * @brief Test that individual scoring models produce sensible results.
 *
 * Verifies that each scoring model (Value, Growth, Momentum) produces
 * appropriate relative rankings for different types of stocks.
 */
static void testScoringModels()
{
    std::cout << "\n--- Test: Individual scoring models ---\n";

    Stock value_stock{"XOM", "Exxon", "Energy", 105.0, 4.5e11, 10.0, 0.035};
    Stock growth_stock{"TSLA", "Tesla", "Auto", 250.0, 7.9e11, 65.0, 0.000};

    ValueScoringModel value_model;
    CHECK(value_model.calculateScore(value_stock) > value_model.calculateScore(growth_stock),
          "Value model scores XOM higher than TSLA");

    GrowthScoringModel growth_model;
    CHECK(growth_model.calculateScore(growth_stock) > growth_model.calculateScore(value_stock),
          "Growth model scores TSLA higher than XOM");

    MomentumScoringModel momentum_model;
    double mom = momentum_model.calculateScore(value_stock);
    CHECK(mom >= 0.0 && mom <= 100.0, "Momentum score is in [0, 100] range");
}

/**
 * @brief Test that scoring results are deterministic and repeatable.
 *
 * Runs scoring twice with the same inputs and confirms scores match.
 */
static void testDeterministicScoring()
{
    std::cout << "\n--- Test: Deterministic scoring ---\n";

    StrategyManager mgr;
    ValueScoringModel value_model;
    GrowthScoringModel growth_model;
    MomentumScoringModel momentum_model;
    mgr.registerModel(&value_model);
    mgr.registerModel(&growth_model);
    mgr.registerModel(&momentum_model);

    auto stocks = makeSampleStocks();
    mgr.setActiveStrategy("Balanced");

    auto ranked1 = mgr.rankStocks(stocks);
    auto ranked2 = mgr.rankStocks(stocks);

    CHECK(ranked1.size() == ranked2.size(), "Repeat scoring yields same number of results");

    bool all_equal = true;
    for (std::size_t i = 0; i < ranked1.size(); ++i)
    {
        if (!approxEqual(ranked1[i].composite, ranked2[i].composite))
        {
            all_equal = false;
            break;
        }
    }
    CHECK(all_equal, "Composite scores are repeatable for same inputs");
}

/**
 * @brief Test that ranked results are ordered by descending score.
 */
static void testRankingOrder()
{
    std::cout << "\n--- Test: Ranking order is descending ---\n";

    StrategyManager mgr;
    ValueScoringModel value_model;
    GrowthScoringModel growth_model;
    MomentumScoringModel momentum_model;
    mgr.registerModel(&value_model);
    mgr.registerModel(&growth_model);
    mgr.registerModel(&momentum_model);

    auto ranked = mgr.rankStocks(makeSampleStocks());

    bool ordered = true;
    for (std::size_t i = 1; i < ranked.size(); ++i)
    {
        if (ranked[i - 1].composite < ranked[i].composite)
        {
            ordered = false;
            break;
        }
    }
    CHECK(ordered, "Stocks are ordered from highest to lowest composite score");
}

/**
 * @brief Test that score breakdown includes individual factor contributions.
 */
static void testBreakdownIncludesFactors()
{
    std::cout << "\n--- Test: Breakdown includes factor subscores ---\n";

    StrategyManager mgr;
    ValueScoringModel value_model;
    GrowthScoringModel growth_model;
    MomentumScoringModel momentum_model;
    mgr.registerModel(&value_model);
    mgr.registerModel(&growth_model);
    mgr.registerModel(&momentum_model);

    auto scored = mgr.scoreStock(makeSampleStocks().front());
    const auto &weights = mgr.getActiveStrategy().weights;

    bool all_present = true;
    for (const auto &pair : weights)
    {
        if (scored.subscores.find(pair.first) == scored.subscores.end())
        {
            all_present = false;
            break;
        }
    }
    CHECK(all_present, "All strategy factors appear in subscore breakdown");
}

/**
 * @brief Test that missing or invalid data defaults to zero contributions.
 */
static void testMissingDataDefaultsToZero()
{
    std::cout << "\n--- Test: Missing data defaults to zero ---\n";

    StrategyManager mgr;
    ValueScoringModel value_model;
    GrowthScoringModel growth_model;
    MomentumScoringModel momentum_model;
    mgr.registerModel(&value_model);
    mgr.registerModel(&growth_model);
    mgr.registerModel(&momentum_model);

    Stock broken{};
    broken.ticker = "NULL";
    broken.name = "Missing Data";
    broken.pe_ratio = std::numeric_limits<double>::quiet_NaN();
    broken.dividend_yield = std::numeric_limits<double>::quiet_NaN();
    broken.price = std::numeric_limits<double>::quiet_NaN();

    auto scored = mgr.scoreStock(broken);

    bool all_zero = true;
    for (const auto &pair : scored.subscores)
    {
        if (!approxEqual(pair.second, 0.0))
        {
            all_zero = false;
            break;
        }
    }

    CHECK(all_zero, "Invalid factor data yields zero subscores");
    CHECK(approxEqual(scored.composite, 0.0), "Composite score defaults to 0 with missing data");
}

/**
 * @brief Main test runner for the scoring strategy test suite.
 *
 * Executes all test functions and reports the results. Returns 0 if all
 * tests pass, 1 if any tests fail.
 *
 * @return Exit code (0 for success, 1 for test failures).
 */
int main()
{
    std::cout << "============================================\n";
    std::cout << "  Scoring Strategy Preset - Test Suite\n";
    std::cout << "============================================\n";

    testMultiplePresetsExist();
    testSelectingStrategyUpdatesWeights();
    testRankingsChangeWithStrategy();
    testDefaultStrategyOnStartup();
    testSwitchWithoutRestart();
    testScoringModels();
    testDeterministicScoring();
    testRankingOrder();
    testBreakdownIncludesFactors();
    testMissingDataDefaultsToZero();

    std::cout << "\n============================================\n";
    std::cout << "  Results: " << passed << " passed, " << failed << " failed\n";
    std::cout << "============================================\n";

    return (failed > 0) ? 1 : 0;
}

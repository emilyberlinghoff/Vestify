/**
 * @file test_scoring.cpp
 * @brief Test suite for scoring strategy functionality.
 * @author Melvin Roger
 * @author Emily Berlinghoff
 *
 * This file contains comprehensive tests for the scoring system including:
 * - Strategy manager functionality
 * - Individual scoring models (Value, Growth, Momentum, Quality, Efficiency)
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
#include "scoring/QualityScoringModel.hpp"
#include "scoring/EfficiencyScoringModel.hpp"

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
 * @brief Helper to register all five scoring models with a StrategyManager.
 *
 * @param mgr StrategyManager to register models with.
 * @param v ValueScoringModel instance.
 * @param g GrowthScoringModel instance.
 * @param m MomentumScoringModel instance.
 * @param q QualityScoringModel instance.
 * @param e EfficiencyScoringModel instance.
 */
static void registerAllModels(StrategyManager &mgr,
                              ValueScoringModel &v,
                              GrowthScoringModel &g,
                              MomentumScoringModel &m,
                              QualityScoringModel &q,
                              EfficiencyScoringModel &e)
{
    mgr.registerModel(&v);
    mgr.registerModel(&g);
    mgr.registerModel(&m);
    mgr.registerModel(&q);
    mgr.registerModel(&e);
}

/**
 * @brief Creates a vector of sample stocks with realistic fundamental data.
 *
 * @return Vector of sample Stock objects.
 */
static std::vector<Stock> makeSampleStocks()
{
    Stock aapl{};
    aapl.ticker = "AAPL"; aapl.name = "Apple Inc."; aapl.sector = "Technology";
    aapl.price = 260.0; aapl.pe_ratio = 34.0; aapl.dividend_yield = 0.005;
    aapl.ev_to_fcf = 39.0; aapl.fcf_yield = 0.026;
    aapl.operating_margin = 0.32; aapl.roe = 1.52; aapl.roa = 0.31;
    aapl.net_margin = 0.27; aapl.gross_margin = 0.47;
    aapl.current_ratio = 0.89; aapl.debt_to_equity = 1.34;
    aapl.ev_to_ebit = 29.0;

    Stock jnj{};
    jnj.ticker = "JNJ"; jnj.name = "Johnson & Johnson"; jnj.sector = "Healthcare";
    jnj.price = 155.0; jnj.pe_ratio = 15.0; jnj.dividend_yield = 0.030;
    jnj.ev_to_fcf = 22.0; jnj.fcf_yield = 0.045;
    jnj.operating_margin = 0.25; jnj.roe = 0.20; jnj.roa = 0.08;
    jnj.net_margin = 0.18; jnj.gross_margin = 0.69;
    jnj.current_ratio = 1.3; jnj.debt_to_equity = 0.50;
    jnj.ev_to_ebit = 18.0;

    Stock xom{};
    xom.ticker = "XOM"; xom.name = "Exxon Mobil"; xom.sector = "Energy";
    xom.price = 105.0; xom.pe_ratio = 10.0; xom.dividend_yield = 0.035;
    xom.ev_to_fcf = 15.0; xom.fcf_yield = 0.065;
    xom.operating_margin = 0.15; xom.roe = 0.18; xom.roa = 0.09;
    xom.net_margin = 0.10; xom.gross_margin = 0.35;
    xom.current_ratio = 1.4; xom.debt_to_equity = 0.40;
    xom.ev_to_ebit = 12.0;

    Stock tsla{};
    tsla.ticker = "TSLA"; tsla.name = "Tesla Inc."; tsla.sector = "Automotive";
    tsla.price = 250.0; tsla.pe_ratio = 65.0; tsla.dividend_yield = 0.000;
    tsla.ev_to_fcf = 80.0; tsla.fcf_yield = 0.008;
    tsla.operating_margin = 0.08; tsla.roe = 0.25; tsla.roa = 0.05;
    tsla.net_margin = 0.06; tsla.gross_margin = 0.18;
    tsla.current_ratio = 1.8; tsla.debt_to_equity = 0.10;
    tsla.ev_to_ebit = 55.0;

    Stock ko{};
    ko.ticker = "KO"; ko.name = "Coca-Cola Co."; ko.sector = "Consumer";
    ko.price = 60.0; ko.pe_ratio = 24.0; ko.dividend_yield = 0.031;
    ko.ev_to_fcf = 28.0; ko.fcf_yield = 0.035;
    ko.operating_margin = 0.30; ko.roe = 0.38; ko.roa = 0.10;
    ko.net_margin = 0.23; ko.gross_margin = 0.60;
    ko.current_ratio = 1.1; ko.debt_to_equity = 1.60;
    ko.ev_to_ebit = 22.0;

    return {aapl, jnj, xom, tsla, ko};
}

// ═══════════════════════════════════════════════════════════════════════
//  TEST 1 — Multiple predefined strategies exist
// ═══════════════════════════════════════════════════════════════════════
static void testMultiplePresetsExist()
{
    std::cout << "\n--- Test: Multiple predefined strategies exist ---\n";

    StrategyManager mgr;
    auto names = mgr.getAvailableStrategies();

    CHECK(names.size() >= 5, "At least 5 presets registered");
    CHECK(std::find(names.begin(), names.end(), "Value") != names.end(), "Value preset exists");
    CHECK(std::find(names.begin(), names.end(), "Growth") != names.end(), "Growth preset exists");
    CHECK(std::find(names.begin(), names.end(), "Momentum") != names.end(), "Momentum preset exists");
    CHECK(std::find(names.begin(), names.end(), "Quality") != names.end(), "Quality preset exists");
    CHECK(std::find(names.begin(), names.end(), "Balanced") != names.end(), "Balanced preset exists");

    for (const auto &name : names)
    {
        const auto &strat = mgr.getStrategy(name);
        CHECK(!strat.weights.empty(), name + " has weights defined");
        CHECK(!strat.description.empty(), name + " has a description");
    }
}

// ═══════════════════════════════════════════════════════════════════════
//  TEST 2 — Selecting a strategy updates weights
// ═══════════════════════════════════════════════════════════════════════
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

// ═══════════════════════════════════════════════════════════════════════
//  TEST 3 — Re-running scoring reflects the updated weights
// ═══════════════════════════════════════════════════════════════════════
static void testRankingsChangeWithStrategy()
{
    std::cout << "\n--- Test: Rankings change when strategy changes ---\n";

    StrategyManager mgr;
    ValueScoringModel v; GrowthScoringModel g; MomentumScoringModel m;
    QualityScoringModel q; EfficiencyScoringModel e;
    registerAllModels(mgr, v, g, m, q, e);

    auto stocks = makeSampleStocks();

    mgr.setActiveStrategy("Value");
    auto value_ranked = mgr.rankStocks(stocks);
    std::string value_top = value_ranked.front().stock.ticker;

    mgr.setActiveStrategy("Growth");
    auto growth_ranked = mgr.rankStocks(stocks);
    std::string growth_top = growth_ranked.front().stock.ticker;

    mgr.setActiveStrategy("Quality");
    auto quality_ranked = mgr.rankStocks(stocks);
    std::string quality_top = quality_ranked.front().stock.ticker;

    std::cout << "    Value top pick:   " << value_top << "\n";
    std::cout << "    Growth top pick:  " << growth_top << "\n";
    std::cout << "    Quality top pick: " << quality_top << "\n";

    CHECK(value_top == "XOM" || value_top == "JNJ",
          "Value strategy top pick is a value stock (XOM or JNJ)");
    CHECK(value_top != growth_top || growth_top != quality_top,
          "Different strategies produce different rankings");

    for (const auto &ss : value_ranked)
    {
        CHECK(ss.composite >= 0.0, ss.stock.ticker + " has non-negative composite score");
    }
}

// ═══════════════════════════════════════════════════════════════════════
//  TEST 4 — Default strategy applied on startup
// ═══════════════════════════════════════════════════════════════════════
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

// ═══════════════════════════════════════════════════════════════════════
//  TEST 5 — Switching strategies without restart
// ═══════════════════════════════════════════════════════════════════════
static void testSwitchWithoutRestart()
{
    std::cout << "\n--- Test: Switch strategies without restart ---\n";

    StrategyManager mgr;
    ValueScoringModel v; GrowthScoringModel g; MomentumScoringModel m;
    QualityScoringModel q; EfficiencyScoringModel e;
    registerAllModels(mgr, v, g, m, q, e);

    auto stocks = makeSampleStocks();

    mgr.setActiveStrategy("Value");
    auto ranked1 = mgr.rankStocks(stocks);
    double xom1 = 0.0;
    for (const auto &ss : ranked1)
    {
        if (ss.stock.ticker == "XOM") { xom1 = ss.composite; break; }
    }

    mgr.setActiveStrategy("Growth");
    auto ranked2 = mgr.rankStocks(stocks);
    double xom2 = 0.0;
    for (const auto &ss : ranked2)
    {
        if (ss.stock.ticker == "XOM") { xom2 = ss.composite; break; }
    }

    CHECK(!approxEqual(xom1, xom2), "XOM composite differs between Value and Growth");

    mgr.setActiveStrategy("Balanced");
    auto ranked3 = mgr.rankStocks(stocks);
    CHECK(!ranked3.empty(), "Balanced ranking returns results without restart");

    std::cout << "    XOM composite (Value):  " << xom1 << "\n";
    std::cout << "    XOM composite (Growth): " << xom2 << "\n";
}

// ═══════════════════════════════════════════════════════════════════════
//  TEST 6 — Individual scoring models produce sensible results
// ═══════════════════════════════════════════════════════════════════════
static void testScoringModels()
{
    std::cout << "\n--- Test: Individual scoring models ---\n";

    auto stocks = makeSampleStocks();
    Stock xom{}, tsla{}, jnj{};
    for (const auto &s : stocks)
    {
        if (s.ticker == "XOM") xom = s;
        if (s.ticker == "TSLA") tsla = s;
        if (s.ticker == "JNJ") jnj = s;
    }

    ValueScoringModel value_model;
    CHECK(value_model.calculateScore(xom) > value_model.calculateScore(tsla),
          "Value model scores XOM higher than TSLA");

    GrowthScoringModel growth_model;
    CHECK(growth_model.calculateScore(tsla) > growth_model.calculateScore(xom),
          "Growth model scores TSLA higher than XOM");

    MomentumScoringModel momentum_model;
    double mom = momentum_model.calculateScore(xom);
    CHECK(mom >= 0.0 && mom <= 100.0, "Momentum score is in [0, 100] range");

    QualityScoringModel quality_model;
    CHECK(quality_model.calculateScore(jnj) > quality_model.calculateScore(tsla),
          "Quality model scores JNJ higher than TSLA (stronger balance sheet)");

    EfficiencyScoringModel efficiency_model;
    CHECK(efficiency_model.calculateScore(xom) > efficiency_model.calculateScore(tsla),
          "Efficiency model scores XOM higher than TSLA (better EV/EBIT, ROA)");
}

// ═══════════════════════════════════════════════════════════════════════
//  TEST 7 — Deterministic scoring
// ═══════════════════════════════════════════════════════════════════════
static void testDeterministicScoring()
{
    std::cout << "\n--- Test: Deterministic scoring ---\n";

    StrategyManager mgr;
    ValueScoringModel v; GrowthScoringModel g; MomentumScoringModel m;
    QualityScoringModel q; EfficiencyScoringModel e;
    registerAllModels(mgr, v, g, m, q, e);

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

// ═══════════════════════════════════════════════════════════════════════
//  TEST 8 — Ranking order is descending
// ═══════════════════════════════════════════════════════════════════════
static void testRankingOrder()
{
    std::cout << "\n--- Test: Ranking order is descending ---\n";

    StrategyManager mgr;
    ValueScoringModel v; GrowthScoringModel g; MomentumScoringModel m;
    QualityScoringModel q; EfficiencyScoringModel e;
    registerAllModels(mgr, v, g, m, q, e);

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

// ═══════════════════════════════════════════════════════════════════════
//  TEST 9 — Breakdown includes all factor subscores
// ═══════════════════════════════════════════════════════════════════════
static void testBreakdownIncludesFactors()
{
    std::cout << "\n--- Test: Breakdown includes factor subscores ---\n";

    StrategyManager mgr;
    ValueScoringModel v; GrowthScoringModel g; MomentumScoringModel m;
    QualityScoringModel q; EfficiencyScoringModel e;
    registerAllModels(mgr, v, g, m, q, e);

    mgr.setActiveStrategy("Balanced");
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
    CHECK(scored.subscores.size() == 5, "Balanced strategy produces 5 subscores");
}

// ═══════════════════════════════════════════════════════════════════════
//  TEST 10 — Missing data defaults to zero
// ═══════════════════════════════════════════════════════════════════════
static void testMissingDataDefaultsToZero()
{
    std::cout << "\n--- Test: Missing data defaults to zero ---\n";

    StrategyManager mgr;
    ValueScoringModel v; GrowthScoringModel g; MomentumScoringModel m;
    QualityScoringModel q; EfficiencyScoringModel e;
    registerAllModels(mgr, v, g, m, q, e);

    Stock broken{};
    broken.ticker = "NULL";
    broken.name = "Missing Data";

    auto scored = mgr.scoreStock(broken);
    CHECK(scored.composite >= 0.0 && scored.composite <= 100.0, "Composite score is in valid range even with missing data");
}

// ═══════════════════════════════════════════════════════════════════════
//  TEST 11 — Quality model validates financial health ranking
// ═══════════════════════════════════════════════════════════════════════
static void testQualityModelRanking()
{
    std::cout << "\n--- Test: Quality model financial health ranking ---\n";

    StrategyManager mgr;
    ValueScoringModel v; GrowthScoringModel g; MomentumScoringModel m;
    QualityScoringModel q; EfficiencyScoringModel e;
    registerAllModels(mgr, v, g, m, q, e);

    mgr.setActiveStrategy("Quality");
    auto ranked = mgr.rankStocks(makeSampleStocks());

    std::cout << "    Quality ranking:\n";
    for (const auto &ss : ranked)
    {
        std::cout << "      " << ss.stock.ticker << ": " << ss.composite << "\n";
    }

    /// JNJ should rank well under Quality (low debt, high gross margin, solid ROE).
    bool jnj_in_top2 = (ranked[0].stock.ticker == "JNJ" || ranked[1].stock.ticker == "JNJ");
    CHECK(jnj_in_top2, "JNJ ranks in top 2 under Quality strategy");
}

// ═══════════════════════════════════════════════════════════════════════
//  TEST 12 — All five models produce scores in valid range
// ═══════════════════════════════════════════════════════════════════════
static void testAllModelsValidRange()
{
    std::cout << "\n--- Test: All models produce scores in [0, 100] ---\n";

    ValueScoringModel v; GrowthScoringModel g; MomentumScoringModel m;
    QualityScoringModel q; EfficiencyScoringModel e;
    std::vector<ScoringModel *> models = {&v, &g, &m, &q, &e};

    auto stocks = makeSampleStocks();
    bool all_valid = true;

    for (auto *model : models)
    {
        for (const auto &stock : stocks)
        {
            double score = model->calculateScore(stock);
            if (score < 0.0 || score > 100.0)
            {
                std::cout << "    OUT OF RANGE: " << model->getModelName()
                          << " scored " << stock.ticker << " at " << score << "\n";
                all_valid = false;
            }
        }
    }
    CHECK(all_valid, "Every model/stock combination produces a score in [0, 100]");
}

// ═══════════════════════════════════════════════════════════════════════
//  MAIN
// ═══════════════════════════════════════════════════════════════════════
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
    testQualityModelRanking();
    testAllModelsValidRange();

    std::cout << "\n============================================\n";
    std::cout << "  Results: " << passed << " passed, " << failed << " failed\n";
    std::cout << "============================================\n";

    return (failed > 0) ? 1 : 0;
}

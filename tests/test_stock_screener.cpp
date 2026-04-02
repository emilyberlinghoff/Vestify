/**
 * @file test_stock_screener.cpp
 * @brief Unit tests for multi-criteria stock screening.
 */

#include <iostream>
#include <string>
#include <vector>

#include "screening/StockScreener.hpp"

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

static std::vector<Stock> makeStocks()
{
    Stock a;
    a.ticker = "AAPL";
    a.name = "Apple";
    a.sector = "Technology";
    a.price = 180.0;
    a.pe_ratio = 28.0;
    a.ev_to_fcf = 24.0;
    a.fcf_yield = 0.05;
    a.roe = 0.30;
    a.operating_margin = 0.29;
    a.debt_to_equity = 1.2;
    a.current_ratio = 0.95;
    a.dividend_yield = 0.005;

    Stock b;
    b.ticker = "MSFT";
    b.name = "Microsoft";
    b.sector = "Technology";
    b.price = 420.0;
    b.pe_ratio = 32.0;
    b.ev_to_fcf = 26.0;
    b.fcf_yield = 0.04;
    b.roe = 0.34;
    b.operating_margin = 0.44;
    b.debt_to_equity = 0.4;
    b.current_ratio = 1.3;
    b.dividend_yield = 0.008;

    Stock c;
    c.ticker = "COST";
    c.name = "Costco";
    c.sector = "Consumer Defensive";
    c.price = 720.0;
    c.pe_ratio = 45.0;
    c.ev_to_fcf = 35.0;
    c.fcf_yield = 0.02;
    c.roe = 0.22;
    c.operating_margin = 0.04;
    c.debt_to_equity = 0.3;
    c.current_ratio = 1.1;
    c.dividend_yield = 0.006;

    return {a, b, c};
}

static void testSectorAndValuationCriteria()
{
    std::cout << "\n--- Test: sector and valuation criteria combine with AND semantics ---\n";

    const auto result = StockScreener::screen(
        makeStocks(),
        {
            {StockScreener::CriterionType::SectorEquals, 0.0, 0.0, "technology"},
            {StockScreener::CriterionType::MaxPeRatio, 0.0, 30.0, ""}
        });

    CHECK(result.matches.size() == 1, "One stock matches both filters");
    CHECK(result.matches[0].ticker == "AAPL", "AAPL survives sector + PE screen");
    CHECK(result.appliedCriteria.size() == 2, "Applied criteria are described");
}

static void testRangeAndQualityCriteria()
{
    std::cout << "\n--- Test: price range and quality criteria can be combined ---\n";

    const auto result = StockScreener::screen(
        makeStocks(),
        {
            {StockScreener::CriterionType::PriceBetween, 150.0, 500.0, ""},
            {StockScreener::CriterionType::MinOperatingMargin, 0.25, 0.0, ""},
            {StockScreener::CriterionType::MaxDebtToEquity, 0.0, 0.5, ""}
        });

    CHECK(result.matches.size() == 1, "One stock matches range + quality filters");
    CHECK(result.matches[0].ticker == "MSFT", "MSFT survives the combined screen");
}

static void testEmptyCriteriaReturnsAllStocks()
{
    std::cout << "\n--- Test: empty criteria returns the full universe ---\n";

    const auto stocks = makeStocks();
    const auto result = StockScreener::screen(stocks, {});

    CHECK(result.matches.size() == stocks.size(), "All stocks returned when no criteria are supplied");
}

int main()
{
    std::cout << "============================================\n";
    std::cout << "  Stock Screener - Unit Test Suite\n";
    std::cout << "============================================\n";

    testSectorAndValuationCriteria();
    testRangeAndQualityCriteria();
    testEmptyCriteriaReturnsAllStocks();

    std::cout << "\n============================================\n";
    std::cout << "  Results: " << passed << " passed, " << failed << " failed\n";
    std::cout << "============================================\n";

    return (failed > 0) ? 1 : 0;
}

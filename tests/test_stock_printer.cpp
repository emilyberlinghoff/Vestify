/**
 * @file test_stock_printer.cpp
 * @brief Unit tests for stock display and ranking output helpers.
 * @author Maxime Lavoie
 */

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "ui/StockPrinter.hpp"

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

static std::string captureStdout(const std::function<void()> &fn)
{
    std::ostringstream buffer;
    auto *original = std::cout.rdbuf(buffer.rdbuf());
    fn();
    std::cout.rdbuf(original);
    return buffer.str();
}

static std::vector<Stock> makeStocks()
{
    Stock a;
    a.ticker = "AAPL";
    a.name = "Apple Inc.";
    a.sector = "Technology";
    a.ev_to_fcf = 24.0;

    Stock b;
    b.ticker = "MSFT";
    b.name = "Microsoft Corp.";
    b.sector = "Technology";
    b.ev_to_fcf = 18.0;

    return {a, b};
}

static void testPrintAvailableStocksShowsSummary()
{
    std::cout << "\n--- Test: printAvailableStocks shows headings and stock rows ---\n";

    const std::string output = captureStdout(
        []()
        {
            StockPrinter::printAvailableStocks(makeStocks());
        });

    CHECK(output.find("Available Stocks") != std::string::npos, "Summary heading is printed");
    CHECK(output.find("AAPL") != std::string::npos, "First ticker is printed");
    CHECK(output.find("Microsoft Corp.") != std::string::npos, "Company name is printed");
}

static void testPrintAvailableStocksHandlesEmptyInput()
{
    std::cout << "\n--- Test: printAvailableStocks handles empty input ---\n";

    const std::string output = captureStdout(
        []()
        {
            StockPrinter::printAvailableStocks({});
        });

    CHECK(output.find("No stock data loaded.") != std::string::npos, "Empty-state message is printed");
}

static void testPrintTopEvFcfSortsAscending()
{
    std::cout << "\n--- Test: printTopEVFCF sorts stocks by cheapest EV/FCF first ---\n";

    const std::string output = captureStdout(
        []()
        {
            StockPrinter::printTopEVFCF(makeStocks(), 2);
        });

    const auto msftPos = output.find("MSFT");
    const auto aaplPos = output.find("AAPL");

    CHECK(msftPos != std::string::npos, "MSFT row is printed");
    CHECK(aaplPos != std::string::npos, "AAPL row is printed");
    CHECK(msftPos < aaplPos, "Lower EV/FCF stock appears first");
}

int main()
{
    std::cout << "============================================\n";
    std::cout << "  Stock Printer - Unit Test Suite\n";
    std::cout << "============================================\n";

    testPrintAvailableStocksShowsSummary();
    testPrintAvailableStocksHandlesEmptyInput();
    testPrintTopEvFcfSortsAscending();

    std::cout << "\n============================================\n";
    std::cout << "  Results: " << passed << " passed, " << failed << " failed\n";
    std::cout << "============================================\n";

    return (failed > 0) ? 1 : 0;
}

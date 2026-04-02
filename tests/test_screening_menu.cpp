/**
 * @file test_screening_menu.cpp
 * @brief Unit tests for the interactive screening menu flow.
 */

#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ui/ScreeningMenu.hpp"

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

struct FakeInput
{
    std::vector<int> ints;
    std::vector<std::string> lines;
    std::size_t intIndex = 0;
    std::size_t lineIndex = 0;

    int nextInt()
    {
        if (intIndex >= ints.size())
        {
            throw std::runtime_error("Ran out of integer inputs");
        }
        return ints[intIndex++];
    }

    std::string nextLine(const std::string &)
    {
        if (lineIndex >= lines.size())
        {
            throw std::runtime_error("Ran out of line inputs");
        }
        return lines[lineIndex++];
    }
};

static std::vector<Stock> makeStocks()
{
    Stock a;
    a.ticker = "AAPL";
    a.name = "Apple";
    a.sector = "Technology";
    a.price = 180.0;
    a.pe_ratio = 28.0;

    Stock b;
    b.ticker = "MSFT";
    b.name = "Microsoft";
    b.sector = "Technology";
    b.price = 420.0;
    b.pe_ratio = 32.0;

    Stock c;
    c.ticker = "JPM";
    c.name = "JPMorgan";
    c.sector = "Financial Services";
    c.price = 210.0;
    c.pe_ratio = 15.0;

    return {a, b, c};
}

static void testScreeningMenuListsSectorsAndFilters()
{
    std::cout << "\n--- Test: screening menu lists sectors and prints current filters ---\n";

    FakeInput input;
    input.ints = {
        1,  // choose sector filter
        2,  // select Technology after alphabetical sort
        13, // print current filters
        14  // run screen
    };
    input.lines = {"n"};

    const std::string output = captureStdout(
        [&]()
        {
            ScreeningMenu::run(
                makeStocks(),
                [&input]()
                {
                    return input.nextInt();
                },
                [&input](const std::string &prompt)
                {
                    return input.nextLine(prompt);
                });
        });

    CHECK(output.find("Available sectors:") != std::string::npos, "Sector list is printed");
    CHECK(output.find("Financial Services") != std::string::npos, "Distinct sectors are shown");
    CHECK(output.find("Technology") != std::string::npos, "Selected sector is shown");
    CHECK(output.find("Current filters:") != std::string::npos, "Current-filter summary is printed");
    CHECK(output.find("Sector = Technology") != std::string::npos, "Selected filter appears in summary");
    CHECK(output.find("Found 2 matching stocks.") != std::string::npos, "Filtered result count is printed");
}

static void testScreeningMenuHandlesEmptyStocks()
{
    std::cout << "\n--- Test: screening menu handles empty stock universe ---\n";

    const std::string output = captureStdout(
        []()
        {
            ScreeningMenu::run(
                {},
                []()
                {
                    return 0;
                },
                [](const std::string &)
                {
                    return std::string();
                });
        });

    CHECK(output.find("No stock data loaded.") != std::string::npos, "Empty-state message is printed");
}

int main()
{
    std::cout << "============================================\n";
    std::cout << "  Screening Menu - Unit Test Suite\n";
    std::cout << "============================================\n";

    testScreeningMenuListsSectorsAndFilters();
    testScreeningMenuHandlesEmptyStocks();

    std::cout << "\n============================================\n";
    std::cout << "  Results: " << passed << " passed, " << failed << " failed\n";
    std::cout << "============================================\n";

    return (failed > 0) ? 1 : 0;
}

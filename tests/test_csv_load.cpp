/**
 * @file test_csv_load.cpp
 * @brief Unit tests for CSV loading into the stock repository.
 */

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "core/StockRepository.hpp"

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

static std::string writeTempCsv(const std::string &contents)
{
    auto dir = std::filesystem::temp_directory_path();
    auto path = dir / "vestify_test.csv";
    std::ofstream out(path);
    out << contents;
    out.close();
    return path.string();
}

static void testValidCsvLoads()
{
    std::cout << "\n--- Test: Valid CSV loads stocks with fields ---\n";
    const std::string csv =
        "ticker,name,sector,price,pe_ratio,dividend_yield\n"
        "AAPL,Apple Inc.,Technology,150.5,20.1,0.015\n"
        "MSFT,Microsoft Corp.,Technology,310.0,30.2,0.01\n";

    std::string path = writeTempCsv(csv);
    StockRepository repo;
    auto result = repo.loadFromCsv(path);

    CHECK(result.errors.empty(), "No errors for valid CSV");
    CHECK(result.stocks.size() == 2, "Two stocks loaded");
    CHECK(result.stocks[0].ticker == "AAPL", "Ticker parsed");
    CHECK(result.stocks[0].name == "Apple Inc.", "Name parsed");
    CHECK(result.stocks[0].sector == "Technology", "Sector parsed");
    CHECK(result.stocks[0].price == 150.5, "Price parsed");
    CHECK(result.stocks[0].pe_ratio == 20.1, "P/E ratio parsed");
    CHECK(result.stocks[0].dividend_yield == 0.015, "Dividend yield parsed");
}

static void testMissingFileHandled()
{
    std::cout << "\n--- Test: Missing CSV file handled gracefully ---\n";
    StockRepository repo;
    auto result = repo.loadFromCsv("this_file_should_not_exist.csv");
    CHECK(result.stocks.empty(), "No stocks loaded for missing file");
    CHECK(!result.errors.empty(), "Error reported for missing file");
}

static void testMalformedRowSkipped()
{
    std::cout << "\n--- Test: Malformed row is skipped ---\n";
    const std::string csv =
        "ticker,name,sector,price,pe_ratio,dividend_yield\n"
        "AAPL,Apple Inc.,Technology,150.5,20.1,0.015\n"
        "BAD,Bad Co.,Technology,not_a_number,10.0,0.01\n";

    std::string path = writeTempCsv(csv);
    StockRepository repo;
    auto result = repo.loadFromCsv(path);

    CHECK(result.stocks.size() == 1, "Malformed row skipped");
    CHECK(!result.errors.empty(), "Error logged for malformed row");
}

int main()
{
    std::cout << "============================================\n";
    std::cout << "  CSV Load - Unit Test Suite\n";
    std::cout << "============================================\n";

    testValidCsvLoads();
    testMissingFileHandled();
    testMalformedRowSkipped();

    std::cout << "\n============================================\n";
    std::cout << "  Results: " << passed << " passed, " << failed << " failed\n";
    std::cout << "============================================\n";

    return (failed > 0) ? 1 : 0;
}

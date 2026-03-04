#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "core/StockRepository.hpp"

/**
 * @brief Simple test assertion helper.
 *
 * @param condition Condition that must be true.
 * @param message Message printed on failure.
 * @return True if the condition is true.
 */
bool assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "TEST FAILURE: " << message << "\n";
    }
    return condition;
}

/**
 * @brief Write test content to a temporary file.
 *
 * @param path File path.
 * @param contents Contents to write.
 */
void writeTempFile(const std::string& path, const std::string& contents) {
    std::ofstream out(path);
    out << contents;
}

/**
 * @brief Test valid CSV parsing via repository.
 *
 * @return True if the test passes.
 */
bool testValidCsv() {
    std::stringstream input;
    input << "ticker,name,sector,price,marketCap,peRatio,dividendYield\n";
    input << "AAPL,Apple,Technology,150.5,2500000000000,25.1,0.006\n";

    const std::string path = "/tmp/vestify_test_valid.csv";
    writeTempFile(path, input.str());

    StockRepository repo;
    auto result = repo.loadFromCsv(path);

    bool ok = true;
    ok &= assertTrue(result.errors.empty(), "Expected no errors for valid CSV");
    ok &= assertTrue(result.stocks.size() == 1, "Expected exactly 1 stock");
    if (!result.stocks.empty()) {
        const auto& stock = result.stocks.front();
        ok &= assertTrue(stock.ticker == "AAPL", "Ticker mismatch");
        ok &= assertTrue(stock.name == "Apple", "Name mismatch");
        ok &= assertTrue(stock.sector == "Technology", "Sector mismatch");
        ok &= assertTrue(stock.price == 150.5, "Price mismatch");
        ok &= assertTrue(stock.market_cap == 2500000000000.0, "Market cap mismatch");
        ok &= assertTrue(stock.pe_ratio == 25.1, "PE ratio mismatch");
        ok &= assertTrue(stock.dividend_yield == 0.006, "Dividend yield mismatch");
    }
    return ok;
}

/**
 * @brief Test missing file handling via repository.
 *
 * @return True if the test passes.
 */
bool testMissingFile() {
    StockRepository repo;
    auto result = repo.loadFromCsv("/tmp/this_file_should_not_exist_123.csv");

    bool ok = true;
    ok &= assertTrue(!result.errors.empty(), "Expected error for missing file");
    ok &= assertTrue(result.stocks.empty(), "Expected no stocks for missing file");
    if (!result.errors.empty()) {
        ok &= assertTrue(
            result.errors.front().find("Failed to open CSV file") != std::string::npos,
            "Missing file error message mismatch"
        );
    }
    return ok;
}

/**
 * @brief Test malformed row handling via repository.
 *
 * @return True if the test passes.
 */
bool testMalformedRow() {
    std::stringstream input;
    input << "ticker,name,sector,price,marketCap,peRatio,dividendYield\n";
    input << "AAPL,Apple,Technology,150.5,2500000000000,25.1,0.006\n";
    input << "MSFT,Microsoft,Technology,NOT_A_NUMBER,2100000000000,30.2,0.007\n";

    const std::string path = "/tmp/vestify_test_malformed.csv";
    writeTempFile(path, input.str());

    StockRepository repo;
    auto result = repo.loadFromCsv(path);

    bool ok = true;
    ok &= assertTrue(result.stocks.size() == 1, "Expected malformed row to be skipped");
    ok &= assertTrue(!result.errors.empty(), "Expected error for malformed row");
    return ok;
}

/**
 * @brief Test runner for CSV loader acceptance tests.
 *
 * @return Exit code.
 */
int main() {
    bool ok = true;
    ok &= testValidCsv();
    ok &= testMissingFile();
    ok &= testMalformedRow();

    if (!ok) {
        return EXIT_FAILURE;
    }
    std::cout << "All CSV loader tests passed.\n";
    return EXIT_SUCCESS;
}

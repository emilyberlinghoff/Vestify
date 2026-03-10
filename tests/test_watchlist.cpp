#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "core/Watchlist.hpp"
#include "persistence/WatchlistRep.hpp"

static const std::string TEMP_FILE = "test_wl_tmp.json";

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
 * @brief Remove the temporary test file if it exists.
 */
void removeTempFile() {
    std::remove(TEMP_FILE.c_str());
}

/**
 * @brief Test in-memory watchlist add, remove, has and duplicate handling.
 *
 * @return True if the test passes.
 */
bool testBasicOps() {
    Watchlist wl;

    bool ok = true;
    ok &= assertTrue(wl.empty(), "New watchlist should be empty");
    ok &= assertTrue(wl.add("AAPL"), "Should add AAPL");
    ok &= assertTrue(wl.add("MSFT"), "Should add MSFT");
    ok &= assertTrue(wl.size() == 2, "Should have 2 items");

    ok &= assertTrue(!wl.add("AAPL"), "Duplicate AAPL should fail");
    ok &= assertTrue(wl.size() == 2, "Still 2 after dup attempt");

    ok &= assertTrue(wl.has("AAPL"), "Should contain AAPL");
    ok &= assertTrue(!wl.has("GOOG"), "Should not contain GOOG");

    ok &= assertTrue(wl.remove("AAPL"), "Should remove AAPL");
    ok &= assertTrue(!wl.has("AAPL"), "AAPL gone after remove");
    ok &= assertTrue(!wl.remove("NOPE"), "Removing nonexistent returns false");

    return ok;
}

/**
 * @brief Test saving a non-empty watchlist and loading it back.
 *
 * @return True if the test passes.
 */
bool testSaveAndLoad() {
    removeTempFile();

    Watchlist wl;
    wl.add("AAPL");
    wl.add("MSFT");
    wl.add("GOOG");

    WatchlistRepo repo(TEMP_FILE);
    auto saved = repo.save(wl);
    bool ok = assertTrue(saved.ok, "Save should succeed");

    auto loaded = repo.load();
    ok &= assertTrue(loaded.errors.empty(), "No errors on load");
    ok &= assertTrue(loaded.watchlist.size() == 3, "Should load 3 tickers");
    ok &= assertTrue(loaded.watchlist.has("AAPL"), "Loaded AAPL");
    ok &= assertTrue(loaded.watchlist.has("MSFT"), "Loaded MSFT");
    ok &= assertTrue(loaded.watchlist.has("GOOG"), "Loaded GOOG");

    removeTempFile();
    return ok;
}

/**
 * @brief Test loading when no watchlist file exists on disk.
 *
 * @return True if the test passes.
 */
bool testNoFile() {
    removeTempFile();

    WatchlistRepo repo(TEMP_FILE);
    auto res = repo.load();

    bool ok = true;
    ok &= assertTrue(res.errors.empty(), "No errors when file missing");
    ok &= assertTrue(res.watchlist.empty(), "Empty watchlist when no file");
    return ok;
}

/**
 * @brief Test loading a corrupted JSON file.
 *
 * @return True if the test passes.
 */
bool testCorruptedFile() {
    removeTempFile();

    {
        std::ofstream f(TEMP_FILE);
        f << "not json at all {{{garbage";
    }

    WatchlistRepo repo(TEMP_FILE);
    auto res = repo.load();

    bool ok = true;
    ok &= assertTrue(!res.errors.empty(), "Should report parse error");
    ok &= assertTrue(res.watchlist.empty(), "Watchlist empty on bad file");

    removeTempFile();
    return ok;
}

/**
 * @brief Test that loaded data matches exactly what was saved.
 *
 * @return True if the test passes.
 */
bool testDataMatchesAfterRoundTrip() {
    removeTempFile();

    Watchlist orig;
    orig.add("TSLA");
    orig.add("NVDA");
    orig.add("AMZN");

    WatchlistRepo repo(TEMP_FILE);
    repo.save(orig);

    auto loaded = repo.load();
    bool ok = assertTrue(loaded.errors.empty(), "Clean load");

    const auto& before = orig.getAll();
    const auto& after = loaded.watchlist.getAll();
    ok &= assertTrue(before.size() == after.size(), "Same count");

    for (size_t i = 0; i < before.size(); i++) {
        ok &= assertTrue(before[i] == after[i],
            "Mismatch at index " + std::to_string(i));
    }

    removeTempFile();
    return ok;
}

/**
 * @brief Test that saving and loading an empty watchlist works.
 *
 * @return True if the test passes.
 */
bool testSaveEmpty() {
    removeTempFile();

    Watchlist wl;
    WatchlistRepo repo(TEMP_FILE);

    auto saved = repo.save(wl);
    bool ok = assertTrue(saved.ok, "Saving empty should work");

    auto loaded = repo.load();
    ok &= assertTrue(loaded.errors.empty(), "No errors loading empty");
    ok &= assertTrue(loaded.watchlist.empty(), "Still empty after roundtrip");

    removeTempFile();
    return ok;
}

/**
 * @brief Test runner for watchlist persistence acceptance tests.
 *
 * @return Exit code.
 */
int main() {
    bool ok = true;
    ok &= testBasicOps();
    ok &= testSaveAndLoad();
    ok &= testNoFile();
    ok &= testCorruptedFile();
    ok &= testDataMatchesAfterRoundTrip();
    ok &= testSaveEmpty();

    if (!ok) {
        return EXIT_FAILURE;
    }
    std::cout << "All watchlist tests passed.\n";
    return EXIT_SUCCESS;
}

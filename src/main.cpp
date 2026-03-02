#include <iostream>

/**
 * @brief Vestify entry point.
 *
 * Prints a minimal CLI menu stub to satisfy initial build/run acceptance tests.
 *
 * @return Exit status code.
 */
int main() {
    std::cout << "Vestify\n";
    std::cout << "1. Load stock data (CSV)\n";
    std::cout << "2. View watchlist\n";
    std::cout << "3. View portfolio\n";
    std::cout << "4. Exit\n";
    std::cout << "> ";

    return 0;
}

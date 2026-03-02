#pragma once

#include <string>

/**
 * @brief Represents a stock with core fields used for screening and display.
 */
struct Stock {
    /** @brief Ticker symbol (e.g., AAPL). */
    std::string ticker;
    /** @brief Company name. */
    std::string name;
    /** @brief Sector name (may be empty if unknown). */
    std::string sector;
    /** @brief Latest price. */
    double price = 0.0;
    /** @brief Market capitalization. */
    double market_cap = 0.0;
    /** @brief Price-to-earnings ratio. */
    double pe_ratio = 0.0;
    /** @brief Dividend yield (as a fraction, e.g., 0.0125 = 1.25%). */
    double dividend_yield = 0.0;
};

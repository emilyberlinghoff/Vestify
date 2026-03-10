/**
 * @file StockPrinter.hpp
 * @brief Header for the StockPrinter class providing formatted stock data display.
 *
 * This header defines the StockPrinter class which provides static methods for
 * formatting and displaying stock information in various formats. It handles
 * monetary value formatting, metric colorization, and tabular display of
 * stock data with proper alignment and visual indicators.
 */

#pragma once

#include <string>
#include <vector>

#include "core/Stock.hpp"

/**
 * @brief Utility class for formatting and displaying stock information.
 *
 * The StockPrinter class provides static methods for displaying stock data
 * in various formats including detailed stock information, summary lists,
 * and ranked displays. It includes formatting utilities for monetary values
 * and colorized display of financial metrics based on performance thresholds.
 *
 * All methods are static, making this a utility class that doesn't require
 * instantiation. Output is directed to stdout with ANSI color codes for
 * enhanced readability in terminal environments.
 */
class StockPrinter
{
public:
    /**
     * @brief Print detailed information for a collection of stocks.
     *
     * Displays comprehensive financial information for each stock including
     * basic company data, financial statements, ratios, and valuation metrics.
     * Uses colorized formatting to highlight performance indicators.
     *
     * @param stocks Vector of Stock objects to display.
     */
    static void printStocks(const std::vector<Stock> &stocks);

    /**
     * @brief Print a summary list of available stocks.
     *
     * Displays a tabular summary of all available stocks showing ticker symbols,
     * sectors, and company names in a compact format suitable for selection menus.
     *
     * @param stocks Vector of Stock objects to display.
     */
    static void printAvailableStocks(const std::vector<Stock> &stocks);

    /**
     * @brief Print top stocks ranked by EV/FCF ratio.
     *
     * Displays the top N stocks sorted by Enterprise Value to Free Cash Flow ratio
     * (ascending order, lower is better). Shows ticker, company name, and EV/FCF value.
     *
     * @param stocks Vector of Stock objects to analyze (sorted in-place).
     * @param count Maximum number of stocks to display (default: 10).
     */
    static void printTopEVFCF(std::vector<Stock> stocks, std::size_t count = 10);

private:
    /**
     * @brief Format a monetary value with appropriate suffixes.
     *
     * Converts large numbers to a more readable format using suffixes:
     * K (thousands), M (millions), B (billions), T (trillions).
     *
     * @param value The monetary value to format.
     * @return Formatted string representation with suffix.
     */
    static std::string formatMoney(double value);

    /**
     * @brief Colorize a metric value where higher values are generally better.
     *
     * Applies ANSI color codes to metric values where higher values indicate
     * better performance (e.g., profitability ratios). Uses green for good,
     * yellow for neutral, and red for weak performance.
     *
     * @param value The metric value to colorize.
     * @param goodThreshold Minimum value considered "good" (green).
     * @param weakThreshold Maximum value considered "weak" (red).
     * @return Colorized string representation of the value.
     */
    static std::string colorizeMetric(double value, double goodThreshold, double weakThreshold);

    /**
     * @brief Colorize a metric value where lower values are generally better.
     *
     * Applies ANSI color codes to valuation metrics where lower values indicate
     * better performance (e.g., P/E ratio). Uses green for attractive valuations,
     * yellow for neutral, and red for expensive valuations.
     *
     * @param value The metric value to colorize.
     * @param goodThreshold Maximum value considered "good" (green).
     * @param badThreshold Minimum value considered "bad" (red).
     * @return Colorized string representation of the value.
     */
    static std::string colorizeLowIsBetter(double value, double goodThreshold, double badThreshold);
};
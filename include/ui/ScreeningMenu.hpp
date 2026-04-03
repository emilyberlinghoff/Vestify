/**
 * @file ScreeningMenu.hpp
 * @brief Header for the dedicated interactive stock screening flow.
 * @author Group 13
 */

#pragma once

#include <functional>
#include <string>
#include <vector>

#include "core/Stock.hpp"

/**
 * @brief Dedicated UI flow for interactive stock screening.
 *
 * This class isolates the screening-specific prompt flow from the main menu so
 * users can build, inspect, and execute multi-criteria screens without
 * increasing the complexity of `InteractiveMenu`.
 *
 * @author Group 13
 */
class ScreeningMenu
{
public:
    /**
     * @brief Run the stock screening prompt flow.
     *
     * Presents the available filter menu, lets the user stack multiple
     * criteria, prints the current filter list on demand, and displays the
     * matching stocks after the screen is executed.
     *
     * @param stocks Loaded stocks available for screening.
     * @param readInt Callback for validated integer input.
     * @param readLine Callback for line-based prompt input.
     */
    static void run(
        const std::vector<Stock> &stocks,
        const std::function<int()> &readInt,
        const std::function<std::string(const std::string &)> &readLine);
};

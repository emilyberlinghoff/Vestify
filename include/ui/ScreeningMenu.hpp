#pragma once

#include <functional>
#include <string>
#include <vector>

#include "core/Stock.hpp"

/**
 * @brief Dedicated UI flow for interactive stock screening.
 */
class ScreeningMenu
{
public:
    /**
     * @brief Run the stock screening prompt flow.
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

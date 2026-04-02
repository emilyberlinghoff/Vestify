/**
 * @file StockScreener.hpp
 * @brief Header for reusable multi-criteria stock screening logic.
 */

#pragma once

#include <string>
#include <vector>

#include "core/Stock.hpp"

/**
 * @brief Reusable stock screening engine for filtering loaded fundamentals.
 */
class StockScreener
{
public:
    /**
     * @brief Types of supported screening criteria.
     */
    enum class CriterionType
    {
        SectorEquals,
        PriceBetween,
        MaxPeRatio,
        MaxPbRatio,
        MaxPsRatio,
        MaxEvToFcf,
        MinFcfYield,
        MinRoe,
        MinOperatingMargin,
        MaxDebtToEquity,
        MinCurrentRatio,
        MinDividendYield
    };

    /**
     * @brief A single user-selected screening rule.
     */
    struct Criterion
    {
        CriterionType type;
        double minValue = 0.0;
        double maxValue = 0.0;
        std::string textValue;
    };

    /**
     * @brief Result of screening a stock list.
     */
    struct ScreenResult
    {
        std::vector<Stock> matches;
        std::vector<std::string> appliedCriteria;
    };

    /**
     * @brief Filter stocks by all provided criteria.
     *
     * A stock must satisfy every criterion to be included in the result.
     *
     * @param stocks Loaded stock universe.
     * @param criteria User-selected filters.
     * @return Matching stocks plus human-readable criterion descriptions.
     */
    static ScreenResult screen(
        const std::vector<Stock> &stocks,
        const std::vector<Criterion> &criteria);
};

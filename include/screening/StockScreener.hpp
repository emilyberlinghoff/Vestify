/**
 * @file StockScreener.hpp
 * @brief Header for reusable multi-criteria stock screening logic.
 * @author Group 13
 */

#pragma once

#include <string>
#include <vector>

#include "core/Stock.hpp"

/**
 * @brief Reusable stock screening engine for filtering loaded fundamentals.
 *
 * Applies all active criteria with AND semantics and returns both the matching
 * stocks and user-facing descriptions of the filters that were applied.
 *
 * @author Group 13
 */
class StockScreener
{
public:
    /**
     * @brief Types of supported screening criteria.
     *
     * Each enumerator maps to one menu-selectable filter supported by the
     * interactive screening flow.
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
     *
     * Stores the criterion type together with either numeric bounds or text
     * data, depending on the type of filter selected.
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
     *
     * Returns both the filtered stock subset and a human-readable summary so
     * the UI can show users exactly which rules were applied.
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

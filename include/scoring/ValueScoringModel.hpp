/**
 * @file ValueScoringModel.hpp
 * @brief Value scoring model implementation.
 * @author Melvin Rogers
 * @author Emily Berlinghoff
 */

#pragma once

#include "scoring/ScoringModel.hpp"

#include <algorithm>
#include <cmath>

/**
 * @brief Scores stocks on value characteristics.
 *
 * Evaluates four value dimensions:
 *   - P/E ratio (lower is better for value investors)
 *   - Dividend yield (higher signals shareholder returns)
 *   - EV/FCF ratio (lower means cheaper relative to cash generation)
 *   - FCF yield (higher means more free cash flow per dollar of market cap)
 *
 * Each component maps to a 0-100 sub-score, then averaged.
 * The model is intended to favor cheaper cash-generating businesses while
 * still rewarding direct shareholder return through dividends.
 *
 * @author Melvin Roger
 * @author Emily Berlinghoff
 */
class ValueScoringModel : public ScoringModel {
public:
    /**
     * @brief Calculate a value score for one stock.
     *
     * Normalizes valuation and cash-generation metrics into sub-scores and
     * averages the valid components into a single comparable ranking value.
     *
     * @param stock Stock fundamentals to evaluate.
     * @return Value score in the range [0, 100].
     */
    double calculateScore(const Stock& stock) const override {
        double total = 0.0;
        int components = 0;

        /// P/E component: lower is better. Map P/E 0-50 -> score 100-0.
        if (std::isfinite(stock.pe_ratio) && stock.pe_ratio > 0.0) {
            total += std::max(0.0, 100.0 - (stock.pe_ratio * 2.0));
            ++components;
        }

        /// Dividend yield: higher is better.
        /// Stored as fraction (0.035 = 3.5%). Map 0-0.08 -> 0-100.
        if (std::isfinite(stock.dividend_yield) && stock.dividend_yield >= 0.0) {
            total += std::min(100.0, stock.dividend_yield * 1250.0);
            ++components;
        }

        /// EV/FCF: lower is better (cheaper valuation).
        /// Map 0-50 -> 100-0.
        if (std::isfinite(stock.ev_to_fcf) && stock.ev_to_fcf > 0.0) {
            total += std::max(0.0, 100.0 - (stock.ev_to_fcf * 2.0));
            ++components;
        }

        /// FCF yield: higher is better (more cash flow per market cap dollar).
        /// Stored as fraction. Map 0-0.10 (10%) -> 0-100.
        if (std::isfinite(stock.fcf_yield) && stock.fcf_yield > 0.0) {
            total += std::min(100.0, stock.fcf_yield * 1000.0);
            ++components;
        }

        return (components > 0) ? (total / components) : 0.0;
    }

    /**
     * @brief Return the display name for this model.
     *
     * This string is used when registering the model with a strategy manager
     * and when showing per-factor score breakdowns to the user.
     *
     * @return The model name string "Value".
     */
    std::string getModelName() const override { return "Value"; }
};

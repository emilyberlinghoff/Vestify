/**
 * @file ValueScoringModel.hpp
 * @brief Value scoring model implementation.
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
 */
class ValueScoringModel : public ScoringModel {
public:
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

    std::string getModelName() const override { return "Value"; }
};
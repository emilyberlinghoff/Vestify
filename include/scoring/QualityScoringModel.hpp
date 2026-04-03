/**
 * @file QualityScoringModel.hpp
 * @brief Quality scoring model implementation.
 * @author Group 13
 */

#pragma once

#include "scoring/ScoringModel.hpp"

#include <algorithm>
#include <cmath>

/**
 * @brief Scores stocks on financial quality and balance sheet health.
 *
 * Evaluates four quality dimensions:
 *   - Current ratio (ability to meet short-term obligations)
 *   - Debt-to-equity (lower leverage is safer)
 *   - Gross margin (pricing power and competitive advantage)
 *   - ROE (management effectiveness, capped to avoid distortion)
 *
 * Each component maps to a 0-100 sub-score, then averaged.
 * This model complements valuation and growth screens by emphasizing durable
 * balance-sheet strength and profitability quality.
 *
 * @author Group 13
 */
class QualityScoringModel : public ScoringModel {
public:
    /**
     * @brief Calculate a quality score for one stock.
     *
     * Converts the stock's liquidity, leverage, margin, and return-on-equity
     * metrics into normalized sub-scores, then averages only the valid inputs
     * to avoid distorting the result with missing data.
     *
     * @param stock Stock fundamentals to evaluate.
     * @return Quality score in the range [0, 100].
     */
    double calculateScore(const Stock& stock) const override {
        double total = 0.0;
        int components = 0;

        /// Current ratio: higher is better (can pay short-term debts).
        /// Map 0-3.0 -> 0-100. Above 3 is maxed out.
        if (std::isfinite(stock.current_ratio) && stock.current_ratio > 0.0) {
            total += std::min(100.0, stock.current_ratio * 33.33);
            ++components;
        }

        /// Debt-to-equity: lower is better (less leveraged).
        /// Map 0-2.0 -> 100-0.
        if (std::isfinite(stock.debt_to_equity) && stock.debt_to_equity >= 0.0) {
            total += std::max(0.0, 100.0 - (stock.debt_to_equity * 50.0));
            ++components;
        }

        /// Gross margin: higher signals pricing power and moat.
        /// Stored as fraction. Map 0-0.80 (80%) -> 0-100.
        if (std::isfinite(stock.gross_margin) && stock.gross_margin > 0.0
            && stock.gross_margin <= 1.0) {
            total += std::min(100.0, stock.gross_margin * 125.0);
            ++components;
        }

        /// ROE: higher is better, but cap at 100% to avoid distorted values.
        /// Stored as fraction. Map 0-0.40 (40%) -> 0-100.
        if (std::isfinite(stock.roe) && stock.roe > 0.0 && stock.roe < 2.0) {
            total += std::min(100.0, stock.roe * 250.0);
            ++components;
        }

        return (components > 0) ? (total / components) : 0.0;
    }

    /**
     * @brief Return the display name for this model.
     *
     * The strategy manager uses this stable string as the lookup key for
     * weights, sub-score breakdowns, and active-strategy reporting.
     *
     * @return The model name string "Quality".
     */
    std::string getModelName() const override { return "Quality"; }
};

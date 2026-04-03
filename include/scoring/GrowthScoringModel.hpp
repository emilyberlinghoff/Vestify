/**
 * @file GrowthScoringModel.hpp
 * @brief Growth scoring model implementation.
 * @author Group 13
 */

#pragma once

#include "scoring/ScoringModel.hpp"

#include <algorithm>
#include <cmath>

/**
 * @brief Scores stocks on growth characteristics.
 *
 * Evaluates four growth dimensions:
 *   - P/E ratio (higher signals market expects future earnings growth)
 *   - Operating margin (higher margin supports scalable growth)
 *   - ROE (high return on equity indicates efficient growth)
 *   - Low dividend yield (earnings reinvested into the business)
 *
 * Each component maps to a 0-100 sub-score, then averaged.
 * It is designed to reward companies whose fundamentals suggest reinvestment
 * capacity and scalable operating performance.
 *
 * @author Group 13
 */
class GrowthScoringModel : public ScoringModel {
public:
    /**
     * @brief Calculate a growth score for one stock.
     *
     * Maps growth-oriented valuation and profitability measures onto the
     * shared 0-100 scoring scale, then averages the components that are
     * available and sensible for the current stock.
     *
     * @param stock Stock fundamentals to evaluate.
     * @return Growth score in the range [0, 100].
     */
    double calculateScore(const Stock& stock) const override {
        double total = 0.0;
        int components = 0;

        /// Higher P/E signals market expects growth. Map P/E 0-50 -> 0-100.
        if (std::isfinite(stock.pe_ratio) && stock.pe_ratio > 0.0) {
            total += std::min(100.0, stock.pe_ratio * 2.0);
            ++components;
        }

        /// Operating margin: higher is better for scalable businesses.
        /// Stored as fraction. Map 0-0.40 (40%) -> 0-100.
        if (std::isfinite(stock.operating_margin) && stock.operating_margin > 0.0) {
            total += std::min(100.0, stock.operating_margin * 250.0);
            ++components;
        }

        /// ROE: higher signals efficient use of equity to generate growth.
        /// Stored as fraction. Map 0-0.40 (40%) -> 0-100.
        if (std::isfinite(stock.roe) && stock.roe > 0.0 && stock.roe < 2.0) {
            total += std::min(100.0, stock.roe * 250.0);
            ++components;
        }

        /// Low dividend yield implies earnings reinvested for growth.
        if (std::isfinite(stock.dividend_yield) && stock.dividend_yield >= 0.0) {
            total += std::max(0.0, 100.0 - (stock.dividend_yield * 1250.0));
            ++components;
        }

        return (components > 0) ? (total / components) : 0.0;
    }

    /**
     * @brief Return the display name for this model.
     *
     * The returned key is used by preset strategies and result views when
     * associating scores with this growth factor model.
     *
     * @return The model name string "Growth".
     */
    std::string getModelName() const override { return "Growth"; }
};

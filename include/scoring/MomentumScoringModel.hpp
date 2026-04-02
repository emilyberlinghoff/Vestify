/**
 * @file MomentumScoringModel.hpp
 * @brief Momentum scoring model implementation.
 */

#pragma once

#include "scoring/ScoringModel.hpp"

#include <algorithm>
#include <cmath>

/**
 * @brief Scores stocks on momentum and profitability trend characteristics.
 *
 * Without full historical return data, this model uses fundamental
 * profitability signals that correlate with price momentum:
 *   - ROA (companies with high asset returns tend to sustain momentum)
 *   - FCF yield (strong cash generation supports continued price gains)
 *   - Net margin (improving profitability drives momentum)
 *
 * Each component maps to a 0-100 sub-score, then averaged.
 *
 * @note When historical price data becomes available, this model should
 *       incorporate actual return-based momentum (e.g., 6-month or 12-month
 *       total return minus the most recent month).
 */
class MomentumScoringModel : public ScoringModel {
public:
    double calculateScore(const Stock& stock) const override {
        double total = 0.0;
        int components = 0;

        /// ROA: higher return on assets correlates with sustained momentum.
        /// Stored as fraction. Map 0-0.25 (25%) -> 0-100.
        if (std::isfinite(stock.roa) && stock.roa > 0.0) {
            total += std::min(100.0, stock.roa * 400.0);
            ++components;
        }

        /// FCF yield: strong cash flow supports price momentum.
        /// Stored as fraction. Map 0-0.10 (10%) -> 0-100.
        if (std::isfinite(stock.fcf_yield) && stock.fcf_yield > 0.0) {
            total += std::min(100.0, stock.fcf_yield * 1000.0);
            ++components;
        }

        /// Net margin: profitability strength as a momentum signal.
        /// Stored as fraction. Map 0-0.30 (30%) -> 0-100.
        if (std::isfinite(stock.net_margin) && stock.net_margin > 0.0) {
            total += std::min(100.0, stock.net_margin * 333.33);
            ++components;
        }

        return (components > 0) ? (total / components) : 0.0;
    }

    std::string getModelName() const override { return "Momentum"; }
};
/**
 * @file EfficiencyScoringModel.hpp
 * @brief Efficiency scoring model implementation.
 */

#pragma once

#include "scoring/ScoringModel.hpp"

#include <algorithm>
#include <cmath>

/**
 * @brief Scores stocks on capital efficiency and operational performance.
 *
 * Evaluates three efficiency dimensions:
 *   - ROA (how well the company uses its assets to generate profit)
 *   - Operating margin (operational efficiency before interest/tax)
 *   - EV/EBIT (market valuation relative to operating earnings)
 *
 * Each component maps to a 0-100 sub-score, then averaged.
 */
class EfficiencyScoringModel : public ScoringModel {
public:
    double calculateScore(const Stock& stock) const override {
        double total = 0.0;
        int components = 0;

        /// ROA: higher means better asset utilization.
        /// Stored as fraction. Map 0-0.25 (25%) -> 0-100.
        if (std::isfinite(stock.roa) && stock.roa > 0.0) {
            total += std::min(100.0, stock.roa * 400.0);
            ++components;
        }

        /// Operating margin: higher means more efficient operations.
        /// Stored as fraction. Map 0-0.40 (40%) -> 0-100.
        if (std::isfinite(stock.operating_margin) && stock.operating_margin > 0.0) {
            total += std::min(100.0, stock.operating_margin * 250.0);
            ++components;
        }

        /// EV/EBIT: lower means cheaper relative to operating earnings.
        /// Map 0-40 -> 100-0.
        if (std::isfinite(stock.ev_to_ebit) && stock.ev_to_ebit > 0.0) {
            total += std::max(0.0, 100.0 - (stock.ev_to_ebit * 2.5));
            ++components;
        }

        return (components > 0) ? (total / components) : 0.0;
    }

    std::string getModelName() const override { return "Efficiency"; }
};
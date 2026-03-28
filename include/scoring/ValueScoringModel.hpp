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
 * Lower P/E ratio and higher dividend yield produce a higher score.
 */
class ValueScoringModel : public ScoringModel {
public:
    double calculateScore(const Stock& stock) const override {
        double score = 0.0;

        /// P/E component: lower is better. Map P/E 0-50 -> score 100-0.
        if (std::isfinite(stock.pe_ratio) && stock.pe_ratio > 0.0) {
            score += std::max(0.0, 100.0 - (stock.pe_ratio * 2.0));
        }

        /// Dividend-yield component: higher is better.
        /// dividend_yield is stored as a fraction (0.035 = 3.5%).
        /// Map 0-0.08 (8%) -> 0-100.
        if (std::isfinite(stock.dividend_yield)) {
            score += std::min(100.0, stock.dividend_yield * 1250.0);
        }

        /// Average of two components -> [0, 100].
        return score / 2.0;
    }

    std::string getModelName() const override { return "Value"; }
};

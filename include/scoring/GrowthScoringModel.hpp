/**
 * @file GrowthScoringModel.hpp
 * @brief Growth scoring model implementation.
 */

#pragma once

#include "scoring/ScoringModel.hpp"

#include <algorithm>
#include <cmath>

/**
 * @brief Scores stocks on growth characteristics.
 *
 * Higher P/E (growth expectations) and lower dividend yield
 * (earnings reinvested) produce a higher score.
 */
class GrowthScoringModel : public ScoringModel {
public:
    double calculateScore(const Stock& stock) const override {
        double score = 0.0;

        /// Higher P/E signals market expects growth. Map P/E 0-50 -> 0-100.
        if (std::isfinite(stock.pe_ratio) && stock.pe_ratio > 0.0) {
            score += std::min(100.0, stock.pe_ratio * 2.0);
        }

        /// Low dividend yield implies more earnings reinvested.
        if (std::isfinite(stock.dividend_yield)) {
            score += std::max(0.0, 100.0 - (stock.dividend_yield * 1250.0));
        }

        return score / 2.0;
    }

    std::string getModelName() const override { return "Growth"; }
};

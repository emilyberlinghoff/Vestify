/**
 * @file MomentumScoringModel.hpp
 * @brief Momentum scoring model implementation.
 */

#pragma once

#include "scoring/ScoringModel.hpp"

#include <algorithm>
#include <cmath>

/**
 * @brief Scores stocks on momentum characteristics.
 *
 * Placeholder implementation: uses price magnitude as a proxy.
 * TODO: Replace with actual momentum calculation (e.g., 6-month return)
 *       once the "Load and Cache Historical Stock Price Data" user story
 *       is implemented.
 */
class MomentumScoringModel : public ScoringModel {
public:
    double calculateScore(const Stock& stock) const override {
        if (!std::isfinite(stock.price)) {
            return 0.0;
        }
        /// log1p(price) * 10 gives a rough 0-100 range for typical stock prices.
        return std::min(100.0, std::log1p(stock.price) * 10.0);
    }

    std::string getModelName() const override { return "Momentum"; }
};

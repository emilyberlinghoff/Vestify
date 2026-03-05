#pragma once

#include <string>

#include "core/Stock.hpp"

/**
 * @brief Abstract base for scoring models.
 *
 * Each concrete model computes a single numeric score for a stock
 * based on its own criteria (value, growth, momentum, etc.).
 */
class ScoringModel {
public:
    virtual ~ScoringModel() = default;

    /**
     * @brief Compute this model's score for the given stock.
     *
     * @param stock Stock to evaluate.
     * @return Score in the range [0, 100].
     */
    virtual double calculateScore(const Stock& stock) const = 0;

    /**
     * @brief Unique name identifying this model (used as a key in weights and results).
     *
     * @return Model name string.
     */
    virtual std::string getModelName() const = 0;
};   
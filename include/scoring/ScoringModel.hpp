/**
 * @file ScoringModel.hpp
 * @brief Abstract interface for individual stock scoring models.
 * @author Melvin Roger
 */

#pragma once

#include <string>

#include "core/Stock.hpp"

/**
 * @brief Abstract base for scoring models.
 *
 * Each concrete model computes a single numeric score for a stock
 * based on its own criteria (value, growth, momentum, etc.).
 * This abstraction allows strategies to combine interchangeable factor models
 * without coupling ranking logic to any one implementation.
 *
 * @author Melvin Rogers
 */
class ScoringModel {
public:
    /**
     * @brief Virtual destructor for polymorphic scoring models.
     *
     * Keeping the destructor virtual ensures derived model objects can be
     * destroyed safely through base pointers.
     */
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
     * Implementations should return a stable identifier so strategies and
     * result breakdowns can refer to the same model consistently.
     *
     * @return Model name string.
     */
    virtual std::string getModelName() const = 0;
};

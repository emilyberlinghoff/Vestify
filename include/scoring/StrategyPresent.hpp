/**
 * @file StrategyPresent.hpp
 * @brief Scoring strategies, results, and ranking logic.
 */

#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <memory>

#include "core/Stock.hpp"
#include "scoring/ScoringModel.hpp"


/// ScoringStrategy — a named preset with a weight per scoring model

/**
 * @brief A named scoring preset mapping model names to weights.
 *
 * Example: "Value" -> { {"Value", 0.60}, {"Growth", 0.20}, {"Momentum", 0.20} }
 */
struct ScoringStrategy {
    /** @brief Display name (e.g., "Value", "Growth"). */
    std::string name;
    /** @brief Short human-readable description. */
    std::string description;
    /** @brief modelName -> weight  (weights should sum to ~1.0). */
    std::unordered_map<std::string, double> weights;

    ScoringStrategy() = default;
    ScoringStrategy(const std::string& name,
                    const std::string& description,
                    const std::unordered_map<std::string, double>& weights)
        : name(name), description(description), weights(weights) {}
};


/// ScoredStock — a Stock paired with its scoring breakdown

/**
 * @brief A stock together with its per-model scores and composite score.
 *
 * Keeps the original Stock struct untouched (no added fields needed).
 */
struct ScoredStock {
    /** @brief The original stock data. */
    Stock stock;
    /** @brief modelName -> sub-score computed by that model. */
    std::unordered_map<std::string, double> subscores;
    /** @brief Weighted composite score. */
    double composite = 0.0;
};


/// StrategyManager — owns presets, active strategy, and ranking logic

/**
 * @brief Manages predefined scoring strategy presets and applies them to stocks.
 *
 * Satisfies the acceptance tests for "Apply Predefined Scoring Strategy Presets":
 *  1. Multiple predefined strategies exist.
 *  2. Selecting a strategy updates the scoring weights.
 *  3. Re-running scoring reflects the new weights.
 *  4. A default strategy is applied on startup (constructor).
 *  5. Switching strategies does not require restarting the application.
 */
class StrategyManager {
public:
    /// ── Construction ──────────────────────────────────────────────────

    /**
     * @brief Construct the manager with built-in presets.
     *
     * Default active strategy is "Value".
     */
    StrategyManager() {
        initPresets();
        active_strategy_name_ = "Value";
    }

    /// ── Model registration ────────────────────────────────────────────

    /**
     * @brief Register a scoring model the manager can use.
     *
     * The caller retains ownership of the pointer (manager does not delete it).
     *
     * @param model Non-null pointer to a ScoringModel.
     */
    void registerModel(ScoringModel* model) {
        if (model) {
            models_[model->getModelName()] = model;
        }
    }

    /// ── Strategy catalogue ────────────────────────────────────────────

    /**
     * @brief Get the names of all available presets (sorted alphabetically).
     *
     * @return Vector of strategy names.
     */
    std::vector<std::string> getAvailableStrategies() const {
        std::vector<std::string> names;
        names.reserve(presets_.size());
        for (const auto& [name, _] : presets_) {
            names.push_back(name);
        }
        std::sort(names.begin(), names.end());
        return names;
    }

    /**
     * @brief Look up a strategy by name.
     *
     * @param name Strategy name.
     * @return Const reference to the strategy.
     * @throws std::out_of_range if not found.
     */
    const ScoringStrategy& getStrategy(const std::string& name) const {
        auto it = presets_.find(name);
        if (it == presets_.end()) {
            throw std::out_of_range("Strategy not found: " + name);
        }
        return it->second;
    }

    /// ── Active strategy management ────────────────────────────────────

    /**
     * @brief Set the active strategy by name.
     *
     * @param name Strategy name.
     * @return True if the strategy exists and was set; false otherwise.
     */
    bool setActiveStrategy(const std::string& name) {
        if (presets_.find(name) == presets_.end()) {
            return false;
        }
        active_strategy_name_ = name;
        return true;
    }

    /**
     * @brief Create or update a custom strategy and activate it.
     *
     * @param name Strategy name.
     * @param description Strategy description.
     * @param weights modelName -> weight.
     * @return True if weights were provided and the strategy was set.
     */
    bool setCustomStrategy(const std::string& name,
                           const std::string& description,
                           const std::unordered_map<std::string, double>& weights) {
        if (weights.empty()) {
            return false;
        }
        presets_[name] = ScoringStrategy(name, description, weights);
        active_strategy_name_ = name;
        return true;
    }

    /**
     * @brief Get the currently active strategy.
     *
     * @return Const reference to the active ScoringStrategy.
     */
    const ScoringStrategy& getActiveStrategy() const {
        return presets_.at(active_strategy_name_);
    }

    /**
     * @brief Get the active strategy's name.
     *
     * @return Strategy name string.
     */
    const std::string& getActiveStrategyName() const {
        return active_strategy_name_;
    }

    /// ── Scoring & ranking ─────────────────────────────────────────────

    /**
     * @brief Score a single stock under the active strategy.
     *
     * @param stock Stock to score.
     * @return ScoredStock with sub-scores and composite.
     */
    ScoredStock scoreStock(const Stock& stock) const {
        const ScoringStrategy& strategy = getActiveStrategy();

        ScoredStock result;
        result.stock = stock;
        double weighted_sum = 0.0;
        double total_weight = 0.0;

        for (const auto& [model_name, weight] : strategy.weights) {
            auto model_it = models_.find(model_name);
            if (model_it == models_.end()) {
                std::cerr << "[StrategyManager] warning: model \""
                          << model_name << "\" not registered, skipping.\n";
                continue;
            }

            double sub_score = model_it->second->calculateScore(stock);
            if (!std::isfinite(sub_score)) {
                sub_score = 0.0;
            }
            sub_score = std::clamp(sub_score, 0.0, 100.0);
            result.subscores[model_name] = sub_score;

            weighted_sum += weight * sub_score;
            total_weight += weight;
        }

        if (total_weight > 0.0) {
            result.composite = weighted_sum / total_weight;
        }
        return result;
    }

    /**
     * @brief Score and rank a list of stocks under the active strategy.
     *
     * @param stocks Input stocks (not modified).
     * @return Vector of ScoredStock sorted descending by composite score.
     */
    std::vector<ScoredStock> rankStocks(const std::vector<Stock>& stocks) const {
        std::vector<ScoredStock> scored;
        scored.reserve(stocks.size());

        for (const auto& s : stocks) {
            scored.push_back(scoreStock(s));
        }

        std::sort(scored.begin(), scored.end(),
                  [](const ScoredStock& a, const ScoredStock& b) {
                      return a.composite > b.composite;
                  });

        return scored;
    }

private:
    /// ── Preset initialization ─────────────────────────────────────────

    void initPresets() {
        presets_["Value"] = ScoringStrategy(
            "Value",
            "Favours undervalued stocks with strong fundamentals (low P/E, high dividend yield).",
            {{"Value", 0.60}, {"Growth", 0.20}, {"Momentum", 0.20}}
        );

        presets_["Growth"] = ScoringStrategy(
            "Growth",
            "Targets companies with high earnings and revenue growth potential.",
            {{"Value", 0.15}, {"Growth", 0.60}, {"Momentum", 0.25}}
        );

        presets_["Momentum"] = ScoringStrategy(
            "Momentum",
            "Emphasises recent price trends and market momentum.",
            {{"Value", 0.10}, {"Growth", 0.20}, {"Momentum", 0.70}}
        );

        presets_["Balanced"] = ScoringStrategy(
            "Balanced",
            "Equal weighting across all scoring dimensions.",
            {{"Value", 0.34}, {"Growth", 0.33}, {"Momentum", 0.33}}
        );
    }

    /// ── Data ──────────────────────────────────────────────────────────

    /** @brief Registered presets. */
    std::unordered_map<std::string, ScoringStrategy> presets_;
    /** @brief Name of the currently active strategy. */
    std::string active_strategy_name_;
    /** @brief Registered scoring models (non-owning). */
    std::unordered_map<std::string, ScoringModel*> models_;
};

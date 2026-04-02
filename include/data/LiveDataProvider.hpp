/**
 * @file LiveDataProvider.hpp
 * @brief Alpha Vantage data provider for historical stock prices.
 */

#pragma once

#include <string>
#include <vector>

#include "core/Stock.hpp"

/**
 * @brief Live data provider that fetches historical prices from Alpha Vantage.
 *
 * Requires an API key provided via the ALPHAVANTAGE_API_KEY environment variable.
 */
class LiveDataProvider {
public:
    /**
     * @brief Single historical price point.
     */
    struct HistoricalPoint {
        std::string date;
        double close = 0.0;
    };

    /**
     * @brief Result of a historical price fetch.
     */
    struct HistoricalResult {
        std::string ticker;
        std::vector<HistoricalPoint> points;
        std::vector<std::string> errors;
        bool from_cache = false;
    };

    /**
     * @brief Construct a provider with a base URL.
     *
     * @param base_url Base URL for Alpha Vantage endpoints.
     */
    explicit LiveDataProvider(std::string base_url = "https://www.alphavantage.co");

    /**
     * @brief Fetch daily historical prices for a ticker (cached to disk).
     *
     * @param ticker Ticker symbol.
     * @param outputsize "compact" or "full".
     * @param adjusted Use adjusted close if available.
     * @param cache_dir Directory for cached CSV files.
     * @return HistoricalResult with points and errors.
     */
    HistoricalResult fetchDailySeries(const std::string& ticker,
                                      const std::string& outputsize = "compact",
                                      bool adjusted = true,
                                      const std::string& cache_dir = "data/historical") const;

private:
    /** @brief Base URL for Alpha Vantage endpoints. */
    std::string base_url_;

    /**
     * @brief Build the Alpha Vantage daily time series URL.
     *
     * @param ticker Ticker symbol.
     * @param api_key Alpha Vantage API key.
     * @param outputsize "compact" or "full".
     * @param adjusted Use adjusted close if available.
     * @return URL for the daily time series endpoint.
     */
    std::string buildDailySeriesUrl(const std::string& ticker,
                                    const std::string& api_key,
                                    const std::string& outputsize,
                                    bool adjusted) const;

    /**
     * @brief URL-encode a string for use in query parameters.
     *
     * @param value Raw value.
     * @return URL-encoded value.
     */
    static std::string urlEncode(const std::string& value);
};

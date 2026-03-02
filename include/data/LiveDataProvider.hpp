#pragma once

#include <string>
#include <vector>

#include "core/Stock.hpp"

/**
 * @brief Live data provider that fetches quotes from Yahoo Finance endpoints.
 *
 * Uses unofficial Yahoo Finance endpoints. These can change or break without notice.
 */
class LiveDataProvider {
public:
    /**
     * @brief Result of a live quote fetch.
     */
    struct QuoteResult {
        /** @brief Parsed stocks. */
        std::vector<Stock> stocks;
        /** @brief Errors encountered during fetch or parse. */
        std::vector<std::string> errors;
    };

    /**
     * @brief Construct a provider with a base URL.
     *
     * @param base_url Base URL for Yahoo endpoints.
     */
    explicit LiveDataProvider(std::string base_url = "https://query1.finance.yahoo.com");

    /**
     * @brief Fetch current quotes for the given tickers.
     *
     * @param tickers List of ticker symbols.
     * @return QuoteResult with stocks and errors.
     */
    QuoteResult fetchQuotes(const std::vector<std::string>& tickers) const;

private:
    /** @brief Base URL for Yahoo endpoints. */
    std::string base_url_;

    /**
     * @brief Attempt to fetch quotes using the given base URL.
     *
     * @param base_url Base URL to use.
     * @param tickers List of ticker symbols.
     * @return QuoteResult with any data or errors.
     */
    QuoteResult fetchWithBaseUrl(const std::string& base_url, const std::vector<std::string>& tickers) const;

    /**
     * @brief Build the Yahoo quote URL.
     *
     * @param tickers List of ticker symbols.
     * @return URL for the quote endpoint.
     */
    std::string buildQuoteUrl(const std::string& base_url, const std::vector<std::string>& tickers) const;

    /**
     * @brief Join tickers into a comma-separated string.
     *
     * @param tickers List of tickers.
     * @return Comma-separated tickers.
     */
    static std::string joinTickers(const std::vector<std::string>& tickers);

    /**
     * @brief URL-encode a string for use in query parameters.
     *
     * @param value Raw value.
     * @return URL-encoded value.
     */
    static std::string urlEncode(const std::string& value);
};

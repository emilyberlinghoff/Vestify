#pragma once

#include <string>
#include <vector>

#include "core/Stock.hpp"

/**
 * @brief Live data provider that fetches quotes from Alpha Vantage.
 *
 * Requires an API key provided via the ALPHAVANTAGE_API_KEY environment variable.
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
     * @param base_url Base URL for Alpha Vantage endpoints.
     */
    explicit LiveDataProvider(std::string base_url = "https://www.alphavantage.co");

    /**
     * @brief Fetch current quotes for the given tickers.
     *
     * @param tickers List of ticker symbols.
     * @return QuoteResult with stocks and errors.
     */
    QuoteResult fetchQuotes(const std::vector<std::string>& tickers) const;

private:
    /** @brief Base URL for Alpha Vantage endpoints. */
    std::string base_url_;

    /**
     * @brief Fetch a single quote from Alpha Vantage.
     *
     * @param ticker Ticker symbol.
     * @param api_key Alpha Vantage API key.
     * @return QuoteResult containing at most one stock.
     */
    QuoteResult fetchQuote(const std::string& ticker, const std::string& api_key) const;

    /**
     * @brief Build the Alpha Vantage Global Quote URL.
     *
     * @param ticker Ticker symbol.
     * @param api_key Alpha Vantage API key.
     * @return URL for the Global Quote endpoint.
     */
    std::string buildGlobalQuoteUrl(const std::string& ticker, const std::string& api_key) const;

    /**
     * @brief Build the Alpha Vantage Company Overview URL.
     *
     * @param ticker Ticker symbol.
     * @param api_key Alpha Vantage API key.
     * @return URL for the Overview endpoint.
     */
    std::string buildOverviewUrl(const std::string& ticker, const std::string& api_key) const;

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

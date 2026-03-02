#include "data/LiveDataProvider.hpp"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <sstream>
#include <stdexcept>

/**
 * @brief libcurl write callback to collect response data.
 *
 * @param contents Response bytes.
 * @param size Size of a byte.
 * @param nmemb Number of bytes.
 * @param userp Pointer to std::string buffer.
 * @return Number of bytes handled.
 */
static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total = size * nmemb;
    auto* buffer = static_cast<std::string*>(userp);
    buffer->append(static_cast<char*>(contents), total);
    return total;
}

/**
 * @brief Fetch a URL with libcurl.
 *
 * @param url URL to fetch.
 * @return Response body.
 */
static std::string httpGet(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Vestify/1.0");

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::string msg = curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        throw std::runtime_error("CURL request failed: " + msg);
    }

    curl_easy_cleanup(curl);
    return response;
}

LiveDataProvider::LiveDataProvider(std::string base_url)
    : base_url_(std::move(base_url)) {}

LiveDataProvider::QuoteResult LiveDataProvider::fetchQuotes(const std::vector<std::string>& tickers) const {
    if (tickers.empty()) {
        QuoteResult result;
        result.errors.push_back("No tickers provided.");
        return result;
    }

    QuoteResult result = fetchWithBaseUrl(base_url_, tickers);
    if (!result.stocks.empty() || result.errors.empty()) {
        return result;
    }

    if (base_url_ != "https://query2.finance.yahoo.com") {
        QuoteResult fallback = fetchWithBaseUrl("https://query2.finance.yahoo.com", tickers);
        result.errors.insert(result.errors.end(), fallback.errors.begin(), fallback.errors.end());
        if (!fallback.stocks.empty()) {
            result.stocks = std::move(fallback.stocks);
        }
    }

    return result;
}

/**
 * @brief Fetch quotes using a specific Yahoo base URL.
 *
 * @param base_url Base URL to use.
 * @param tickers List of ticker symbols.
 * @return QuoteResult with any data or errors.
 */
LiveDataProvider::QuoteResult LiveDataProvider::fetchWithBaseUrl(
    const std::string& base_url,
    const std::vector<std::string>& tickers
) const {
    QuoteResult result;
    const std::string url = buildQuoteUrl(base_url, tickers);

    try {
        const std::string body = httpGet(url);
        const auto json = nlohmann::json::parse(body);

        if (json.contains("finance") && json["finance"].contains("error")) {
            result.errors.push_back("Yahoo Finance error: " + json["finance"]["error"].dump());
            return result;
        }

        if (!json.contains("quoteResponse") || !json["quoteResponse"].contains("result")) {
            result.errors.push_back("Unexpected Yahoo Finance response format.");
            return result;
        }

        for (const auto& item : json["quoteResponse"]["result"]) {
            Stock stock;

            if (item.contains("symbol")) {
                stock.ticker = item["symbol"].get<std::string>();
            }
            if (stock.ticker.empty()) {
                continue;
            }

            if (item.contains("longName")) {
                stock.name = item["longName"].get<std::string>();
            } else if (item.contains("shortName")) {
                stock.name = item["shortName"].get<std::string>();
            }

            if (item.contains("regularMarketPrice")) {
                stock.price = item["regularMarketPrice"].get<double>();
            }
            if (item.contains("marketCap")) {
                stock.market_cap = item["marketCap"].get<double>();
            }
            if (item.contains("trailingPE")) {
                stock.pe_ratio = item["trailingPE"].get<double>();
            }
            if (item.contains("dividendYield")) {
                stock.dividend_yield = item["dividendYield"].get<double>();
            }

            result.stocks.push_back(std::move(stock));
        }
    } catch (const std::exception& ex) {
        result.errors.push_back(ex.what());
    }

    return result;
}

std::string LiveDataProvider::buildQuoteUrl(
    const std::string& base_url,
    const std::vector<std::string>& tickers
) const {
    std::ostringstream url;
    url << base_url << "/v7/finance/quote?symbols=" << urlEncode(joinTickers(tickers));
    return url.str();
}

std::string LiveDataProvider::joinTickers(const std::vector<std::string>& tickers) {
    std::ostringstream out;
    for (size_t i = 0; i < tickers.size(); ++i) {
        if (i > 0) {
            out << ',';
        }
        out << tickers[i];
    }
    return out.str();
}

/**
 * @brief URL-encode a value using libcurl utilities.
 *
 * @param value Raw value.
 * @return URL-encoded value.
 */
std::string LiveDataProvider::urlEncode(const std::string& value) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return value;
    }
    char* encoded = curl_easy_escape(curl, value.c_str(), static_cast<int>(value.size()));
    std::string result = encoded ? encoded : value;
    if (encoded) {
        curl_free(encoded);
    }
    curl_easy_cleanup(curl);
    return result;
}

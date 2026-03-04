#include "data/LiveDataProvider.hpp"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <cstdlib>
#include <sstream>
#include <stdexcept>

/**
 * @brief HTTP response container.
 */
struct HttpResponse {
    /** @brief HTTP status code. */
    long status = 0;
    /** @brief Response body. */
    std::string body;
};

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
 * @brief Fetch a URL with libcurl and capture status/body.
 *
 * @param url URL to fetch.
 * @return HttpResponse with status and body.
 */
static HttpResponse httpGetWithStatus(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(
        curl,
        CURLOPT_USERAGENT,
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 "
        "(KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36"
    );

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::string msg = curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        throw std::runtime_error("CURL request failed: " + msg);
    }

    long status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    curl_easy_cleanup(curl);
    return {status, response};
}

/**
 * @brief Fetch a URL and return body, throwing on error status.
 *
 * @param url URL to fetch.
 * @return HttpResponse with status and body.
 */
static HttpResponse httpGet(const std::string& url) {
    HttpResponse resp = httpGetWithStatus(url);
    if (resp.status >= 400) {
        throw std::runtime_error("HTTP error: " + std::to_string(resp.status));
    }
    return resp;
}

LiveDataProvider::LiveDataProvider(std::string base_url)
    : base_url_(std::move(base_url)) {}

LiveDataProvider::QuoteResult LiveDataProvider::fetchQuotes(const std::vector<std::string>& tickers) const {
    QuoteResult result;
    if (tickers.empty()) {
        result.errors.push_back("No tickers provided.");
        return result;
    }

    const char* key_env = std::getenv("ALPHAVANTAGE_API_KEY");
    if (!key_env || std::string(key_env).empty()) {
        result.errors.push_back("Missing ALPHAVANTAGE_API_KEY environment variable.");
        return result;
    }
    const std::string api_key = key_env;

    for (const auto& ticker : tickers) {
        QuoteResult single = fetchQuote(ticker, api_key);
        result.errors.insert(result.errors.end(), single.errors.begin(), single.errors.end());
        if (!single.stocks.empty()) {
            result.stocks.push_back(single.stocks.front());
        }
    }

    return result;
}

LiveDataProvider::QuoteResult LiveDataProvider::fetchQuote(
    const std::string& ticker,
    const std::string& api_key
) const {
    QuoteResult result;
    if (ticker.empty()) {
        result.errors.push_back("Ticker is empty.");
        return result;
    }

    Stock stock;
    stock.ticker = ticker;

    try {
        const std::string quote_url = buildGlobalQuoteUrl(ticker, api_key);
        const HttpResponse quote_resp = httpGet(quote_url);
        const auto quote_json = nlohmann::json::parse(quote_resp.body);

        if (quote_json.contains("Global Quote")) {
            const auto& q = quote_json["Global Quote"];
            if (q.contains("05. price")) {
                stock.price = std::stod(q["05. price"].get<std::string>());
            }
        } else if (quote_json.contains("Note")) {
            result.errors.push_back("Alpha Vantage rate limit: " + quote_json["Note"].get<std::string>());
        } else if (quote_json.contains("Error Message")) {
            result.errors.push_back("Alpha Vantage error: " + quote_json["Error Message"].get<std::string>());
        }
    } catch (const std::exception& ex) {
        result.errors.push_back(ex.what());
    }

    try {
        const std::string overview_url = buildOverviewUrl(ticker, api_key);
        const HttpResponse overview_resp = httpGet(overview_url);
        const auto overview_json = nlohmann::json::parse(overview_resp.body);

        if (overview_json.contains("Name")) {
            stock.name = overview_json["Name"].get<std::string>();
        }
        if (overview_json.contains("Sector")) {
            stock.sector = overview_json["Sector"].get<std::string>();
        }
        if (overview_json.contains("MarketCapitalization")) {
            stock.market_cap = std::stod(overview_json["MarketCapitalization"].get<std::string>());
        }
        if (overview_json.contains("PERatio")) {
            stock.pe_ratio = std::stod(overview_json["PERatio"].get<std::string>());
        }
        if (overview_json.contains("DividendYield")) {
            stock.dividend_yield = std::stod(overview_json["DividendYield"].get<std::string>());
        }
    } catch (const std::exception& ex) {
        result.errors.push_back(ex.what());
    }

    if (!stock.ticker.empty()) {
        result.stocks.push_back(stock);
    }

    return result;
}

std::string LiveDataProvider::buildGlobalQuoteUrl(
    const std::string& ticker,
    const std::string& api_key
) const {
    std::ostringstream url;
    url << base_url_ << "/query?function=GLOBAL_QUOTE";
    url << "&symbol=" << urlEncode(ticker);
    url << "&apikey=" << urlEncode(api_key);
    return url.str();
}

std::string LiveDataProvider::buildOverviewUrl(
    const std::string& ticker,
    const std::string& api_key
) const {
    std::ostringstream url;
    url << base_url_ << "/query?function=OVERVIEW";
    url << "&symbol=" << urlEncode(ticker);
    url << "&apikey=" << urlEncode(api_key);
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

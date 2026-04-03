/**
 * @file LiveDataProvider.cpp
 * @brief Implementation of the Alpha Vantage historical data provider.
 * @author Group 13
 */

#include "data/LiveDataProvider.hpp"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <cstdlib>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <algorithm>
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

static std::string toUpperCopy(std::string value) {
    for (char& c : value) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return value;
}

static bool loadCachedSeries(const std::string& path,
                             std::vector<LiveDataProvider::HistoricalPoint>& out) {
    std::ifstream infile(path);
    if (!infile.is_open()) {
        return false;
    }

    std::string line;
    if (!std::getline(infile, line)) {
        return false;
    }

    while (std::getline(infile, line)) {
        if (line.empty()) {
            continue;
        }
        std::stringstream ss(line);
        std::string date;
        std::string closeStr;
        if (!std::getline(ss, date, ',')) {
            continue;
        }
        if (!std::getline(ss, closeStr, ',')) {
            continue;
        }
        try {
            double close = std::stod(closeStr);
            out.push_back({date, close});
        } catch (...) {
            continue;
        }
    }

    return !out.empty();
}

static bool saveCachedSeries(const std::string& path,
                             const std::vector<LiveDataProvider::HistoricalPoint>& points) {
    std::ofstream outfile(path);
    if (!outfile.is_open()) {
        return false;
    }

    outfile << "date,close\n";
    for (const auto& point : points) {
        outfile << point.date << "," << point.close << "\n";
    }
    return true;
}

LiveDataProvider::LiveDataProvider(std::string base_url)
    : base_url_(std::move(base_url)) {}

LiveDataProvider::HistoricalResult LiveDataProvider::fetchDailySeries(
    const std::string& ticker,
    const std::string& outputsize,
    bool adjusted,
    const std::string& cache_dir
) const {
    HistoricalResult result;
    if (ticker.empty()) {
        result.errors.push_back("Ticker is empty.");
        return result;
    }

    const char* key_env = std::getenv("ALPHAVANTAGE_API_KEY");
    if (!key_env || std::string(key_env).empty()) {
        result.errors.push_back("Missing ALPHAVANTAGE_API_KEY environment variable.");
        return result;
    }

    const std::string api_key = key_env;
    const std::string symbol = toUpperCopy(ticker);
    result.ticker = symbol;

    std::filesystem::create_directories(cache_dir);
    const std::string cache_path = cache_dir + "/" + symbol + "_daily.csv";
    if (loadCachedSeries(cache_path, result.points)) {
        result.from_cache = true;
        return result;
    }

    auto parseSeries = [&](const nlohmann::json& json, bool useAdjusted) -> bool {
        bool isInfo = json.contains("Information");
        if (json.contains("Note")) {
            result.errors.push_back("Alpha Vantage rate limit: " + json["Note"].get<std::string>());
            return false;
        }
        if (isInfo) {
            result.errors.push_back("Alpha Vantage info: " + json["Information"].get<std::string>());
            return false;
        }
        if (json.contains("Error Message")) {
            result.errors.push_back("Alpha Vantage error: " + json["Error Message"].get<std::string>());
            return false;
        }

        if (!json.contains("Time Series (Daily)")) {
            return false;
        }

        const auto& series = json["Time Series (Daily)"];
        for (auto it = series.begin(); it != series.end(); ++it) {
            const std::string date = it.key();
            const auto& entry = it.value();
            std::string key = "4. close";
            if (useAdjusted && entry.contains("5. adjusted close")) {
                key = "5. adjusted close";
            }
            if (!entry.contains(key)) {
                continue;
            }
            try {
                double close = std::stod(entry[key].get<std::string>());
                result.points.push_back({date, close});
            } catch (...) {
                continue;
            }
        }
        return !result.points.empty();
    };

    try {
        const std::string url = buildDailySeriesUrl(symbol, api_key, outputsize, adjusted);
        const HttpResponse resp = httpGet(url);
        const auto json = nlohmann::json::parse(resp.body);

        bool ok = parseSeries(json, adjusted);
        if (!ok && adjusted) {
            /// Fallback to unadjusted if adjusted data is unavailable.
            const std::string fallbackUrl = buildDailySeriesUrl(symbol, api_key, outputsize, false);
            const HttpResponse fallbackResp = httpGet(fallbackUrl);
            const auto fallbackJson = nlohmann::json::parse(fallbackResp.body);
            ok = parseSeries(fallbackJson, false);
            if (ok) {
                result.errors.clear();
            }
        }

        if (!ok) {
            if (result.errors.empty()) {
                result.errors.push_back("Missing Time Series (Daily) data for " + symbol);
            }
            return result;
        }

        std::sort(result.points.begin(), result.points.end(),
                  [](const HistoricalPoint& a, const HistoricalPoint& b) {
                      return a.date < b.date;
                  });

        saveCachedSeries(cache_path, result.points);
    } catch (const std::exception& ex) {
        result.errors.push_back(ex.what());
    }

    return result;
}

std::string LiveDataProvider::buildDailySeriesUrl(
    const std::string& ticker,
    const std::string& api_key,
    const std::string& outputsize,
    bool adjusted
) const {
    std::ostringstream url;
    url << base_url_ << "/query?function=";
    url << (adjusted ? "TIME_SERIES_DAILY_ADJUSTED" : "TIME_SERIES_DAILY");
    url << "&symbol=" << urlEncode(ticker);
    url << "&outputsize=" << urlEncode(outputsize);
    url << "&apikey=" << urlEncode(api_key);
    return url.str();
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

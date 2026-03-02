#pragma once

#include <algorithm>
#include <cctype>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/Stock.hpp"

/**
 * @brief CSV loader for stock datasets.
 */
class CSVLoader {
public:
    /**
     * @brief Options for CSV parsing.
     */
    struct Options {
        /** @brief Field delimiter. */
        char delimiter = ',';
        /** @brief Whether the CSV includes a header row. */
        bool has_header = true;
    };

    /**
     * @brief Result of a CSV load operation.
     */
    struct LoadResult {
        /** @brief Parsed stocks. */
        std::vector<Stock> stocks;
        /** @brief Human-readable error messages for skipped rows or failures. */
        std::vector<std::string> errors;
    };

    /**
     * @brief Construct a CSV loader.
     *
     * @param options Parsing options.
     */
    /** @brief Construct a CSV loader with default options. */
    CSVLoader() = default;

    /**
     * @brief Construct a CSV loader with custom options.
     *
     * @param options Parsing options.
     */
    explicit CSVLoader(Options options) : options_(options) {}

    /**
     * @brief Load stocks from a CSV file on disk.
     *
     * @param path Path to the CSV file.
     * @return LoadResult containing parsed stocks and any errors.
     */
    LoadResult loadFile(const std::string& path) const {
        std::ifstream file(path);
        if (!file.is_open()) {
            LoadResult result;
            result.errors.push_back("Failed to open CSV file: " + path);
            return result;
        }
        return loadStream(file);
    }

    /**
     * @brief Load stocks from an input stream.
     *
     * @param in Input stream.
     * @return LoadResult containing parsed stocks and any errors.
     */
    LoadResult loadStream(std::istream& in) const {
        LoadResult result;

        std::string line;
        std::unordered_map<std::string, size_t> header_index;

        if (options_.has_header && std::getline(in, line)) {
            auto header_fields = parseCsvLine(line);
            for (size_t i = 0; i < header_fields.size(); ++i) {
                header_index[normalizeHeader(header_fields[i])] = i;
            }
        }

        size_t row = 1;
        while (std::getline(in, line)) {
            ++row;
            if (line.empty()) {
                continue;
            }

            auto fields = parseCsvLine(line);
            auto stock_opt = parseStock(fields, header_index);
            if (!stock_opt.has_value()) {
                result.errors.push_back("Row " + std::to_string(row) + ": malformed or missing required fields; skipped.");
                continue;
            }

            result.stocks.push_back(*stock_opt);
        }

        return result;
    }

private:
    /** @brief Parsing options. */
    Options options_;

    /**
     * @brief Parse a single CSV line into fields.
     *
     * @param line Input line.
     * @return Vector of fields.
     */
    std::vector<std::string> parseCsvLine(const std::string& line) const {
        std::vector<std::string> fields;
        std::string field;
        bool in_quotes = false;

        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];
            if (c == '"') {
                if (in_quotes && i + 1 < line.size() && line[i + 1] == '"') {
                    field.push_back('"');
                    ++i;
                } else {
                    in_quotes = !in_quotes;
                }
            } else if (c == options_.delimiter && !in_quotes) {
                fields.push_back(trim(field));
                field.clear();
            } else {
                field.push_back(c);
            }
        }

        fields.push_back(trim(field));
        return fields;
    }

    /**
     * @brief Normalize header names for matching.
     *
     * @param header Raw header string.
     * @return Normalized header.
     */
    std::string normalizeHeader(const std::string& header) const {
        std::string out;
        out.reserve(header.size());
        for (char c : header) {
            if (std::isalnum(static_cast<unsigned char>(c))) {
                out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
            }
        }
        return out;
    }

    /**
     * @brief Trim leading and trailing whitespace.
     *
     * @param value Raw string.
     * @return Trimmed string.
     */
    std::string trim(const std::string& value) const {
        size_t start = 0;
        while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start]))) {
            ++start;
        }
        size_t end = value.size();
        while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
            --end;
        }
        return value.substr(start, end - start);
    }

    /**
     * @brief Parse a stock from CSV fields.
     *
     * @param fields Fields from the CSV row.
     * @param header_index Map of normalized header name to index.
     * @return Optional stock if the row is valid.
     */
    std::optional<Stock> parseStock(
        const std::vector<std::string>& fields,
        const std::unordered_map<std::string, size_t>& header_index
    ) const {
        Stock stock;

        auto getField = [&](const std::vector<std::string>& keys, size_t fallback_index) -> std::optional<std::string> {
            for (const auto& key : keys) {
                auto it = header_index.find(key);
                if (it != header_index.end() && it->second < fields.size()) {
                    return fields[it->second];
                }
            }
            if (!header_index.empty()) {
                return std::nullopt;
            }
            if (fallback_index < fields.size()) {
                return fields[fallback_index];
            }
            return std::nullopt;
        };

        auto ticker = getField({"ticker", "symbol"}, 0);
        auto name = getField({"name", "longname", "shortname"}, 1);
        auto sector = getField({"sector"}, 2);
        auto price = getField({"price", "regularmarketprice"}, 3);
        auto market_cap = getField({"marketcap"}, 4);
        auto pe_ratio = getField({"peratio", "trailingpe"}, 5);
        auto dividend_yield = getField({"dividendyield"}, 6);

        if (!ticker.has_value() || ticker->empty()) {
            return std::nullopt;
        }

        stock.ticker = *ticker;
        stock.name = name.value_or("");
        stock.sector = sector.value_or("");

        if (!parseDouble(price, stock.price)) {
            return std::nullopt;
        }
        parseDouble(market_cap, stock.market_cap);
        parseDouble(pe_ratio, stock.pe_ratio);
        parseDouble(dividend_yield, stock.dividend_yield);

        return stock;
    }

    /**
     * @brief Parse a string into a double.
     *
     * @param value String value.
     * @param out Parsed double.
     * @return True if parsed successfully.
     */
    bool parseDouble(const std::optional<std::string>& value, double& out) const {
        if (!value.has_value() || value->empty()) {
            return false;
        }
        try {
            out = std::stod(*value);
        } catch (const std::exception&) {
            return false;
        }
        return true;
    }
};

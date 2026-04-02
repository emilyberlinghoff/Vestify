/**
 * @file WatchlistRepo.cpp
 * @brief Implementation of JSON-backed watchlist loading and saving.
 */

#include "persistence/WatchListRepo.hpp"

#include <fstream>
#include <exception>
#include <nlohmann/json.hpp>

/**
 * @brief Load all watchlists from the configured JSON file.
 *
 * @return LoadResult containing parsed watchlists and any errors.
 */
WatchListRepo::LoadResult WatchListRepo::loadAll() const
{
    LoadResult res;

    std::ifstream infile(filepath_);
    if (!infile.is_open())
    {
        res.errors.push_back("Could not open file for reading: " + filepath_);
        return res;
    }

    try
    {
        nlohmann::json j;
        infile >> j;

        // Support old format:
        // {
        //   "name": "Tech",
        //   "tickers": ["AAPL", "MSFT"]
        // }
        if (j.is_object())
        {
            if (!j.contains("name") || !j["name"].is_string())
            {
                res.errors.push_back("Missing or invalid watchlist name.");
                return res;
            }

            WatchList wl(j["name"].get<std::string>());

            if (j.contains("tickers") && j["tickers"].is_array())
            {
                for (const auto& ticker : j["tickers"])
                {
                    if (ticker.is_string())
                    {
                        wl.add(ticker.get<std::string>());
                    }
                    else
                    {
                        res.errors.push_back("Encountered non-string ticker in single watchlist.");
                    }
                }
            }
            else
            {
                res.errors.push_back("Missing or invalid tickers array in single watchlist.");
            }

            res.watchlists.push_back(wl);
            return res;
        }

        // Support new format:
        // [
        //   {"name":"Tech","tickers":["AAPL","MSFT"]},
        //   {"name":"Banks","tickers":["JPM","BAC"]}
        // ]
        if (j.is_array())
        {
            for (const auto& item : j)
            {
                if (!item.is_object())
                {
                    res.errors.push_back("Encountered non-object entry in watchlists array.");
                    continue;
                }

                if (!item.contains("name") || !item["name"].is_string())
                {
                    res.errors.push_back("A watchlist is missing a valid name.");
                    continue;
                }

                WatchList wl(item["name"].get<std::string>());

                if (item.contains("tickers") && item["tickers"].is_array())
                {
                    for (const auto& ticker : item["tickers"])
                    {
                        if (ticker.is_string())
                        {
                            wl.add(ticker.get<std::string>());
                        }
                        else
                        {
                            res.errors.push_back(
                                "Encountered non-string ticker in watchlist: " + wl.getName());
                        }
                    }
                }
                else
                {
                    res.errors.push_back(
                        "Missing or invalid tickers array in watchlist: " + wl.getName());
                }

                res.watchlists.push_back(wl);
            }

            return res;
        }

        res.errors.push_back("Invalid JSON format in watchlist file.");
    }
    catch (const std::exception& ex)
    {
        res.errors.push_back(std::string("Failed to parse JSON: ") + ex.what());
    }

    return res;
}

/**
 * @brief Save all watchlists using the array-based JSON format.
 *
 * @param watchlists Watchlists to persist.
 * @return SaveResult describing success or failure.
 */
WatchListRepo::SaveResult WatchListRepo::saveAll(const std::vector<WatchList>& watchlists) const
{
    SaveResult res;

    nlohmann::json root = nlohmann::json::array();

    for (const auto& watchlist : watchlists)
    {
        nlohmann::json item;
        item["name"] = watchlist.getName();
        item["tickers"] = watchlist.getAll();
        root.push_back(item);
    }

    std::ofstream outfile(filepath_);
    if (!outfile.is_open())
    {
        res.errMsg = "Cant open file for writing: " + filepath_;
        return res;
    }

    try
    {
        outfile << root.dump(2);
        res.ok = true;
    }
    catch (const std::exception& ex)
    {
        res.errMsg = std::string("Write failed: ") + ex.what();
    }

    return res;
}

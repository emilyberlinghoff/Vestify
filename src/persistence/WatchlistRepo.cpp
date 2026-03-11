#include "persistence/WatchListRepo.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

/**
 * @brief Load the watchlist from the JSON file.
 *
 * @return LoadResult with watchlist and any errors.
 */
WatchListRepo::LoadResult WatchListRepo::load() const
{
    LoadResult out;

    std::ifstream infile(filepath_);
    if (!infile.is_open())
    {
        return out;
    }

    nlohmann::json j;

    try
    {
        infile >> j;
    }
    catch (const nlohmann::json::parse_error &ex)
    {
        out.errors.push_back(
            "Could not parse watchlist (" + filepath_ + "): " + ex.what());
        return out;
    }

    if (j.contains("name") && j["name"].is_string())
    {
        out.watchlist.setName(j["name"].get<std::string>());
    }

    if (!j.contains("tickers") || !j["tickers"].is_array())
    {
        out.errors.push_back("Watchlist json missing tickers array: " + filepath_);
        return out;
    }

    for (const auto &entry : j["tickers"])
    {
        if (entry.is_string())
        {
            out.watchlist.add(entry.get<std::string>());
        }
        else
        {
            out.errors.push_back("Skipped non-string entry in watchlist file");
        }
    }

    return out;
}

/**
 * @brief Write the watchlist to the JSON file.
 *
 * @param wl Watchlist to save.
 * @return SaveResult indicating success or failure.
 */
WatchListRepo::SaveResult WatchListRepo::save(const WatchList &wl) const
{
    SaveResult res;

    nlohmann::json j;

    j["name"] = wl.getName();
    j["tickers"] = wl.getAll();

    std::ofstream outfile(filepath_);

    if (!outfile.is_open())
    {
        res.errMsg = "Cant open file for writing: " + filepath_;
        return res;
    }

    try
    {
        outfile << j.dump(2);
        res.ok = true;
    }
    catch (const std::exception &ex)
    {
        res.errMsg = std::string("Write failed: ") + ex.what();
    }

    return res;
}

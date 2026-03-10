#include "include/core/Watchlist.hpp"

#include <algorithm>
#include <cctype>

Watchlist::Watchlist(const std::string& name)
    : name_(name)
{
}

bool Watchlist::add(const std::string& sym)
{
    std::string normalized = normalizeSymbol(sym);

    if (has(normalized))
    {
        return false;
    }

    symbols_.push_back(normalized);
    return true;
}

bool Watchlist::remove(const std::string& sym)
{
    std::string normalized = normalizeSymbol(sym);

    auto pos = std::find(symbols_.begin(), symbols_.end(), normalized);
    if (pos == symbols_.end())
    {
        return false;
    }

    symbols_.erase(pos);
    return true;
}

bool Watchlist::has(const std::string& sym) const
{
    std::string normalized = normalizeSymbol(sym);
    return std::find(symbols_.begin(), symbols_.end(), normalized) != symbols_.end();
}

const std::vector<std::string>& Watchlist::getAll() const
{
    return symbols_;
}

size_t Watchlist::size() const
{
    return symbols_.size();
}

bool Watchlist::empty() const
{
    return symbols_.empty();
}

void Watchlist::clear()
{
    symbols_.clear();
}

const std::string& Watchlist::getName() const
{
    return name_;
}

void Watchlist::setName(const std::string& name)
{
    name_ = name;
}

std::string Watchlist::normalizeSymbol(const std::string& sym)
{
    std::string result = sym;

    for (char& c : result)
    {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }

    return result;
}
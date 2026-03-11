#include "core/Watchlist.hpp"

#include <algorithm>
#include <cctype>

WatchList::WatchList(const std::string &name)
    : name_(name)
{
}

bool WatchList::add(const std::string &sym)
{
    std::string normalized = normalizeSymbol(sym);

    if (has(normalized))
    {
        return false;
    }

    symbols_.push_back(normalized);
    return true;
}

bool WatchList::remove(const std::string &sym)
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

bool WatchList::has(const std::string &sym) const
{
    std::string normalized = normalizeSymbol(sym);
    return std::find(symbols_.begin(), symbols_.end(), normalized) != symbols_.end();
}

const std::vector<std::string> &WatchList::getAll() const
{
    return symbols_;
}

size_t WatchList::size() const
{
    return symbols_.size();
}

bool WatchList::empty() const
{
    return symbols_.empty();
}

void WatchList::clear()
{
    symbols_.clear();
}

const std::string &WatchList::getName() const
{
    return name_;
}

void WatchList::setName(const std::string &name)
{
    name_ = name;
}

std::string WatchList::normalizeSymbol(const std::string &sym)
{
    std::string result = sym;

    for (char &c : result)
    {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }

    return result;
}
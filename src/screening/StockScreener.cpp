#include "screening/StockScreener.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>

namespace
{
std::string toUpperCopy(std::string value)
{
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char c)
        {
            return static_cast<char>(std::toupper(c));
        });
    return value;
}

bool isStrictlyPositive(double value)
{
    return value > 1e-9;
}

std::string formatDouble(double value, int precision = 2)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

bool matchesCriterion(const Stock &stock, const StockScreener::Criterion &criterion)
{
    switch (criterion.type)
    {
    case StockScreener::CriterionType::SectorEquals:
        return toUpperCopy(stock.sector) == toUpperCopy(criterion.textValue);
    case StockScreener::CriterionType::PriceBetween:
        return stock.price >= criterion.minValue && stock.price <= criterion.maxValue;
    case StockScreener::CriterionType::MaxPeRatio:
        return isStrictlyPositive(stock.pe_ratio) && stock.pe_ratio <= criterion.maxValue;
    case StockScreener::CriterionType::MaxPbRatio:
        return isStrictlyPositive(stock.pb_ratio) && stock.pb_ratio <= criterion.maxValue;
    case StockScreener::CriterionType::MaxPsRatio:
        return isStrictlyPositive(stock.ps_ratio) && stock.ps_ratio <= criterion.maxValue;
    case StockScreener::CriterionType::MaxEvToFcf:
        return isStrictlyPositive(stock.ev_to_fcf) && stock.ev_to_fcf <= criterion.maxValue;
    case StockScreener::CriterionType::MinFcfYield:
        return stock.fcf_yield >= criterion.minValue;
    case StockScreener::CriterionType::MinRoe:
        return stock.roe >= criterion.minValue;
    case StockScreener::CriterionType::MinOperatingMargin:
        return stock.operating_margin >= criterion.minValue;
    case StockScreener::CriterionType::MaxDebtToEquity:
        return stock.debt_to_equity <= criterion.maxValue;
    case StockScreener::CriterionType::MinCurrentRatio:
        return stock.current_ratio >= criterion.minValue;
    case StockScreener::CriterionType::MinDividendYield:
        return stock.dividend_yield >= criterion.minValue;
    }

    return false;
}

std::string describeCriterion(const StockScreener::Criterion &criterion)
{
    switch (criterion.type)
    {
    case StockScreener::CriterionType::SectorEquals:
        return "Sector = " + criterion.textValue;
    case StockScreener::CriterionType::PriceBetween:
        return "Price between $" + formatDouble(criterion.minValue) +
               " and $" + formatDouble(criterion.maxValue);
    case StockScreener::CriterionType::MaxPeRatio:
        return "P/E <= " + formatDouble(criterion.maxValue);
    case StockScreener::CriterionType::MaxPbRatio:
        return "P/B <= " + formatDouble(criterion.maxValue);
    case StockScreener::CriterionType::MaxPsRatio:
        return "P/S <= " + formatDouble(criterion.maxValue);
    case StockScreener::CriterionType::MaxEvToFcf:
        return "EV/FCF <= " + formatDouble(criterion.maxValue);
    case StockScreener::CriterionType::MinFcfYield:
        return "FCF yield >= " + formatDouble(criterion.minValue * 100.0) + "%";
    case StockScreener::CriterionType::MinRoe:
        return "ROE >= " + formatDouble(criterion.minValue * 100.0) + "%";
    case StockScreener::CriterionType::MinOperatingMargin:
        return "Operating margin >= " + formatDouble(criterion.minValue * 100.0) + "%";
    case StockScreener::CriterionType::MaxDebtToEquity:
        return "Debt-to-equity <= " + formatDouble(criterion.maxValue);
    case StockScreener::CriterionType::MinCurrentRatio:
        return "Current ratio >= " + formatDouble(criterion.minValue);
    case StockScreener::CriterionType::MinDividendYield:
        return "Dividend yield >= " + formatDouble(criterion.minValue * 100.0) + "%";
    }

    return "Unknown criterion";
}
} // namespace

StockScreener::ScreenResult StockScreener::screen(
    const std::vector<Stock> &stocks,
    const std::vector<Criterion> &criteria)
{
    ScreenResult result;
    result.appliedCriteria.reserve(criteria.size());

    for (const auto &criterion : criteria)
    {
        result.appliedCriteria.push_back(describeCriterion(criterion));
    }

    for (const auto &stock : stocks)
    {
        bool matchesAll = true;
        for (const auto &criterion : criteria)
        {
            if (!matchesCriterion(stock, criterion))
            {
                matchesAll = false;
                break;
            }
        }

        if (matchesAll)
        {
            result.matches.push_back(stock);
        }
    }

    std::sort(
        result.matches.begin(),
        result.matches.end(),
        [](const Stock &lhs, const Stock &rhs)
        {
            return lhs.ticker < rhs.ticker;
        });

    return result;
}

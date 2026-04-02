#include "ui/ScreeningMenu.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

#include "screening/StockScreener.hpp"
#include "ui/StockPrinter.hpp"

namespace
{
std::vector<std::string> collectSectors(const std::vector<Stock> &stocks)
{
    std::vector<std::string> sectors;
    sectors.reserve(stocks.size());

    for (const auto &stock : stocks)
    {
        if (!stock.sector.empty())
        {
            sectors.push_back(stock.sector);
        }
    }

    std::sort(sectors.begin(), sectors.end());
    sectors.erase(std::unique(sectors.begin(), sectors.end()), sectors.end());
    return sectors;
}

void printCriteriaSummary(const std::vector<StockScreener::Criterion> &criteria)
{
    auto preview = StockScreener::screen({}, criteria);

    std::cout << "\nCurrent filters:\n";
    if (preview.appliedCriteria.empty())
    {
        std::cout << "None selected.\n";
        return;
    }

    for (std::size_t i = 0; i < preview.appliedCriteria.size(); ++i)
    {
        std::cout << i + 1 << ". " << preview.appliedCriteria[i] << "\n";
    }
}
} // namespace

void ScreeningMenu::run(
    const std::vector<Stock> &stocks,
    const std::function<int()> &readInt,
    const std::function<std::string(const std::string &)> &readLine)
{
    if (stocks.empty())
    {
        std::cout << "No stock data loaded.\n";
        return;
    }

    std::vector<StockScreener::Criterion> criteria;
    bool addingCriteria = true;

    while (addingCriteria)
    {
        std::cout << "\nStock Screening Criteria\n";
        std::cout << "1. Sector equals\n";
        std::cout << "2. Price range\n";
        std::cout << "3. Max P/E ratio\n";
        std::cout << "4. Max P/B ratio\n";
        std::cout << "5. Max P/S ratio\n";
        std::cout << "6. Max EV/FCF\n";
        std::cout << "7. Min FCF yield (%)\n";
        std::cout << "8. Min ROE (%)\n";
        std::cout << "9. Min operating margin (%)\n";
        std::cout << "10. Max debt-to-equity\n";
        std::cout << "11. Min current ratio\n";
        std::cout << "12. Min dividend yield (%)\n";
        std::cout << "13. Print current filters\n";
        std::cout << "14. Run screen\n";
        std::cout << "15. Cancel\n";
        std::cout << "> ";

        const int selection = readInt();

        auto readPositiveDouble = [&](const std::string &prompt, double &value) -> bool
        {
            std::string input = readLine(prompt);
            std::stringstream stream(input);
            if (!(stream >> value))
            {
                std::cout << "Invalid number.\n";
                return false;
            }
            return true;
        };

        switch (selection)
        {
        case 1:
        {
            const auto sectors = collectSectors(stocks);
            if (sectors.empty())
            {
                std::cout << "No sector data is available in the loaded stocks.\n";
                break;
            }

            std::cout << "\nAvailable sectors:\n";
            for (std::size_t i = 0; i < sectors.size(); ++i)
            {
                std::cout << i + 1 << ". " << sectors[i] << "\n";
            }
            std::cout << "Select sector number: ";
            const int sectorChoice = readInt();
            if (sectorChoice < 1 || sectorChoice > static_cast<int>(sectors.size()))
            {
                std::cout << "Invalid sector selection.\n";
                break;
            }

            const std::string &sector = sectors[sectorChoice - 1];
            criteria.push_back({StockScreener::CriterionType::SectorEquals, 0.0, 0.0, sector});
            std::cout << "Added sector filter: " << sector << "\n";
            break;
        }
        case 2:
        {
            double minPrice = 0.0;
            double maxPrice = 0.0;
            if (!readPositiveDouble("Minimum price: ", minPrice) ||
                !readPositiveDouble("Maximum price: ", maxPrice))
            {
                break;
            }
            if (minPrice < 0.0 || maxPrice < 0.0 || minPrice > maxPrice)
            {
                std::cout << "Invalid price range.\n";
                break;
            }
            criteria.push_back({StockScreener::CriterionType::PriceBetween, minPrice, maxPrice, ""});
            std::cout << "Added price range filter.\n";
            break;
        }
        case 3:
        case 4:
        case 5:
        case 6:
        case 10:
        case 11:
        {
            double value = 0.0;
            std::string prompt;
            StockScreener::CriterionType type = StockScreener::CriterionType::MaxPeRatio;

            if (selection == 3)
            {
                prompt = "Maximum P/E ratio: ";
                type = StockScreener::CriterionType::MaxPeRatio;
            }
            else if (selection == 4)
            {
                prompt = "Maximum P/B ratio: ";
                type = StockScreener::CriterionType::MaxPbRatio;
            }
            else if (selection == 5)
            {
                prompt = "Maximum P/S ratio: ";
                type = StockScreener::CriterionType::MaxPsRatio;
            }
            else if (selection == 6)
            {
                prompt = "Maximum EV/FCF: ";
                type = StockScreener::CriterionType::MaxEvToFcf;
            }
            else if (selection == 10)
            {
                prompt = "Maximum debt-to-equity: ";
                type = StockScreener::CriterionType::MaxDebtToEquity;
            }
            else
            {
                prompt = "Minimum current ratio: ";
                type = StockScreener::CriterionType::MinCurrentRatio;
            }

            if (!readPositiveDouble(prompt, value))
            {
                break;
            }

            if (value < 0.0)
            {
                std::cout << "Value cannot be negative.\n";
                break;
            }

            if (type == StockScreener::CriterionType::MinCurrentRatio)
            {
                criteria.push_back({type, value, 0.0, ""});
            }
            else
            {
                criteria.push_back({type, 0.0, value, ""});
            }

            std::cout << "Added screening filter.\n";
            break;
        }
        case 7:
        case 8:
        case 9:
        case 12:
        {
            double percent = 0.0;
            std::string prompt;
            StockScreener::CriterionType type = StockScreener::CriterionType::MinFcfYield;

            if (selection == 7)
            {
                prompt = "Minimum FCF yield (%): ";
                type = StockScreener::CriterionType::MinFcfYield;
            }
            else if (selection == 8)
            {
                prompt = "Minimum ROE (%): ";
                type = StockScreener::CriterionType::MinRoe;
            }
            else if (selection == 9)
            {
                prompt = "Minimum operating margin (%): ";
                type = StockScreener::CriterionType::MinOperatingMargin;
            }
            else
            {
                prompt = "Minimum dividend yield (%): ";
                type = StockScreener::CriterionType::MinDividendYield;
            }

            if (!readPositiveDouble(prompt, percent))
            {
                break;
            }
            criteria.push_back({type, percent / 100.0, 0.0, ""});
            std::cout << "Added screening filter.\n";
            break;
        }
        case 13:
            printCriteriaSummary(criteria);
            break;
        case 14:
            addingCriteria = false;
            break;
        case 15:
            std::cout << "Screening cancelled.\n";
            return;
        default:
            std::cout << "Invalid option.\n";
            break;
        }
    }

    auto result = StockScreener::screen(stocks, criteria);

    std::cout << "\nApplied criteria:\n";
    if (result.appliedCriteria.empty())
    {
        std::cout << "None. Showing all loaded stocks.\n";
    }
    else
    {
        for (const auto &description : result.appliedCriteria)
        {
            std::cout << "- " << description << "\n";
        }
    }

    std::cout << "\nFound " << result.matches.size() << " matching stocks.\n";
    if (result.matches.empty())
    {
        return;
    }

    StockPrinter::printAvailableStocks(result.matches);

    std::string detailChoice = readLine("Print full details for the screened stocks? (y/n): ");
    if (!detailChoice.empty() && (detailChoice[0] == 'y' || detailChoice[0] == 'Y'))
    {
        StockPrinter::printStocks(result.matches);
    }
}

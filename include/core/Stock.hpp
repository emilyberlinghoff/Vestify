/**
 * @file Stock.hpp
 * @brief Core data structure for stock financial information.
 *
 * This header defines the Stock struct which encapsulates comprehensive
 * financial data for individual stocks, including market data, financial
 * statements, ratios, and valuation metrics used throughout the Vestify
 * stock analysis application.
 *
 * @author Group 13
 */

#pragma once

#include <string>

/**
 * @brief Comprehensive financial data structure for individual stocks.
 *
 * The Stock struct contains all financial information needed for stock analysis,
 * organized into logical categories including identity, market data, income
 * statement, cash flow, balance sheet, ratios, and valuation metrics.
 * All monetary values are in USD, and ratios are expressed as decimals
 * (e.g., 0.15 for 15%).
 */
struct Stock
{
    /** @brief Stock ticker symbol (e.g., "AAPL", "GOOGL"). */
    std::string ticker;
    /** @brief Full company name. */
    std::string name;
    /** @brief Industry sector classification. */
    std::string sector;

    /** @brief Current stock price per share in USD. */
    double price = 0.0;
    /** @brief Total shares outstanding. */
    double shares_outstanding = 0.0;
    /** @brief Market capitalization (price × shares_outstanding) in USD. */
    double market_cap = 0.0;
    /** @brief Annual dividend yield as a decimal (e.g., 0.025 for 2.5%). */
    double dividend_yield = 0.0;

    /** @brief Total revenue for the period in USD. */
    double revenue = 0.0;
    /** @brief Gross profit (revenue - cost of goods sold) in USD. */
    double gross_profit = 0.0;
    /** @brief Operating income (EBIT) in USD. */
    double operating_income = 0.0;
    /** @brief Net income attributable to shareholders in USD. */
    double net_income = 0.0;

    /** @brief Operating cash flow in USD. */
    double operating_cash_flow = 0.0;
    /** @brief Capital expenditures (negative value) in USD. */
    double capital_expenditures = 0.0;
    /** @brief Free cash flow (operating cash flow - capex) in USD. */
    double free_cash_flow = 0.0;

    /** @brief Total debt obligations in USD. */
    double total_debt = 0.0;
    /** @brief Cash and cash equivalents in USD. */
    double cash = 0.0;
    /** @brief Total shareholders' equity in USD. */
    double total_equity = 0.0;
    /** @brief Total assets in USD. */
    double total_assets = 0.0;
    /** @brief Current assets (due within 1 year) in USD. */
    double current_assets = 0.0;
    /** @brief Current liabilities (due within 1 year) in USD. */
    double current_liabilities = 0.0;

    /** @brief Gross margin (gross_profit / revenue) as decimal. */
    double gross_margin = 0.0;
    /** @brief Operating margin (operating_income / revenue) as decimal. */
    double operating_margin = 0.0;
    /** @brief Net margin (net_income / revenue) as decimal. */
    double net_margin = 0.0;
    /** @brief Current ratio (current_assets / current_liabilities). */
    double current_ratio = 0.0;
    /** @brief Debt-to-equity ratio (total_debt / total_equity). */
    double debt_to_equity = 0.0;

    /** @brief Book value per share (total_equity / shares_outstanding). */
    double book_value_per_share = 0.0;
    /** @brief Revenue per share (revenue / shares_outstanding). */
    double revenue_per_share = 0.0;
    /** @brief Earnings per share (net_income / shares_outstanding). */
    double eps = 0.0;
    /** @brief Free cash flow per share (free_cash_flow / shares_outstanding). */
    double fcf_per_share = 0.0;

    /** @brief Price-to-earnings ratio (price / eps). */
    double pe_ratio = 0.0;
    /** @brief Price-to-book ratio (price / book_value_per_share). */
    double pb_ratio = 0.0;
    /** @brief Price-to-sales ratio (price / revenue_per_share). */
    double ps_ratio = 0.0;
    /** @brief Free cash flow yield (free_cash_flow / market_cap) as decimal. */
    double fcf_yield = 0.0;

    /** @brief Enterprise value (market_cap + total_debt - cash) in USD. */
    double enterprise_value = 0.0;
    /** @brief EV-to-EBIT ratio (enterprise_value / operating_income). */
    double ev_to_ebit = 0.0;
    /** @brief EV-to-sales ratio (enterprise_value / revenue). */
    double ev_to_sales = 0.0;
    /** @brief EV-to-FCF ratio (enterprise_value / free_cash_flow). */
    double ev_to_fcf = 0.0;

    /** @brief Return on equity (net_income / total_equity) as decimal. */
    double roe = 0.0;
    /** @brief Return on assets (net_income / total_assets) as decimal. */
    double roa = 0.0;
};

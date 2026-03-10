#pragma once

#include <string>

struct Stock {
    // Identity
    std::string ticker;
    std::string name;
    std::string sector;

    // Market
    double price = 0.0;
    double shares_outstanding = 0.0;
    double market_cap = 0.0;
    double dividend_yield = 0.0;

    // Income statement
    double revenue = 0.0;
    double gross_profit = 0.0;
    double operating_income = 0.0;
    double net_income = 0.0;

    // Cash flow
    double operating_cash_flow = 0.0;
    double capital_expenditures = 0.0;
    double free_cash_flow = 0.0;

    // Balance sheet
    double total_debt = 0.0;
    double cash = 0.0;
    double total_equity = 0.0;
    double total_assets = 0.0;
    double current_assets = 0.0;
    double current_liabilities = 0.0;

    // Margins / ratios
    double gross_margin = 0.0;
    double operating_margin = 0.0;
    double net_margin = 0.0;
    double current_ratio = 0.0;
    double debt_to_equity = 0.0;

    // Per-share
    double book_value_per_share = 0.0;
    double revenue_per_share = 0.0;
    double eps = 0.0;
    double fcf_per_share = 0.0;

    // Valuation
    double pe_ratio = 0.0;
    double pb_ratio = 0.0;
    double ps_ratio = 0.0;
    double fcf_yield = 0.0;

    // Enterprise value
    double enterprise_value = 0.0;
    double ev_to_ebit = 0.0;
    double ev_to_sales = 0.0;
    double ev_to_fcf = 0.0;

    // Profitability
    double roe = 0.0;
    double roa = 0.0;
};
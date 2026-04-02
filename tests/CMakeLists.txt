add_executable(scoring_tests
    test_scoring.cpp
)

target_include_directories(scoring_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_test(NAME scoring_tests COMMAND scoring_tests)

add_executable(watchlist_tests
    test_watchlist.cpp
    ../src/persistence/WatchlistRepo.cpp
    ../src/core/Watchlist.cpp
)

target_include_directories(watchlist_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(watchlist_tests PRIVATE nlohmann_json::nlohmann_json)

add_test(NAME watchlist_tests COMMAND watchlist_tests)

add_executable(backtest_tests
    test_backtest.cpp
    ../src/backtest/BacktestEngine.cpp
)

target_include_directories(backtest_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_test(NAME backtest_tests COMMAND backtest_tests)

add_executable(csv_load_tests
    test_csv_load.cpp
    ../src/core/StockRepository.cpp
)

target_include_directories(csv_load_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_test(NAME csv_load_tests COMMAND csv_load_tests)

add_executable(stock_screener_tests
    test_stock_screener.cpp
    ../src/screening/StockScreener.cpp
)

target_include_directories(stock_screener_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_test(NAME stock_screener_tests COMMAND stock_screener_tests)

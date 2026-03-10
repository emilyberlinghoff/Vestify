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
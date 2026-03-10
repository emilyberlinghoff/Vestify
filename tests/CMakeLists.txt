include(FetchContent)

add_executable(csv_loader_tests
    test_csv_loader.cpp
    ../src/core/StockRepository.cpp
)

target_include_directories(csv_loader_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_test(NAME csv_loader_tests COMMAND csv_loader_tests)

# Scoring strategy preset tests 
add_executable(scoring_tests
    test_scoring.cpp
)

target_include_directories(scoring_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_test(NAME scoring_tests COMMAND scoring_tests)

#  Watchlist persistence tests 
add_executable(watchlist_tests
    test_watchlist.cpp
    ../src/persistence/WatchlistRepo.cpp
)

target_include_directories(watchlist_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# nlohmann/json is needed for the persistence layer
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

target_link_libraries(watchlist_tests PRIVATE nlohmann_json::nlohmann_json)

add_test(NAME watchlist_tests COMMAND watchlist_tests)

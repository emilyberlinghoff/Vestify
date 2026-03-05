add_executable(csv_loader_tests
    test_csv_loader.cpp
    ../src/core/StockRepository.cpp
)

target_include_directories(csv_loader_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_test(NAME csv_loader_tests COMMAND csv_loader_tests)

# ── Scoring strategy preset tests ──────────────────────────────────────
add_executable(scoring_tests
    test_scoring.cpp
)

target_include_directories(scoring_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_test(NAME scoring_tests COMMAND scoring_tests)

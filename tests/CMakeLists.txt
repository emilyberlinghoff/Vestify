add_executable(scoring_tests
    test_scoring.cpp
)

target_include_directories(scoring_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_test(NAME scoring_tests COMMAND scoring_tests)
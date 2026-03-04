add_executable(csv_loader_tests
    test_csv_loader.cpp
)

target_include_directories(csv_loader_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_test(NAME csv_loader_tests COMMAND csv_loader_tests)

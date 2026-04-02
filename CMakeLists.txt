cmake_minimum_required(VERSION 3.16)

project(Vestify LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VESTIFY_BUILD_TESTS "Build Vestify tests" ON)
option(VESTIFY_ENABLE_LIVE "Enable live Alpha Vantage data provider" ON)

add_executable(vestify
    src/main.cpp
    src/core/StockRepository.cpp
    src/core/WatchList.cpp
    src/backtest/BacktestEngine.cpp
    src/data/LiveDataProvider.cpp
    src/persistence/WatchListRepo.cpp
    src/screening/StockScreener.cpp
    src/ui/InteractiveMenu.cpp
    src/ui/ScreeningMenu.cpp
    src/ui/StockPrinter.cpp
)

target_include_directories(vestify
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (VESTIFY_ENABLE_LIVE)
    find_package(CURL REQUIRED)
    find_package(nlohmann_json REQUIRED)

    target_compile_definitions(vestify PRIVATE VESTIFY_ENABLE_LIVE=1)
    target_link_libraries(vestify PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    target_compile_options(vestify PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (VESTIFY_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

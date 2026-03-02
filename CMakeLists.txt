cmake_minimum_required(VERSION 3.16)

project(Vestify LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VESTIFY_BUILD_TESTS "Build Vestify tests" ON)

add_executable(vestify
    src/main.cpp
)

target_include_directories(vestify
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Reasonable warnings for most Unix-like toolchains
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    target_compile_options(vestify PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (VESTIFY_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

cmake_minimum_required(VERSION 3.20)
project(QtCiDemo VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Test)

# ── Main Application ──────────────────────────────────────────────
add_executable(QtCiDemo
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/Calculator.cpp
    src/Calculator.h
)

target_link_libraries(QtCiDemo PRIVATE Qt6::Widgets)

# ── Unit Tests ────────────────────────────────────────────────────
add_executable(UnitTests
    tests/test_calculator.cpp
    src/Calculator.cpp
    src/Calculator.h
)

target_link_libraries(UnitTests PRIVATE Qt6::Test)
target_include_directories(UnitTests PRIVATE src)

enable_testing()
add_test(NAME UnitTests COMMAND UnitTests)

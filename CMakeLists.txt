cmake_minimum_required(VERSION 3.20)
project(QtCiDemo VERSION 2.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)   # for the Qt GUI adapter + the one remaining QtTest target

find_package(Qt6 REQUIRED COMPONENTS Widgets Test)

# ── GoogleTest (pinned, reproducible) ─────────────────────────────
# Fetched at configure time. URL_HASH pins the exact tarball so the build is
# reproducible (same discipline as pinning the Docker base / Qt version).
include(FetchContent)
FetchContent_Declare(googletest
    URL      https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
    URL_HASH SHA256=7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926
)
# Keep GoogleTest from overriding parent project settings (MSVC CRT etc.)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ── Domain core (NO Qt — enforced by linking no Qt) ───────────────
add_library(core STATIC
    src/core/Calculator.cpp
    src/core/CalculatorService.cpp
)
target_include_directories(core PUBLIC src)

# ── GUI driving adapter ───────────────────────────────────────────
add_library(adapters_gui STATIC src/adapters/gui/MainWindow.cpp)
target_link_libraries(adapters_gui PUBLIC core Qt6::Widgets)

# ── Application (composition root) ────────────────────────────────
add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE adapters_gui)

# ── Testing ───────────────────────────────────────────────────────
enable_testing()
include(GoogleTest)

# Unit + headless integration: GoogleTest / GoogleMock. NO Qt — these compile
# and run with just a C++ compiler (portable to a bare or cross toolchain).
add_executable(test_calculator tests/unit/test_calculator.cpp)
target_link_libraries(test_calculator PRIVATE core GTest::gtest_main)
gtest_discover_tests(test_calculator PROPERTIES LABELS "unit")

add_executable(test_calculator_service tests/unit/test_calculator_service.cpp)
target_include_directories(test_calculator_service PRIVATE tests)   # for mocks/
target_link_libraries(test_calculator_service PRIVATE core GTest::gtest_main GTest::gmock)
gtest_discover_tests(test_calculator_service PROPERTIES LABELS "unit")

add_executable(test_service_log tests/integration/test_service_log.cpp)
target_link_libraries(test_service_log PRIVATE core GTest::gtest_main)
gtest_discover_tests(test_service_log PROPERTIES LABELS "integration")

# GUI integration: the ONLY place Qt Test remains, because it needs the Qt
# event loop, QTest input simulation, and the offscreen platform.
add_executable(test_gui_service tests/integration/test_gui_service.cpp)
target_link_libraries(test_gui_service PRIVATE adapters_gui Qt6::Test Qt6::Widgets)
add_test(NAME test_gui_service COMMAND test_gui_service)
set_tests_properties(test_gui_service PROPERTIES
    LABELS "integration"
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

cmake_minimum_required(VERSION 3.14)
project(SoloLevelingAriseTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Include paths
set(PROJECT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../ImGui DirectX 11 Kiero Hook")
set(MOCK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mocks")

# Test: PatternScanner
add_executable(test_pattern_scanner
    unit/test_pattern_scanner.cpp
)
target_include_directories(test_pattern_scanner PRIVATE
    ${MOCK_DIR}
    ${PROJECT_SRC_DIR}
    ${PROJECT_SRC_DIR}/include
)
target_compile_definitions(test_pattern_scanner PRIVATE UNIT_TESTING)
target_link_libraries(test_pattern_scanner GTest::gtest_main)

# Test: GameHooks (feature flag logic)
add_executable(test_game_hooks
    unit/test_game_hooks.cpp
)
target_include_directories(test_game_hooks PRIVATE
    ${MOCK_DIR}
    ${PROJECT_SRC_DIR}
    ${PROJECT_SRC_DIR}/include
)
target_compile_definitions(test_game_hooks PRIVATE UNIT_TESTING)
target_link_libraries(test_game_hooks GTest::gtest_main)

# Test: Offsets
add_executable(test_offsets
    unit/test_offsets.cpp
)
target_include_directories(test_offsets PRIVATE
    ${MOCK_DIR}
    ${PROJECT_SRC_DIR}
    ${PROJECT_SRC_DIR}/include
)
target_compile_definitions(test_offsets PRIVATE UNIT_TESTING)
target_link_libraries(test_offsets GTest::gtest_main)

# Test: MinHookWrapper
add_executable(test_minhook_wrapper
    unit/test_minhook_wrapper.cpp
)
target_include_directories(test_minhook_wrapper PRIVATE
    ${MOCK_DIR}
    ${PROJECT_SRC_DIR}
    ${PROJECT_SRC_DIR}/include
)
target_compile_definitions(test_minhook_wrapper PRIVATE UNIT_TESTING)
target_link_libraries(test_minhook_wrapper GTest::gtest_main)

# Test: Enums
add_executable(test_enums
    unit/test_enums.cpp
)
target_include_directories(test_enums PRIVATE
    ${MOCK_DIR}
    ${PROJECT_SRC_DIR}
    ${PROJECT_SRC_DIR}/include
)
target_compile_definitions(test_enums PRIVATE UNIT_TESTING)
target_link_libraries(test_enums GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_pattern_scanner)
gtest_discover_tests(test_game_hooks)
gtest_discover_tests(test_offsets)
gtest_discover_tests(test_minhook_wrapper)
gtest_discover_tests(test_enums)

cmake_minimum_required(VERSION 3.28)
project(Twirl LANGUAGES CXX)

enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Get dependencies
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)
FetchContent_Declare(
    EnTT
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.10.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(EnTT)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(nlohmann_json)
find_package(GTest REQUIRED)

# Include directories
include_directories(include)
include_directories(${GTEST_INCLUDE_DIRS})

# Build and link main files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/**/*.cpp")
add_executable(twirl ${SOURCES})
target_link_libraries(
    twirl
    PRIVATE
    sfml-graphics
    EnTT::EnTT
    nlohmann_json::nlohmann_json
)

# Build and link test files
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp" "src/*.cpp" "tests/**/*.cpp" "src/**/*.cpp")
list(REMOVE_ITEM TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
add_executable(twirl_tests ${TEST_SOURCES})
target_link_libraries(
    twirl_tests
    PRIVATE
    ${GTEST_LIBRARIES}
    sfml-graphics
    EnTT::EnTT
    nlohmann_json::nlohmann_json
)
target_link_libraries(
    twirl_tests
    PUBLIC
)

target_compile_features(twirl PRIVATE cxx_std_17)

# Add tests
add_test(NAME twirl_tests COMMAND twirl_tests)

# Trigger this to treat empty components as regular components.
# add_definitions(-DENTT_NO_ETO)

if(WIN32)
    add_custom_command(
        TARGET twirl
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:main>
        VERBATIM)
endif()

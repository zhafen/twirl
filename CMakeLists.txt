cmake_minimum_required(VERSION 3.28)
project(Twirl LANGUAGES CXX)

enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

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

# Collect all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(twirl ${SOURCES})
target_link_libraries(
    twirl
    PRIVATE
    sfml-graphics
    EnTT::EnTT
    nlohmann_json::nlohmann_json
)

# file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
add_executable(twirl_tests tests/test_main.cpp tests/test_shape.cpp src/shape.cpp)
target_link_libraries(
    twirl_tests
    ${GTEST_LIBRARIES}
    pthread
    sfml-graphics
    EnTT::EnTT
    nlohmann_json::nlohmann_json
)

target_compile_features(twirl PRIVATE cxx_std_17)

# Add tests
add_test(NAME twirl_tests COMMAND twirl_tests)

add_definitions(-DENTT_NO_ETO)

if(WIN32)
    add_custom_command(
        TARGET twirl
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:main>
        VERBATIM)
endif()

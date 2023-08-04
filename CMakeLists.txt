cmake_minimum_required(VERSION 3.24)
project(UraniumTL VERSION 0.1.0
    LANGUAGES CXX C
)

option(UN_BUILD_SAMPLES OFF)
option(UN_BUILD_TESTS OFF)

enable_testing()
set(UN_PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
list(PREPEND CMAKE_MODULE_PATH "${UN_PROJECT_ROOT}/cmake")

include(Config)
include(ThirdParty/get_cpm)
include(ThirdParty/dragonbox)

if (UN_BUILD_TESTS)
    include(ThirdParty/gtest)
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(UraniumTL)

if (UN_BUILD_SAMPLES)
    add_subdirectory(Samples)
endif ()

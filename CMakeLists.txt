cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(trading_common)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
set(TRADING_COMMON_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include")
if (CMAKE_DEBUG)
    message(STATUS "trading_common CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}")
endif ()

include(FetchContent)
include(cmake/nlohmann_json.cmake)
include(cmake/simple_logger.cmake)

add_library(trading_common STATIC
        src/ohlc.cpp include/trading_common/ohlc.h

)
target_include_directories(trading_common
        PRIVATE
        ${TRADING_COMMON_INCLUDE}
        ${NLOHMANN_JSON_INCLUDE}
)
target_link_libraries(trading_common
        PRIVATE
)

install(TARGETS trading_common DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

option(NO_TRADING_COMMON_TESTS "trading_common Disable Current tests" OFF)

if (NOT NO_TRADING_COMMON_TESTS)
    include(FetchContent)
    FetchContent_Declare(Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.4.0
    )
    FETCHCONTENT_GETPROPERTIES(Catch2)
    FETCHCONTENT_MAKEAVAILABLE(Catch2)

    add_subdirectory(test)
endif ()
include(FetchContent)
FetchContent_Declare(nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)
find_path(JSON_INCLUDE_DIR
        NAMES nlohmann/json.hpp
        PATHS ${nlohmann_json_SOURCE_DIR}/single_include
        REQUIRED
)
set(NLOHMANN_JSON_SOURCE_DIR ${nlohmann_json_SOURCE_DIR} CACHE INTERNAL "")
set(NLOHMANN_JSON_INCLUDE ${NLOHMANN_JSON_SOURCE_DIR}/single_include CACHE INTERNAL "")

if (CMAKE_DEBUG)
    message(STATUS "common/cmake NLOHMANN_JSON_INCLUDE ${NLOHMANN_JSON_INCLUDE}")
endif ()

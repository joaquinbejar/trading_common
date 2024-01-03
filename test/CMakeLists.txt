
add_executable(test_ohlc test_ohlc.cpp)
target_include_directories(test_ohlc
        PRIVATE
        ${COMMON_INCLUDE}
        ${NLOHMANN_JSON_INCLUDE}
)

target_link_libraries(test_ohlc PRIVATE Catch2::Catch2WithMain)
target_link_libraries(test_ohlc PRIVATE
        common
)



add_executable(test_ohlc test_ohlc.cpp)
target_include_directories(test_ohlc
        PRIVATE
        ${SIMPLE_COLOR_INCLUDE}
        ${COMMON_INCLUDE}
        ${TRADING_COMMON_INCLUDE}
        ${NLOHMANN_JSON_INCLUDE}
)

target_link_libraries(test_ohlc PRIVATE Catch2::Catch2WithMain)
target_link_libraries(test_ohlc PRIVATE
        trading_common
        common
)

add_executable(test_order test_order.cpp)
target_include_directories(test_order
        PRIVATE
        ${SIMPLE_COLOR_INCLUDE}
        ${COMMON_INCLUDE}
        ${TRADING_COMMON_INCLUDE}
        ${NLOHMANN_JSON_INCLUDE}
)

target_link_libraries(test_order PRIVATE Catch2::Catch2WithMain)
target_link_libraries(test_order PRIVATE
        trading_common
        common
)

add_executable(test_position test_position.cpp)
target_include_directories(test_position
        PRIVATE
        ${SIMPLE_COLOR_INCLUDE}
        ${COMMON_INCLUDE}
        ${TRADING_COMMON_INCLUDE}
        ${NLOHMANN_JSON_INCLUDE}
)

target_link_libraries(test_position PRIVATE Catch2::Catch2WithMain)
target_link_libraries(test_position PRIVATE
        trading_common
        common
)
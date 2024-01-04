//
// Created by Joaquin Bejar Garcia on 19/10/23.
//


#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "trading_common/ohlc.h"
#include "nlohmann/json.hpp"

using namespace trading::common;
using json = nlohmann::json;

TEST_CASE("OHLC Default Constructor", "[OHLC]") {
    OHLC ohlc;

    REQUIRE(ohlc.open == 0);
    REQUIRE(ohlc.high == 0);
    REQUIRE(ohlc.low == 0);
    REQUIRE(ohlc.close == 0);
}


TEST_CASE("OHLC JSON Constructor", "[OHLC]") {
    json j = {{"open",  1.0},
              {"high",  2.0},
              {"low",   0.5},
              {"close", 1.5}};
    OHLC ohlc(j);

    REQUIRE(ohlc.open == 1.0);
    REQUIRE(ohlc.high == 2.0);
    REQUIRE(ohlc.low == 0.5);
    REQUIRE(ohlc.close == 1.5);
}

TEST_CASE("OHLC Parameterized Constructor", "[OHLC]") {
    OHLC ohlc(1.0, 2.0, 0.5, 1.5);

    REQUIRE(ohlc.open == 1.0);
    REQUIRE(ohlc.high == 2.0);
    REQUIRE(ohlc.low == 0.5);
    REQUIRE(ohlc.close == 1.5);
}

TEST_CASE("OHLC to_json Method", "[OHLC]") {
    OHLC ohlc(1.0, 2.0, 0.5, 1.5);
    json j = ohlc.to_json();

    REQUIRE(j["open"] == 1.0);
    REQUIRE(j["high"] == 2.0);
    REQUIRE(j["low"] == 0.5);
    REQUIRE(j["close"] == 1.5);
}

TEST_CASE("HeikinAshi Constructor with Two OHLCs", "[HeikinAshi]") {
    OHLC previous(1.0, 2.0, 0.5, 1.5);
    OHLC current(1.5, 2.5, 1.0, 2.0);
    HeikinAshi ha(current, previous);
    REQUIRE_THAT(ha.open, Catch::Matchers::WithinRel(1.25, 0.001));
    REQUIRE_THAT(ha.open, Catch::Matchers::WithinAbs(1.25, 0.000001));
    REQUIRE_THAT(ha.close, Catch::Matchers::WithinRel(1.75, 0.001));
    REQUIRE_THAT(ha.close, Catch::Matchers::WithinAbs(1.75, 0.000001));
    REQUIRE_THAT(ha.high, Catch::Matchers::WithinRel(2.5, 0.001));
    REQUIRE_THAT(ha.high, Catch::Matchers::WithinAbs(2.5, 0.000001));
    REQUIRE_THAT(ha.low, Catch::Matchers::WithinRel(1.0, 0.001));
    REQUIRE_THAT(ha.low, Catch::Matchers::WithinAbs(1.0, 0.000001));
}

TEST_CASE("HeikinAshi Constructor with One OHLC", "[HeikinAshi]") {
    OHLC current(1.5, 2.5, 1.0, 2.0);
    HeikinAshi ha(current);
    REQUIRE_THAT(ha.open, Catch::Matchers::WithinRel(1.5, 0.001));
    REQUIRE_THAT(ha.open, Catch::Matchers::WithinAbs(1.5, 0.000001));
    REQUIRE_THAT(ha.close, Catch::Matchers::WithinRel(1.75, 0.001));
    REQUIRE_THAT(ha.close, Catch::Matchers::WithinAbs(1.75, 0.000001));
    REQUIRE_THAT(ha.high, Catch::Matchers::WithinRel(2.5, 0.001));
    REQUIRE_THAT(ha.high, Catch::Matchers::WithinAbs(2.5, 0.000001));
    REQUIRE_THAT(ha.low, Catch::Matchers::WithinRel(1.0, 0.001));
    REQUIRE_THAT(ha.low, Catch::Matchers::WithinAbs(1.0, 0.000001));
}


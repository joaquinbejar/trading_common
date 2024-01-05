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

TEST_CASE("Test Timestamp creation", "[Timestamp]") {
    SECTION("Test default constructor") {
        Timestamp t;
        REQUIRE(t.timestamp == 0);
    }

    SECTION("Test constructor with timestamp") {
        timestamp_t time = 1626173400; // Tiempo Unix para 2021-07-13 13:30:00 UTC
        Timestamp t(time);
        REQUIRE(t.timestamp == time);
    }

    SECTION("Test constructor with valid json") {
        nlohmann::json j = { {"timestamp", 1626173400} };
        Timestamp t(j);
        REQUIRE(t.timestamp == 1626173400);
    }

    SECTION("Test constructor with invalid json") {
        nlohmann::json j = { {"time", 1626173400} };  // Se usa una clave incorrecta
        REQUIRE_THROWS_AS(Timestamp(j), OHLCException);
    }
}

TEST_CASE("Test Timestamp conversion to json", "[Timestamp]") {
    SECTION("Default Timestamp") {
        Timestamp t;
        nlohmann::json j = t.to_json();
        REQUIRE(j["timestamp"] == 0);
    }

    SECTION("Timestamp with specific time") {
        timestamp_t time = 1626173400; // Tiempo Unix para 2021-07-13 13:30:00 UTC
        Timestamp t(time);
        nlohmann::json j = t.to_json();
        REQUIRE(j["timestamp"] == time);
    }
}

TEST_CASE( "OHLCV class is tested", "[OHLCV]" ) {

    // Testing default constructor
    SECTION( "Default Constructor" ) {
        OHLCV ohlcv;
        REQUIRE( ohlcv.volume == 0);
    }

        // Testing the parameterized constructor
    SECTION( "Parameterized Constructor" ) {
        OHLCV ohlcv(1000, 1.5, 2.5, 0.5, 1.0, 100);
        REQUIRE(ohlcv.timestamp == 1000);
        REQUIRE(ohlcv.open == 1.5);
        REQUIRE(ohlcv.high == 2.5);
        REQUIRE(ohlcv.low == 0.5);
        REQUIRE(ohlcv.close == 1.0);
        REQUIRE(ohlcv.volume == 100);
    }

        // Testing to_json function
    SECTION( "to_json function" ) {
        OHLCV ohlcv(1000, 1.5, 2.5, 0.5, 1.0, 100);
        json j = ohlcv.to_json();

        REQUIRE(j["timestamp"] == ohlcv.timestamp);
        REQUIRE(j["open"] == ohlcv.open);
        REQUIRE(j["high"] == ohlcv.high);
        REQUIRE(j["low"] == ohlcv.low);
        REQUIRE(j["close"] == ohlcv.close);
        REQUIRE(j["volume"] == ohlcv.volume);
    }
}
//
// Created by Joaquin Bejar Garcia on 19/10/23.
//


#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "trading_common/ohlc.h"
#include "nlohmann/json.hpp"
#include <iostream>

using namespace trading::common;
using json = nlohmann::json;

TEST_CASE("Symbol", "[Symbol]") {

    SECTION("Empty constructor") {
        Symbol sym;

        REQUIRE(sym.to_json()["symbol"].get<std::string>() == "");
    }

    SECTION("Nonempty constructor") {
        std::shared_ptr<std::string> str = std::make_shared<std::string>("test symbol");
        Symbol sym(str);

        REQUIRE(sym.to_json()["symbol"].get<std::string>() == "test symbol");
    }

    SECTION("Json constructor") {
        nlohmann::json j = {
                {"symbol", "test symbol from json"}
        };

        Symbol sym(j);

        REQUIRE(sym.to_json()["symbol"].get<std::string>() == "test symbol from json");
    }

    SECTION("Json constructor exception") {
        nlohmann::json j = {
                {"bad_key", "test symbol from json"}
        };

        REQUIRE_THROWS_AS(Symbol(j), OHLCException);
    }

    SECTION("To json") {
        std::shared_ptr<std::string> str = std::make_shared<std::string>("test symbol to json");
        Symbol sym(str);

        REQUIRE(sym.to_json().dump() == R"({"symbol":"test symbol to json"})");
    }

}


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
        nlohmann::json j = {{"timestamp", 1626173400}};
        Timestamp t(j);
        REQUIRE(t.timestamp == 1626173400);
    }

    SECTION("Test constructor with invalid json") {
        nlohmann::json j = {{"time", 1626173400}};  // Se usa una clave incorrecta
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

TEST_CASE("OHLCV class is tested", "[OHLCV]") {

    // Testing default constructor
    SECTION("Default Constructor") {
        OHLCV ohlcv;
        REQUIRE(ohlcv.volume == 0);
    }

        // Testing the parameterized constructor
    SECTION("Parameterized Constructor") {
        symbol_t symbol = std::make_shared<std::string>("AAPL");
        OHLCV ohlcv(symbol, 1000, 1.5, 2.5, 0.5, 1.0, 100);
        REQUIRE(ohlcv.timestamp == 1000);
        REQUIRE(ohlcv.open == 1.5);
        REQUIRE(ohlcv.high == 2.5);
        REQUIRE(ohlcv.low == 0.5);
        REQUIRE(ohlcv.close == 1.0);
        REQUIRE(ohlcv.volume == 100);
    }

        // Testing to_json function
    SECTION("to_json function") {
        symbol_t symbol = std::make_shared<std::string>("AAPL");
        OHLCV ohlcv(symbol, 1000, 1.5, 2.5, 0.5, 1.0, 100);
        json j = ohlcv.to_json();

        REQUIRE(j["timestamp"] == ohlcv.timestamp);
        REQUIRE(j["open"] == ohlcv.open);
        REQUIRE(j["high"] == ohlcv.high);
        REQUIRE(j["low"] == ohlcv.low);
        REQUIRE(j["close"] == ohlcv.close);
        REQUIRE(j["volume"] == ohlcv.volume);
    }
}


TEST_CASE("SeriesOHLCV Construction", "[SeriesOHLCV]") {
    SECTION("Default Constructor") {
        SeriesOHLCV series;
        REQUIRE(series.empty());
        REQUIRE(series.size() == 0);
    }

    SECTION("Constructor from JSON") {
        // Asume un JSON de ejemplo
        json j = R"([{"timestamp": 123456789, "open": 1.0, "high": 2.0, "low": 0.5, "close": 1.5, "volume": 100}])"_json;
        SeriesOHLCV series(j);
        REQUIRE_FALSE(series.empty());
        REQUIRE(series.size() == 1);
    }
}

TEST_CASE("SeriesOHLCV Insertion and Access", "[SeriesOHLCV]") {
    SeriesOHLCV series;
    symbol_t symbol = std::make_shared<std::string>("AAPL");
    OHLCV ohlcv1(symbol, 1704495600, 1.5, 2.5, 0.5, 1.0, 100);
    OHLCV ohlcv2(symbol, 1704525353, 2.5, 3.5, 1.5, 2.0, 200);

    SECTION("Inserting OHLCV") {
        REQUIRE(series.insert(ohlcv1));
        REQUIRE(series.size() == 1);
    }

    SECTION("Accessing OHLCV by timestamp") {
        series.insert(ohlcv1);
        auto &retrievedOHLCV = series[ohlcv1.timestamp];
        // Aquí agregar verificaciones específicas para ohlc1
    }

    SECTION("Accessing OHLCV by date") {
        series.insert(ohlcv1);
        auto &retrievedOHLCV = series["2024-01-06"]; // Asumiendo que ohlc2 tiene esta fecha
        REQUIRE(retrievedOHLCV.timestamp == ohlcv1.timestamp);
    }

    SECTION("Inserting OHLCV with same timestamp") {
        series.insert(ohlcv1);
        REQUIRE(series.insert(ohlcv1));
        REQUIRE(series.size() == 1);
    }
}

TEST_CASE("SeriesOHLCV to_json Functionality", "[SeriesOHLCV]") {
    SeriesOHLCV series;
    symbol_t symbol = std::make_shared<std::string>("AAPL");
    OHLCV ohlcv1(symbol, 1704495600, 1.5, 2.5, 0.5, 1.0, 100);
    series.insert(ohlcv1);

    SECTION("to_json Conversion") {
        auto j = series.to_json();
        REQUIRE(j[0]["timestamp"] == 1704495600);
        REQUIRE(j[0]["open"] == 1.5);
        REQUIRE(j[0]["high"] == 2.5);
        REQUIRE(j[0]["low"] == 0.5);
        REQUIRE(j[0]["close"] == 1.0);
        REQUIRE(j[0]["volume"] == 100);
    }
}

TEST_CASE("SeriesOHLCV iterator Functionality", "[SeriesOHLCV]") {
    SeriesOHLCV series;
    symbol_t symbol = std::make_shared<std::string>("AAPL");
    OHLCV ohlcv1(symbol, 1704510000, 1.5, 2.5, 0.5, 1.0, 100);
    OHLCV ohlcv2(symbol, 1704540000, 4.5, 5.5, 3.5, 4.0, 400);
    OHLCV ohlcv3(symbol, 1704530000, 3.5, 4.5, 2.5, 3.0, 300);
    OHLCV ohlcv4(symbol, 1704550000, 5.5, 6.5, 4.5, 5.0, 500);
    OHLCV ohlcv5(symbol, 1704520000, 2.5, 3.5, 1.5, 2.0, 200);

    series.insert(ohlcv2);
    series.insert(ohlcv3);
    series.insert(ohlcv4);
    series.insert(ohlcv5);
    series.insert(ohlcv1);

    SECTION("Begin") {
        SeriesOHLCV::iterator begin = series.begin();
        const auto &[timestamp, ohlc] = *begin;
        REQUIRE(ohlc.timestamp == 1704510000);
        REQUIRE(timestamp == 1704510000);
    }

    SECTION("Last element") {
        SeriesOHLCV::iterator end = series.end();
        --end;
        const auto &[timestamp, ohlc] = *end;
        REQUIRE(ohlc.timestamp == 1704550000);
        REQUIRE(timestamp == 1704550000);
    }

    SECTION("Iterating over all elements") {
        int count = 0;
        for (auto it = series.begin(); it != series.end(); ++it) {
            count++;
        }
        REQUIRE(count == 5);
    }

    SECTION("Iterating over all elements and check order") {
        int count = 0;
        for (auto it = series.begin(); it != series.end(); ++it) {
            count++;
            const auto &[timestamp, ohlc] = *it;
            REQUIRE(ohlc.timestamp == 1704500000 + (count * 10000));
        }
        REQUIRE(count == 5);
    }

    SECTION("Reverse Iterating over all elements and check order with auto") {
        int count = 0;
        for (auto it = series.rbegin(); it != series.rend(); --it) {
            count++;
            const auto &[timestamp, ohlc] = *it;
            REQUIRE(ohlc.timestamp == 1704560000 - (count * 10000));
        }
    }


}

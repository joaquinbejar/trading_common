//
// Created by Joaquin Bejar Garcia on 27/1/24.
//

#include <trading_common/instructions.h>
#include <catch2/catch_test_macros.hpp>


using namespace common::instructions;

class simpleJson {
    public:
        std::string param1;
        int param2;
    json to_json() const {
        return json{{"param1", param1}, {"param2", param2}};
    }
    void from_json(const json &j) {
        param1 = j.contains("param1") ? j["param1"].get<std::string>() : "";
        param2 = j.contains("param2") ? j["param2"].get<int>() : 0;
    }
};

TEST_CASE("Type enum and functions") {
    SECTION("get_type_name") {
        REQUIRE(get_type_name(Type::NONE).empty());
        REQUIRE(get_type_name(Type::TICKER) == "ticker");
    }

    SECTION("get_type_from_string") {
        REQUIRE(get_type_from_string("ticker") == Type::TICKER);
        REQUIRE(get_type_from_string("invalid") == Type::NONE);
    }
}

TEST_CASE("Selector enum and functions") {
    SECTION("get_selector_name") {
        REQUIRE(get_selector_name(Selector::NONE).empty());
        REQUIRE(get_selector_name(Selector::ALL) == "all");
    }

    SECTION("get_selector_from_string") {
        REQUIRE(get_selector_from_string("all") == Selector::ALL);
        REQUIRE(get_selector_from_string("invalid") == Selector::NONE);
    }
}

TEST_CASE("Instructions struct") {
    Instructions<simpleJson> instructions;

    SECTION("to_json and from_json") {
        instructions.type = Type::EMA;
        instructions.selector = Selector::ONE;
        instructions.tickers = {"AAPL", "MSFT"};

        json j = instructions.to_json();

        REQUIRE(j["type"] == "ema");
        REQUIRE(j["selector"] == "one");
        REQUIRE(j["tickers"] == json({"AAPL", "MSFT"}));

        json j2 = {{"type", "sma"}, {"selector", "set"}, {"tickers", {"GOOG", "AMZN"}}};
        instructions.from_json(j2);
        REQUIRE(instructions.type == Type::SMA);
        REQUIRE(instructions.selector == Selector::SET);
        REQUIRE(instructions.tickers == std::vector<std::string>({"GOOG", "AMZN"}));

        json j3 = {{"type", "sma"},
                   {"selector", "set"},
                   {"tickers", {"GOOG", "AMZN"}},
                   {"other", {{"param1", "value1"}, {"param2", 2}}} };
        instructions.from_json(j3);
        REQUIRE(instructions.type == Type::SMA);
        REQUIRE(instructions.selector == Selector::SET);
        REQUIRE(instructions.tickers == std::vector<std::string>({"GOOG", "AMZN"}));
        REQUIRE(instructions.other.param1 == "value1");
        REQUIRE(instructions.other.param2 == 2);
    }

    SECTION("to_string") {
        instructions.type = Type::EMA;
        instructions.selector = Selector::ONE;
        instructions.tickers = {"AAPL", "MSFT"};

        std::string expected_str = R"({"other":{"param1":"","param2":2},"selector":"one","tickers":["AAPL","MSFT"],"type":"ema"})";
        REQUIRE(instructions.to_string() == expected_str);
    }

    SECTION("std::string") {
        instructions.type = Type::EMA;
        instructions.selector = Selector::ONE;
        instructions.tickers = {"AAPL", "MSFT"};

        std::string expected_str = R"({"other":{"param1":"","param2":2},"selector":"one","tickers":["AAPL","MSFT"],"type":"ema"})";
        REQUIRE(instructions.to_string() == expected_str);
        std::string get_string = instructions;
        REQUIRE(get_string == expected_str);
    }

    SECTION("from_string") {
        std::string expected_str = R"({"other":{"param1":"","param2":2},"selector":"one","tickers":["AAPL","MSFT"],"type":"ema"})";
        instructions.from_string(expected_str);
        REQUIRE(instructions.type == Type::EMA);
        REQUIRE(instructions.selector == Selector::ONE);
        REQUIRE(instructions.tickers == std::vector<std::string>({"AAPL", "MSFT"}));
        REQUIRE(instructions.other.param1.empty());
        REQUIRE(instructions.other.param2 == 2);
    }
}

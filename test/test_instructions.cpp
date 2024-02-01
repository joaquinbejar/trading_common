//
// Created by Joaquin Bejar Garcia on 27/1/24.
//

#include <trading_common/instructions.h>
#include <catch2/catch_test_macros.hpp>


using namespace trading::instructions;

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

class MetaInstruction {

public:
    std::string date;
    std::string table; // destination table in DB
    bool gte = false; // greater than or equal

    [[nodiscard]] json to_json() const {
        return {{"date",  date},
                {"table", table},
                {"gte",   gte}};
    }

    void from_json(const json &j) {
        if (j.contains("date"))
            date = j.at("date").get<std::string>();
        if (j.contains("table"))
            table = j.at("table").get<std::string>();
        if (j.contains("gte"))
            gte = j.at("gte").get<bool>();
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



TEST_CASE("Testing Instructions<MetaInstruction>", "[Instructions<MetaInstruction>]") {
    Instructions<MetaInstruction> instruction;
    json j = R"(
        {
            "type": "ticker",
            "selector": "one",
            "tickers": ["AAPL"],
            "timestamp": 1706639471,
            "other": {
                "table": "Tickers",
                "date": "2024-01-30",
                "gte": true
            }
        }
        )"_json;
    instruction.from_json(j);
    REQUIRE(instruction.type == Type::TICKER);
    REQUIRE(instruction.selector == Selector::ONE);
    REQUIRE(instruction.tickers.size() == 1);
    REQUIRE(instruction.tickers[0] == "AAPL");
    REQUIRE(instruction.timestamp == 1706639471);
    REQUIRE(instruction.other.date == "2024-01-30");
    REQUIRE(instruction.other.table == "Tickers");
    REQUIRE(instruction.other.gte);
    json to_json = instruction.to_json();
    REQUIRE(to_json["type"] == "ticker");
    REQUIRE(to_json["selector"] == "one");
    REQUIRE(to_json["tickers"] == json({"AAPL"}));
    REQUIRE(to_json["timestamp"] == 1706639471);
    REQUIRE(to_json["other"]["date"] == "2024-01-30");
    REQUIRE(to_json["other"]["table"] == "Tickers");
    REQUIRE(to_json["other"]["gte"]);
    REQUIRE(instruction.to_string() == R"({"other":{"date":"2024-01-30","gte":true,"table":"Tickers"},"selector":"one","tickers":["AAPL"],"timestamp":1706639471,"type":"ticker"})");
    Instructions<MetaInstruction> instruction1;
    instruction1.from_string(R"({"other":{"date":"2024-01-30","gte":true,"table":"Tickers"},"selector":"one","tickers":["AAPL"],"timestamp":1706639471,"type":"ticker"})");
    REQUIRE(instruction1.type == Type::TICKER);
    REQUIRE(instruction1.selector == Selector::ONE);
    REQUIRE(instruction1.tickers.size() == 1);
    REQUIRE(instruction1.tickers[0] == "AAPL");
    REQUIRE(instruction1.timestamp == 1706639471);
    REQUIRE(instruction1.other.date == "2024-01-30");
    REQUIRE(instruction1.other.table == "Tickers");
    REQUIRE(instruction1.other.gte);
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

        json j2 = {{"type", "sma"}, {"selector", "set"}, {"tickers", {"GOOG", "AMZN"}}, {"timestamp", 1706545894}};
        instructions.from_json(j2);
        REQUIRE(instructions.type == Type::SMA);
        REQUIRE(instructions.selector == Selector::SET);
        REQUIRE(instructions.tickers == std::vector<std::string>({"GOOG", "AMZN"}));
        REQUIRE(instructions.timestamp == 1706545894);

        json j3 = {{"type", "sma"},
                   {"selector", "set"},
                   {"tickers", {"GOOG", "AMZN"}},
                   {"other", {{"param1", "value1"}, {"param2", 2}}},
                   {"timestamp", 1706545894} };
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
        instructions.timestamp = 1706546004;

        std::string expected_str = R"({"other":{"param1":"","param2":2},"selector":"one","tickers":["AAPL","MSFT"],"timestamp":1706546004,"type":"ema"})";
        REQUIRE(instructions.to_string() == expected_str);
    }

    SECTION("std::string") {
        instructions.type = Type::EMA;
        instructions.selector = Selector::ONE;
        instructions.tickers = {"AAPL", "MSFT"};
        instructions.timestamp = 1706546004;

        std::string expected_str = R"({"other":{"param1":"","param2":2},"selector":"one","tickers":["AAPL","MSFT"],"timestamp":1706546004,"type":"ema"})";
        REQUIRE(instructions.to_string() == expected_str);
        std::string get_string = instructions;
        REQUIRE(get_string == expected_str);
    }

    SECTION("from_string") {
        std::string expected_str = R"({"other":{"param1":"","param2":2},"selector":"one","tickers":["AAPL","MSFT"],"timestamp":1706546004,"type":"ema"})";
        instructions.from_string(expected_str);
        REQUIRE(instructions.type == Type::EMA);
        REQUIRE(instructions.selector == Selector::ONE);
        REQUIRE(instructions.tickers == std::vector<std::string>({"AAPL", "MSFT"}));
        REQUIRE(instructions.other.param1.empty());
        REQUIRE(instructions.other.param2 == 2);
    }
}

TEST_CASE("Instructions struct in threadqueue") {
    ::common::ThreadQueue<Instructions<simpleJson>> queue;

    SECTION("Enqueue 1") {
        Instructions<simpleJson> instruction;
        instruction.type = Type::EMA;
        instruction.selector = Selector::ONE;
        instruction.tickers = {"AAPL", "MSFT"};
        instruction.other.param1 = "value1";
        instruction.other.param2 = 2;
        instruction.timestamp = 1706546004;

        queue.enqueue(instruction);
        REQUIRE(queue.size() == 1);

        Instructions<simpleJson> instruction2;
        bool result = queue.dequeue(instruction2);
        REQUIRE(result);
        REQUIRE(instruction2.type == Type::EMA);
        REQUIRE(instruction2.selector == Selector::ONE);
        REQUIRE(instruction2.tickers == std::vector<std::string>({"AAPL", "MSFT"}));
        REQUIRE(instruction2.other.param1 == "value1");
        REQUIRE(instruction2.other.param2 == 2);
        std::string expected_str = R"({"other":{"param1":"value1","param2":2},"selector":"one","tickers":["AAPL","MSFT"],"timestamp":1706546004,"type":"ema"})";
        REQUIRE(instruction2.to_string() == expected_str);
    }

    SECTION("Enqueue 2") {
        Instructions<simpleJson> instruction;
        instruction.from_string(R"({"other":{"param1":"value1","param2":2},"selector":"one","tickers":["AAPL","MSFT"],"timestamp":1706546004,"type":"ema"})");
        queue.enqueue(instruction);
        REQUIRE(queue.size() == 1);

        Instructions<simpleJson> instruction2;
        bool result = queue.dequeue(instruction2);
        REQUIRE(result);
        REQUIRE(instruction2.type == Type::EMA);
        REQUIRE(instruction2.selector == Selector::ONE);
        REQUIRE(instruction2.tickers == std::vector<std::string>({"AAPL", "MSFT"}));
        REQUIRE(instruction2.other.param1 == "value1");
        REQUIRE(instruction2.other.param2 == 2);
        std::string expected_str = R"({"other":{"param1":"value1","param2":2},"selector":"one","tickers":["AAPL","MSFT"],"timestamp":1706546004,"type":"ema"})";
        REQUIRE(instruction2.to_string() == expected_str);
    }

}
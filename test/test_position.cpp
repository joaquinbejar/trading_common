//
// Created by Joaquin Bejar Garcia on 19/10/23.
//


#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <trading_common/order.h>
#include <trading_common/position.h>
#include "nlohmann/json.hpp"

using namespace trading::position;
using json = nlohmann::json;
using Order = trading::order::Order;

TEST_CASE("Position Class Tests", "[Position]") {
    Position position;

    SECTION("Initialization of Position") {
        REQUIRE(position.balance == 0);
        REQUIRE(position.current_price == 0);
        REQUIRE(position.entry_price == 0);
        REQUIRE(!position.id.empty());
        REQUIRE(position.pnl == 0);
        REQUIRE(position.side == Side::NONE);
        REQUIRE(position.symbol->empty());
        REQUIRE(position.timestamp != 0);
        REQUIRE(position.validate());
    }

    SECTION("Validation of Position without Side") {
        position.balance = 10;
        position.current_price = 10;
        position.entry_price = 10;
        position.pnl = 100;
        position.side = Side::NONE;
        position.symbol = std::make_shared<std::string>("BTC-USD");
        REQUIRE_FALSE(position.validate());
    }

    SECTION("Validation of Position without Symbol") {
        position.side = Side::LONG;
        position.balance = 10;
        position.current_price = 10;
        position.entry_price = 10;
        position.pnl = 100;
        position.symbol = std::make_shared<std::string>("");
        REQUIRE_FALSE(position.validate());
    }

    SECTION("Validation of Position without entry_price") {
        position.side = Side::LONG;
        position.balance = 10;
        position.current_price = 10;
        position.entry_price = 0;
        position.pnl = 100;
        position.symbol = std::make_shared<std::string>("BTC-USD");
        REQUIRE_FALSE(position.validate());
    }
}

TEST_CASE("Position apply_order Method Tests", "[Position]") {
    Position position;

    SECTION("Apply BUY Order to Position empty ") {
        price_t current_prince = 600;
        json j = {
                {"quantity",        200},
                {"symbol",          "BTC"},
                {"side",            "buy"},
                {"filled",          200},
                {"filled_at_price", 500},
                {"limit_price",     0},
                {"type",            "market"},
                {"status",          "closed"}
        };

        Order order(j);
        REQUIRE(position.validate());
        position.set_current_price(current_prince);
        ApplyOrderResult result = position.apply_order(order);
        REQUIRE(position.validate());
        REQUIRE(result.message.empty());
        REQUIRE(result.success);
        REQUIRE(result.pnl == 200 * (current_prince - position.entry_price));
        REQUIRE(position.balance == 200);
        REQUIRE(position.entry_price == 500);
        REQUIRE(position.side == Side::LONG);
        REQUIRE(*position.symbol == *order.symbol);
    }

    SECTION("Apply SELL Order to Position empty ") {
        price_t current_prince = 600;
        json j = {
                {"quantity",        200},
                {"symbol",          "BTC"},
                {"side",            "sell"},
                {"filled",          200},
                {"filled_at_price", 500},
                {"limit_price",     500},
                {"type",            "limit"},
                {"status",          "closed"}
        };

        Order order(j);
        REQUIRE(position.validate());
        position.set_current_price(current_prince);
        ApplyOrderResult result = position.apply_order(order);
        REQUIRE(position.validate());
        REQUIRE(result.message.empty());
        REQUIRE(result.success);
        REQUIRE(result.pnl == 200 * (position.entry_price - current_prince));
        REQUIRE(position.balance == 200);
        REQUIRE(position.entry_price == 500);
        REQUIRE(position.side == Side::SHORT);
        REQUIRE(*position.symbol == *order.symbol);
    }

    SECTION("Apply BUY Order to Long Position") {
        price_t current_prince = 600;
        json j1 = {
                {"quantity",        200},
                {"symbol",          "BTC"},
                {"side",            "buy"},
                {"filled",          200},
                {"filled_at_price", 500},
                {"limit_price",     500},
                {"type",            "limit"},
                {"status",          "closed"}
        };
        Order order1(j1);
        json j2 = {
                {"quantity",        200},
                {"symbol",          "BTC"},
                {"side",            "buy"},
                {"filled",          100},
                {"filled_at_price", 450},
                {"limit_price",     0},
                {"type",            "market"},
                {"status",          "closed"}
        };
        Order order2(j2);
        std::cout << "Position PRE: " << position.to_json().dump(4) << std::endl;
        REQUIRE(position.validate());
        position.set_current_price(current_prince);
        ApplyOrderResult result1 = position.apply_order(order1);
        std::cout << "result1: " << result1.message << std::endl;
        ApplyOrderResult result2 = position.apply_order(order2);
        std::cout << "result2: " << result2.message << std::endl;
        std::cout << "Position POST: " << position.to_json().dump(4) << std::endl;

        REQUIRE(position.validate());
        REQUIRE(result2.message.empty());
        REQUIRE(result2.success);
        REQUIRE(position.balance == 300);
        REQUIRE(position.pnl == 300 * (current_prince - position.entry_price));
        REQUIRE(position.entry_price == (double) ((200 * 500) + (100 * 450)) / 300);
        REQUIRE(position.side == Side::LONG);
        REQUIRE(*position.symbol == *order1.symbol);
    }

//    SECTION("Apply SELL Order to Short Position") {
//        price_t current_prince = 600;
//        json j1 = {
//                {"quantity",        200},
//                {"symbol",          "BTC"},
//                {"side",            "sell"},
//                {"filled",          200},
//                {"filled_at_price", 500},
//                {"limit_price",     500},
//                {"type",            "limit"},
//                {"status",          "closed"}
//        };
//        Order order1(j1);
//        json j2 = {
//                {"quantity",        200},
//                {"symbol",          "BTC"},
//                {"side",            "sell"},
//                {"filled",          100},
//                {"filled_at_price", 450},
//                {"limit_price",     0},
//                {"type",            "market"},
//                {"status",          "closed"}
//        };
//        Order order2(j2);
//        REQUIRE(position.validate());
//        ApplyOrderResult result1 = position.apply_order(order1);
//        std::cout << "result1: " << result1.message << std::endl;
//        ApplyOrderResult result2 = position.apply_order(order2);
//        std::cout << "result2: " << result2.message << std::endl;
//        position.set_current_price(current_prince);
//
//        std::cout << "Position POST: " << position.to_json().dump(4) << std::endl;
//
//        REQUIRE(position.validate());
//        REQUIRE(result2.message.empty());
//        REQUIRE(result2.success);
//        REQUIRE(position.balance == 300);
//        REQUIRE(position.pnl == 300 * (position.entry_price - current_prince));
//        REQUIRE(position.entry_price == (double) ((200 * 500) + (100 * 450)) / 300);
//        REQUIRE(position.side == Side::SHORT);
//        REQUIRE(*position.symbol == *order1.symbol);
//    }

}
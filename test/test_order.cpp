//
// Created by Joaquin Bejar Garcia on 19/10/23.
//


#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "trading_common/order.h"
#include "nlohmann/json.hpp"
#include <iostream>

using namespace trading::order;
using json = nlohmann::json;

TEST_CASE("Order struct tests", "[Order]") {
    using namespace trading::order;

    SECTION("Default constructor test") {
        Order order;
        REQUIRE(order.timestamp == 0);
        REQUIRE(order.quantity == 0);
    }

    SECTION("Order from JSON test") {
        json j = {
                {"timestamp", 100},
                {"quantity", 200},
                {"symbol", "BTC"},
                {"side", "buy"},
                {"filled", 0},
                {"filled_at_price", 500},
                {"limit_price", 500},
                {"id", "123"},
                {"type", "market"}
        };

        Order order(j);
        REQUIRE(order.timestamp == 100);
        REQUIRE(order.quantity == 200);
        REQUIRE(*order.symbol == "BTC");
        REQUIRE(order.side == Side::BUY);
        REQUIRE(order.filled == 0);
        REQUIRE(order.filled_at_price == 500);
        REQUIRE(order.limit_price == 500);
        REQUIRE(order.id == "123");
        REQUIRE(order.type == Type::MARKET);


    }

    SECTION("Order Constructor to json test"){
        symbol_t symbol = std::make_shared<std::string>("BTC");
        Order order(100, 200, symbol, Side::BUY, 0, 500, 500, "123", Type::MARKET);

        json j = order.to_json();
        REQUIRE(j["timestamp"] == 100);
        REQUIRE(j["quantity"] == 200);
        REQUIRE(j["symbol"] == "BTC");
        REQUIRE(j["side"] == "BUY");
        REQUIRE(j["filled"] == 0);
        REQUIRE(j["filled_at_price"] == 500);
        REQUIRE(j["limit_price"] == 500);
        REQUIRE(j["id"] == "123");
        REQUIRE(j["type"] == "MARKET");
    }

    SECTION("Order to json test"){
        symbol_t symbol = std::make_shared<std::string>("BTC");
        Order order;
        order.timestamp = 100;
        order.quantity = 200;
        order.symbol = symbol;
        order.side = Side::BUY;
        order.filled = 0;
        order.filled_at_price = 500;
        order.limit_price = 500;
        order.id = "123";
        order.type = Type::MARKET;

        json j = order.to_json();
        REQUIRE(j["timestamp"] == 100);
        REQUIRE(j["quantity"] == 200);
        REQUIRE(j["symbol"] == "BTC");
        REQUIRE(j["side"] == "BUY");
        REQUIRE(j["filled"] == 0);
        REQUIRE(j["filled_at_price"] == 500);
        REQUIRE(j["limit_price"] == 500);
        REQUIRE(j["id"] == "123");
        REQUIRE(j["type"] == "MARKET");
    }
}

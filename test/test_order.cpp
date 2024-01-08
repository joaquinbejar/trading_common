//
// Created by Joaquin Bejar Garcia on 19/10/23.
//


#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "trading_common/order.h"
#include "nlohmann/json.hpp"

using namespace trading::order;
using json = nlohmann::json;

TEST_CASE("Order struct tests", "[Order]") {
    using namespace trading::order;

    SECTION("Default constructor test") {
        Order order;
        REQUIRE(order.timestamp != 0);
        REQUIRE(order.quantity == 0);
        REQUIRE(!order.id.empty());
    }

    SECTION("Order from JSON test") {
        json j = {
                {"timestamp",       100},
                {"quantity",        200},
                {"symbol",          "BTC"},
                {"side",            "buy"},
                {"filled",          0},
                {"filled_at_price", 500},
                {"limit_price",     500},
                {"id",              "123"},
                {"type",            "market"},
                {"status",          "open"}
        };

        Order order(j);
        REQUIRE(order.timestamp == 100);
        REQUIRE(order.quantity == 200);
        REQUIRE(*order.symbol == "BTC");
        REQUIRE(order.side == Side::BUY);
        REQUIRE(order.filled == 0);
        REQUIRE(order.filled_at_price == 500);
        REQUIRE(order.limit_price == 500);
        REQUIRE(order.type == Type::MARKET);
        REQUIRE(order.status == Status::OPEN);
    }

    SECTION("Order Constructor to json test") {
        symbol_t symbol = std::make_shared<std::string>("BTC");
        Order order(100, 200, symbol, Side::BUY, 0, 500, 500, "123", Type::MARKET, Status::OPEN);

        json j = order.to_json();
        REQUIRE(j["timestamp"] == 100);
        REQUIRE(j["quantity"] == 200);
        REQUIRE(j["symbol"] == "BTC");
        REQUIRE(j["side"] == "BUY");
        REQUIRE(j["filled"] == 0);
        REQUIRE(j["filled_at_price"] == 500);
        REQUIRE(j["limit_price"] == 500);
        REQUIRE(j["type"] == "MARKET");
        REQUIRE(j["status"] == "OPEN");
    }

    SECTION("Order to json test") {
        symbol_t symbol = std::make_shared<std::string>("BTC");
        Order order;
        order.timestamp = 100;
        order.quantity = 200;
        order.symbol = symbol;
        order.side = Side::BUY;
        order.filled = 0;
        order.filled_at_price = 500;
        order.limit_price = 500;
        order.type = Type::MARKET;

        json j = order.to_json();
        REQUIRE(j["timestamp"] == 100);
        REQUIRE(j["quantity"] == 200);
        REQUIRE(j["symbol"] == "BTC");
        REQUIRE(j["side"] == "BUY");
        REQUIRE(j["filled"] == 0);
        REQUIRE(j["filled_at_price"] == 500);
        REQUIRE(j["limit_price"] == 500);
        REQUIRE(j["type"] == "MARKET");
    }
}


TEST_CASE("Order Validation Tests", "[Order]") {
    Order order;

    SECTION("Order with all default values should be valid") {
        REQUIRE(order.validate() == true);
    }

    SECTION("Order with zero quantity should be invalid") {
        json j = R"({
            "quantity": 0,
            "symbol": "BTC",
            "side": "BUY",
            "filled": 0,
            "filled_at_price": 0,
            "limit_price": 0,
            "type": "MARKET",
            "status": "OPEN"
        })"_json;
        Order open_qty = Order(j);
        REQUIRE(open_qty.validate() == false);
    }

    SECTION("Order with non-zero quantity should be invalid but symbol is empty") {
        order.quantity = 10;
        REQUIRE(order.validate() == false);
    }

    SECTION("Order with non-null symbol should be invalid but no side") {
        order.quantity = 10;
        order.symbol = std::make_shared<std::string>("AAPL");
        REQUIRE(order.validate() == false);
    }

    SECTION("Order with valid side should be invalid but no type") {
        order.quantity = 10;
        order.symbol = std::make_shared<std::string>("AAPL");
        order.side = trading::order::Side::BUY;
        REQUIRE(order.validate() == false);
    }

    SECTION("Order with valid type should be invalid") {
        order.quantity = 10;
        order.symbol = std::make_shared<std::string>("AAPL");
        order.side = trading::order::Side::BUY;
        order.type = trading::order::Type::LIMIT;
        REQUIRE(order.validate() == false);
    }

    SECTION("Order with valid status should be invalid but no filled") {
        order.quantity = 10;
        order.symbol = std::make_shared<std::string>("AAPL");
        order.side = trading::order::Side::BUY;
        order.type = trading::order::Type::LIMIT;
        order.status = trading::order::Status::OPEN;
        REQUIRE(order.validate() == false);
    }

    SECTION("Order with valid filled should be invalid but its open") {
        order.quantity = 10;
        order.symbol = std::make_shared<std::string>("AAPL");
        order.side = trading::order::Side::BUY;
        order.type = trading::order::Type::MARKET;
        order.status = trading::order::Status::OPEN;
        order.filled = 10;
        REQUIRE(order.validate() == false);
    }

    SECTION("OPEN Order with valid fields should be valid") {
        json j = R"({
            "quantity": 1,
            "symbol": "BTC",
            "side": "BUY",
            "filled": 0,
            "filled_at_price": 0,
            "limit_price": 0,
            "type": "MARKET",
            "status": "OPEN"
        })"_json;
        Order open_order = Order(j);
        REQUIRE(open_order.validate() == true);
    }

    SECTION("CLOSED Order with valid fields should be valid") {
        json j = R"({
            "quantity": 1,
            "symbol": "BTC",
            "side": "BUY",
            "filled": 0,
            "filled_at_price": 0,
            "limit_price": 0,
            "type": "MARKET",
            "status": "CLOSED"
        })"_json;
        Order closed_order = Order(j);
        REQUIRE(closed_order.validate() == true);
    }

    SECTION("FILLED Order with valid fields should be valid") {
        json j = R"({
            "quantity": 1,
            "symbol": "BTC",
            "side": "BUY",
            "filled": 1,
            "filled_at_price": 10,
            "limit_price": 10,
            "type": "LIMIT",
            "status": "FILLED"
        })"_json;
        Order filled_order = Order(j);
//        std::cout << filled_order.to_json().dump(4) << std::endl;
        REQUIRE(filled_order.validate() == true);
    }

    SECTION("FILLED Order with valid fields but filled set and filled_at_price are zero should be invalid") {
        json j = R"({
            "quantity": 1,
            "symbol": "BTC",
            "side": "BUY",
            "filled": 1,
            "filled_at_price": 0,
            "limit_price": 10,
            "type": "LIMIT",
            "status": "FILLED"
        })"_json;
        Order filled_order = Order(j);
        REQUIRE(filled_order.validate() == false);
    }

    SECTION("FILLED Order with valid fields but filled_at_price set and filled are zero should be invalid") {
        json j = R"({
            "quantity": 1,
            "symbol": "BTC",
            "side": "BUY",
            "filled": 0,
            "filled_at_price": 10,
            "limit_price": 10,
            "type": "LIMIT",
            "status": "FILLED"
        })"_json;
        Order filled_order = Order(j);
        REQUIRE(filled_order.validate() == false);
    }

    SECTION("CANCELED Order with valid fields should be valid") {
        json j = R"({
            "quantity": 1,
            "symbol": "BTC",
            "side": "BUY",
            "filled": 0,
            "filled_at_price": 0,
            "limit_price": 0,
            "type": "MARKET",
            "status": "CANCELED"
        })"_json;
        Order canceled_order = Order(j);
        REQUIRE(canceled_order.validate() == true);
    }

}


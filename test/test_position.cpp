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
        Position::ApplyOrderResult result = position.apply_order(order);
        REQUIRE(position.validate());

        json j_position = position.to_json();
        std::cout << j_position.dump(4) << std::endl;
        Position position2(j_position);
        REQUIRE(position2.validate());
        std::cout << position2.to_json().dump(4) << std::endl;
        Position position3 = Position(j_position.at("id").get<trading::position::id_t>(),
                                      j_position.at("timestamp").get<trading::position::timestamp_t>(),
                                      j_position.at("balance").get<size_t>(),
                                      std::make_shared<std::string>(j_position.at("symbol").get<std::string>()),
                                      Side::LONG,
                                      j_position.at("entry_price").get<trading::position::price_t>(),
                                      j_position.at("current_price").get<trading::position::price_t>(),
                                      j_position.at("pnl").get<trading::position::price_t>());
        REQUIRE(position3.validate());
        std::cout << position3.to_json().dump(4) << std::endl;


    }

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
        Position::ApplyOrderResult result = position.apply_order(order);
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
        Position::ApplyOrderResult result = position.apply_order(order);
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
        REQUIRE(position.validate());
        position.set_current_price(current_prince);
        Position::ApplyOrderResult result1 = position.apply_order(order1);
        Position::ApplyOrderResult result2 = position.apply_order(order2);

        REQUIRE(position.validate());
        REQUIRE(result2.message.empty());
        REQUIRE(result2.success);
        REQUIRE(position.balance == 300);
        REQUIRE(position.pnl == 300 * (current_prince - position.entry_price));
        REQUIRE(position.entry_price == (double) ((200 * 500) + (100 * 450)) / 300);
        REQUIRE(position.side == Side::LONG);
        REQUIRE(*position.symbol == *order1.symbol);
    }

    SECTION("Apply SELL Order to Short Position") {
        price_t current_prince = 600;
        json j1 = {
                {"quantity",        200},
                {"symbol",          "BTC"},
                {"side",            "sell"},
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
                {"side",            "sell"},
                {"filled",          100},
                {"filled_at_price", 450},
                {"limit_price",     0},
                {"type",            "market"},
                {"status",          "closed"}
        };
        Order order2(j2);
        REQUIRE(position.validate());
        position.set_current_price(current_prince);
        Position::ApplyOrderResult result1 = position.apply_order(order1);
        Position::ApplyOrderResult result2 = position.apply_order(order2);

        REQUIRE(position.validate());
        REQUIRE(result2.message.empty());
        REQUIRE(result2.success);
        REQUIRE(position.balance == 300);
        REQUIRE(position.pnl == 300 * (position.entry_price - current_prince));
        REQUIRE(position.entry_price == (double) ((200 * 500) + (100 * 450)) / 300);
        REQUIRE(position.side == Side::SHORT);
        REQUIRE(*position.symbol == *order1.symbol);
    }

    SECTION("Apply SELL Order to Long Position") {
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
                {"quantity",        100},
                {"symbol",          "BTC"},
                {"side",            "sell"},
                {"filled",          100},
                {"filled_at_price", 450},
                {"limit_price",     0},
                {"type",            "market"},
                {"status",          "closed"}
        };
        Order order2(j2);
        REQUIRE(position.validate());
        position.set_current_price(current_prince);
        Position::ApplyOrderResult result1 = position.apply_order(order1);
        Position::ApplyOrderResult result2 = position.apply_order(order2);

        REQUIRE(position.validate());
        REQUIRE(result2.message.empty());
        REQUIRE(result2.success);
        REQUIRE(position.balance == 100);
        REQUIRE(position.pnl == 100 * (current_prince - position.entry_price));
        REQUIRE(position.entry_price == (double) ((200 * 500) - (100 * 450)) / 100);
        REQUIRE(position.side == Side::LONG);
        REQUIRE(*position.symbol == *order1.symbol);
    }

    SECTION("Apply BIGGER SELL Order to Long Position 1") {
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
                {"quantity",        500},
                {"symbol",          "BTC"},
                {"side",            "sell"},
                {"filled",          400},
                {"filled_at_price", 450},
                {"limit_price",     0},
                {"type",            "market"},
                {"status",          "closed"}
        };
        Order order2(j2);
        REQUIRE(position.validate());
        position.set_current_price(current_prince);
        Position::ApplyOrderResult result1 = position.apply_order(order1);
        Position::ApplyOrderResult result2 = position.apply_order(order2);

        REQUIRE(position.validate());
        REQUIRE(result2.message.empty());
        REQUIRE(result2.success);
        REQUIRE(position.balance == 200);
        REQUIRE(position.entry_price == 450);
        REQUIRE(position.pnl == -40000);
        REQUIRE(position.side == Side::SHORT);
        REQUIRE(*position.symbol == *order1.symbol);
    }

    SECTION("Apply BIGGER SELL Order to Long Position 2") {
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
                {"quantity",        400},
                {"symbol",          "BTC"},
                {"side",            "sell"},
                {"filled",          400},
                {"filled_at_price", 600},
                {"limit_price",     0},
                {"type",            "market"},
                {"status",          "closed"}
        };
        Order order2(j2);
        REQUIRE(position.validate());
        position.set_current_price(current_prince);
        Position::ApplyOrderResult result1 = position.apply_order(order1);
        Position::ApplyOrderResult result2 = position.apply_order(order2);

        REQUIRE(position.validate());
        REQUIRE(result2.message.empty());
        REQUIRE(result2.success);
        REQUIRE(position.balance == 200);
        REQUIRE(position.entry_price == 600);
        REQUIRE(position.pnl == 20000);
        REQUIRE(position.side == Side::SHORT);
        REQUIRE(*position.symbol == *order1.symbol);
    }

    SECTION("Apply BIGGER SELL Order to Long Position 3") {
        price_t current_prince = 500;
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
                {"quantity",        400},
                {"symbol",          "BTC"},
                {"side",            "sell"},
                {"filled",          400},
                {"filled_at_price", 600},
                {"limit_price",     0},
                {"type",            "market"},
                {"status",          "closed"}
        };
        Order order2(j2);
        REQUIRE(position.validate());
        position.set_current_price(current_prince);
        Position::ApplyOrderResult result1 = position.apply_order(order1);
        Position::ApplyOrderResult result2 = position.apply_order(order2);

        REQUIRE(position.validate());
        REQUIRE(result2.message.empty());
        REQUIRE(result2.success);
        REQUIRE(position.balance == 200);
        REQUIRE(position.entry_price == 600);
        REQUIRE(position.pnl == 40000);
        REQUIRE(position.side == Side::SHORT);
        REQUIRE(*position.symbol == *order1.symbol);
    }


    SECTION("Apply BUY Order to Short Position 1") {
        price_t current_prince = 200;
        json j1 = {
                {"quantity",        200},
                {"symbol",          "BTC"},
                {"side",            "sell"},
                {"filled",          200},
                {"filled_at_price", 100},
                {"limit_price",     100},
                {"type",            "limit"},
                {"status",          "closed"}
        };
        Order order1(j1);
        json j2 = {
                {"quantity",        400},
                {"symbol",          "BTC"},
                {"side",            "buy"},
                {"filled",          100},
                {"filled_at_price", 100},
                {"limit_price",     0},
                {"type",            "market"},
                {"status",          "closed"}
        };
        Order order2(j2);
        REQUIRE(position.validate());
        position.set_current_price(current_prince);
        Position::ApplyOrderResult result1 = position.apply_order(order1);
        Position::ApplyOrderResult result2 = position.apply_order(order2);

        REQUIRE(position.validate());
        REQUIRE(result2.message.empty());
        REQUIRE(result2.success);
        REQUIRE(position.balance == 100);
        REQUIRE(position.entry_price == 100);
        REQUIRE(position.pnl == -10000);
        REQUIRE(position.side == Side::SHORT);
        REQUIRE(*position.symbol == *order1.symbol);
    }


    SECTION("Apply BUY Order to Short Position 2") {
        price_t current_prince = 100;
        json j1 = {
                {"quantity",        200},
                {"symbol",          "BTC"},
                {"side",            "sell"},
                {"filled",          200},
                {"filled_at_price", 200},
                {"limit_price",     200},
                {"type",            "limit"},
                {"status",          "closed"}
        };
        Order order1(j1);
        json j2 = {
                {"quantity",        100},
                {"symbol",          "BTC"},
                {"side",            "buy"},
                {"filled",          100},
                {"filled_at_price", 100},
                {"limit_price",     0},
                {"type",            "market"},
                {"status",          "closed"}
        };
        Order order2(j2);
        REQUIRE(position.validate());
        position.set_current_price(current_prince);
        Position::ApplyOrderResult result1 = position.apply_order(order1);
        REQUIRE(position.pnl == 20000);
        Position::ApplyOrderResult result2 = position.apply_order(order2);

        REQUIRE(position.validate());
        REQUIRE(result2.message.empty());
        REQUIRE(result2.success);
        REQUIRE(position.balance == 100);
        REQUIRE(position.entry_price == 300);
        REQUIRE(position.pnl == 20000);
        REQUIRE(position.side == Side::SHORT);
        REQUIRE(*position.symbol == *order1.symbol);
    }

    SECTION("Apply BUY Order to Short Position 3") {
        price_t current_prince = 100;
        json j1 = {
                {"quantity",        200},
                {"symbol",          "BTC"},
                {"side",            "sell"},
                {"filled",          200},
                {"filled_at_price", 200},
                {"limit_price",     200},
                {"type",            "limit"},
                {"status",          "closed"}
        };
        Order order1(j1);
        json j2 = {
                {"quantity",        100},
                {"symbol",          "BTC"},
                {"side",            "buy"},
                {"filled",          100},
                {"filled_at_price", 50},
                {"limit_price",     0},
                {"type",            "market"},
                {"status",          "closed"}
        };
        Order order2(j2);
        REQUIRE(position.validate());
        position.set_current_price(current_prince);
        Position::ApplyOrderResult result1 = position.apply_order(order1);
        REQUIRE(position.pnl == 20000);
        Position::ApplyOrderResult result2 = position.apply_order(order2);

        REQUIRE(position.validate());
        REQUIRE(result2.message.empty());
        REQUIRE(result2.success);
        REQUIRE(position.balance == 100);
        REQUIRE(position.entry_price == 350);
        REQUIRE(position.pnl == 25000);
        REQUIRE(position.side == Side::SHORT);
        REQUIRE(*position.symbol == *order1.symbol);
    }

    SECTION("Apply BUY Order to Short Position 3") {
        price_t current_prince = 100;
        json j1 = {
                {"quantity",        200},
                {"symbol",          "BTC"},
                {"side",            "sell"},
                {"filled",          200},
                {"filled_at_price", 200},
                {"limit_price",     200},
                {"type",            "limit"},
                {"status",          "closed"}
        };
        Order order1(j1);
        json j2 = {
                {"quantity",        400},
                {"symbol",          "BTC"},
                {"side",            "buy"},
                {"filled",          400},
                {"filled_at_price", 50},
                {"limit_price",     0},
                {"type",            "market"},
                {"status",          "closed"}
        };
        Order order2(j2);
        REQUIRE(position.validate());
        position.set_current_price(current_prince);
        Position::ApplyOrderResult result1 = position.apply_order(order1);
        REQUIRE(position.pnl == 20000);
        Position::ApplyOrderResult result2 = position.apply_order(order2);

        REQUIRE(position.validate());
        REQUIRE(result2.message.empty());
        REQUIRE(result2.success);
        REQUIRE(position.balance == 200);
        REQUIRE(position.entry_price == 50);
        REQUIRE(position.pnl == 40000);
        REQUIRE(position.side == Side::LONG);
        REQUIRE(*position.symbol == *order1.symbol);
    }

    SECTION("Apply BUY Order to Short Position 3") {
        price_t current_prince = 100;
        json j1 = {
                {"quantity",        200},
                {"symbol",          "BTC"},
                {"side",            "buy"},
                {"filled",          200},
                {"filled_at_price", 200},
                {"limit_price",     200},
                {"type",            "limit"},
                {"status",          "closed"}
        };
        Order order1(j1);
        json j2 = {
                {"quantity",        400},
                {"symbol",          "BTC"},
                {"side",            "buy"},
                {"filled",          400},
                {"filled_at_price", 50},
                {"limit_price",     0},
                {"type",            "market"},
                {"status",          "closed"}
        };
        Order order2(j2);
        json j3 = {
                {"quantity",        500},
                {"symbol",          "BTC"},
                {"side",            "buy"},
                {"filled",          500},
                {"filled_at_price", 150},
                {"limit_price",     0},
                {"type",            "market"},
                {"status",          "closed"}
        };
        Order order3(j3);
        Position position1;
        Position position2;
        Position position3;

        REQUIRE(position1.validate());
        position1.set_current_price(current_prince);

        REQUIRE(position2.validate());
        position2.set_current_price(current_prince);

        REQUIRE(position3.validate());
        position3.set_current_price(current_prince);

        Position::ApplyOrderResult result1 = position1.apply_order(order1);
        REQUIRE(position1.pnl == -20000);

        Position::ApplyOrderResult result2 = position2.apply_order(order2);
        REQUIRE(position2.pnl == 20000);

        Position::ApplyOrderResult result3 = position3.apply_order(order3);
        REQUIRE(position3.pnl == -25000);


        REQUIRE(position.validate());
        position.set_current_price(current_prince);
        Position::ApplyOrderResult result = position.apply_order(order1);
        REQUIRE(position.pnl == -20000);
        result = position.apply_order(order2);
        REQUIRE(position.pnl == 0);
        result = position.apply_order(order3);
        REQUIRE(-25000 == (int) position.pnl);


        REQUIRE(position.validate());
        REQUIRE(result.message.empty());
        REQUIRE(result.success);
        REQUIRE(position.balance == 1100);
        REQUIRE((int) position.entry_price == 122);
        REQUIRE(position.side == Side::LONG);
        REQUIRE(*position.symbol == *order1.symbol);
    }

}

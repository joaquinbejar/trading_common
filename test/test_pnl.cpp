//
// Created by Joaquin Bejar Garcia on 19/10/23.
//


#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <trading_common/order.h>
#include <trading_common/position.h>
#include <trading_common/pnl.h>
#include "nlohmann/json.hpp"


using json = nlohmann::json;
using Order = trading::order::Order;

using namespace trading::pnl;

TEST_CASE("PnL operations", "[PnL]") {
    PnL pnl;
    Position pos1;
    Position pos2;

    price_t current_prince_btc = 600;
    price_t current_prince_eth = 300;

    json j1 = {
            {"quantity",        200},
            {"symbol",          "BTC"},
            {"side",            "buy"},
            {"filled",          200},
            {"filled_at_price", 500},
            {"limit_price",     0},
            {"type",            "market"},
            {"status",          "closed"}
    };

    Order order(j1);
    REQUIRE(pos1.validate());
    pos1.set_current_price(current_prince_btc);
    Position::ApplyOrderResult result = pos1.apply_order(order);
    REQUIRE(pos1.validate());
    REQUIRE(pos1.pnl == 20000);

    json j2 = {
            {"quantity",        100},
            {"symbol",          "ETH"},
            {"side",            "buy"},
            {"filled",          100},
            {"filled_at_price", 200},
            {"limit_price",     0},
            {"type",            "market"},
            {"status",          "closed"}
    };
    Order order2(j2);
    REQUIRE(pos2.validate());
    pos2.set_current_price(current_prince_eth);
    Position::ApplyOrderResult result2 = pos2.apply_order(order2);
    REQUIRE(pos2.validate());
    REQUIRE(pos2.pnl == 10000);


    SECTION("Add and calculate total value") {

        pnl.add_position(pos1);
        pnl.add_position(pos2);
        pnl.add_cash(1000);

        REQUIRE(pnl.calculate_total_value() == 31000);
    }

    SECTION("Delete position and recalculate total value") {
        pnl.add_position(pos1);
        pnl.add_position(pos2);
        pnl.add_cash(2000);

        pnl.delete_position("ETH");

        REQUIRE(pnl.calculate_total_value() == 22000);
    }


}

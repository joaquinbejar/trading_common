//
// Created by Joaquin Bejar Garcia on 3/1/24.
//

#ifndef TRADING_COMMON_POSITION_H
#define TRADING_COMMON_POSITION_H


#include <cstdlib>
//#include <string>
#include <common/common.h>
#include <trading_common/common.h>
#include <trading_common/ohlc.h>
#include <trading_common/order.h>

using Result = trading::common::Result;
using namespace trading::common;

namespace trading::position {

    enum class Side {
        NONE = 0,
        LONG = 1,
        SHORT = 2
    };



    class Position {
    public:
        struct ApplyOrderResult : public Result {
            price_t pnl = 0;
        };

        id_t_ id = ::common::key_generator();
        timestamp_t timestamp = ::common::dates::get_unix_timestamp();
        size_t balance = 0;
        symbol_t symbol = std::make_shared<symbol_value_t>();
        Side side = Side::NONE;
        price_t entry_price = 0;
        price_t current_price = 0;
        price_t pnl = 0;

        Position() = default;

        Position(id_t_ id, timestamp_t timestamp, size_t balance, symbol_t symbol, Side side, price_t entry_price,
                 price_t current_price, price_t pnl);

        explicit Position(json &j);

        bool validate() const;

        void set_current_price(price_t cp);

        [[nodiscard]] price_t get_pnl() const;

        [[nodiscard]] json to_json() const;

        ApplyOrderResult apply_order(const trading::order::Order &order);

    private:
        Position::ApplyOrderResult apply_order_for_empty_position(const trading::order::Order &order);

        Position::ApplyOrderResult apply_order_for_long_position(const trading::order::Order &order);

        Position::ApplyOrderResult apply_order_for_short_position(const trading::order::Order &order);

        static Position::ApplyOrderResult validateOrder(const trading::order::Order &order);

    };

}
#endif //TRADING_COMMON_POSITION_H

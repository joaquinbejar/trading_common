//
// Created by Joaquin Bejar Garcia on 3/1/24.
//

#ifndef TRADING_COMMON_ORDER_H
#define TRADING_COMMON_ORDER_H

#include <cstdlib>
#include <string>
#include <common/common.h>
#include <trading_common/common.h>
#include <trading_common/ohlc.h>

using OHLC = trading::common::OHLC;
using OHLCV = trading::common::OHLCV;
using namespace trading::common;

namespace trading::order {

    class OrderException : public std::exception {
    private:
        std::string message{};
    public:
        explicit OrderException(std::string message);

        [[nodiscard]] const char *what() const noexcept override;
    };

    struct ValidateResult {
        bool success = false;
        std::string message{};
    };

    enum class Side {
        NONE = 0,
        BUY = 1,
        SELL = 2
    };

    enum class Type {
        NONE = 0,
        MARKET = 1,
        LIMIT = 2
    };

    enum class Status {
        NONE = 0,
        OPEN = 1,
        CLOSED = 2,
        FILLED = 3,
        CANCELED = 4
    };


    struct Order {
        id_t_ id = ::common::key_generator();
        timestamp_t timestamp = ::common::dates::get_unix_timestamp();
        size_t quantity = 0;
        symbol_t symbol = std::make_shared<std::string>();
        Side side = Side::NONE;
        size_t filled = 0;
        price_t filled_at_price = 0;
        price_t limit_price = 0;

        Type type = Type::NONE;
        Status status = Status::NONE;

        Order() = default;

        Order(timestamp_t timestamp, size_t quantity, symbol_t symbol, Side side, size_t filled,
              price_t filled_at_price,
              price_t limit_price, id_t_ id, Type type, Status status);

        explicit Order(json &j);

        void check_match_price(OHLC &ohlc);

        void check_match_price(OHLCV &ohlc);

        ValidateResult validate() const;

        [[nodiscard]] json to_json() const;

    private:
        bool is_empty_order() const;

        ValidateResult check_symbol() const;

        ValidateResult check_quantity() const;


    };

}
#endif //TRADING_COMMON_ORDER_H

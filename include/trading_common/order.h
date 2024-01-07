//
// Created by Joaquin Bejar Garcia on 3/1/24.
//

#ifndef TRADING_COMMON_ORDER_H
#define TRADING_COMMON_ORDER_H

#include <cstdlib>
#include <string>
#include <common/common.h>

namespace trading::order {
    typedef long long timestamp_t;
    typedef std::shared_ptr<std::string> symbol_t;
    typedef std::string id_t;
    typedef double price_t;

    class OrderException : public std::exception {
    private:
        std::string message{};
    public:
        explicit OrderException(std::string message);

        [[nodiscard]] const char *what() const noexcept override;
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


    struct Order {
        timestamp_t timestamp = 0;
        size_t quantity = 0;
        symbol_t symbol = std::make_shared<std::string>();
        Side side = Side::NONE;
        size_t filled = 0;
        price_t price = 0;
        price_t limit_price = 0;
        id_t id = ::common::key_generator();
        Type type = Type::NONE;
        Order() = default;

        Order(timestamp_t timestamp, size_t quantity, symbol_t symbol, Side side, size_t filled, price_t price,
              price_t limit_price, id_t id, Type type);

        explicit Order(json &j);

        [[nodiscard]] json to_json() const;
    };

}
#endif //TRADING_COMMON_ORDER_H

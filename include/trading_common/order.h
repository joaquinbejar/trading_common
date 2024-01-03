//
// Created by Joaquin Bejar Garcia on 3/1/24.
//

#ifndef TRADING_COMMON_ORDER_H
#define TRADING_COMMON_ORDER_H

#include <cstdlib>
#include <string>
#include <common/common.h>

namespace trading::order {
    class OHLCException : public std::exception {
    private:
        std::string message{};
    public:
        explicit OHLCException(std::string message);

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
        size_t timestamp = 0;
        size_t quantity = 0;
        std::string symbol{};
        Side side = Side::NONE;
        size_t filled = 0;
        double price = 0;
        double limit_price = 0;
        std::string id = ::common::key_generator();
        Type type = Type::NONE;
        Order() = default;

        Order(json &j);

        json to_json() const;
    };

}
#endif //TRADING_COMMON_ORDER_H

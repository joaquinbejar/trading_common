//
// Created by Joaquin Bejar Garcia on 3/1/24.
//

#include <trading_common/order.h>

namespace trading::order {

    OHLCException::OHLCException(std::string message) : message(std::move(message)) {}

    [[nodiscard]] const char *OHLCException::what() const noexcept {
        return message.c_str();
    }

    Order::Order(json &j) {
        try {
            timestamp = j["timestamp"];
            quantity = j["quantity"];
            symbol = j["symbol"];
            filled = j["filled"];
            price = j["price"];
            limit_price = j["limit_price"];
            id = j["id"];
            std::string side_str = ::common::to_upper(j["side"]);
            if (side_str == "BUY") {
                side = Side::BUY;
            } else if (side_str == "SELL") {
                side = Side::SELL;
            } else {
                side = Side::NONE;
            }
            std::string type_str = ::common::to_upper(j["type"]);
            if (type_str == "MARKET") {
                type = Type::MARKET;
            } else if (type_str == "LIMIT") {
                type = Type::LIMIT;
            } else {
                type = Type::NONE;
            }
        } catch (json::exception &e) {
            throw OHLCException("Error parsing OHLC json: " + std::string(e.what()));
        }
    }

    json Order::to_json() const {
        json j;
        j["timestamp"] = timestamp;
        j["quantity"] = quantity;
        j["symbol"] = symbol;
        j["filled"] = filled;
        j["price"] = price;
        j["limit_price"] = limit_price;
        j["id"] = id;
        switch (side) {
            case Side::BUY:
                j["side"] = "BUY";
                break;
            case Side::SELL:
                j["side"] = "SELL";
                break;
            default:
                j["side"] = "NONE";
                break;
        }
        switch (type) {
            case Type::MARKET:
                j["type"] = "MARKET";
                break;
            case Type::LIMIT:
                j["type"] = "LIMIT";
                break;
            default:
                j["type"] = "NONE";
                break;
        }
        return j;
    }

}
//
// Created by Joaquin Bejar Garcia on 3/1/24.
//

#include <trading_common/order.h>

#include <utility>

namespace trading::order {

    OrderException::OrderException(std::string message) : message(std::move(message)) {}

    [[nodiscard]] const char *OrderException::what() const noexcept {
        return message.c_str();
    }

    Order::Order(timestamp_t timestamp, size_t quantity, symbol_t symbol, Side side, size_t filled,
                 price_t filled_at_price, price_t limit_price, id_t id, Type type) : timestamp(timestamp),
                                                                                     quantity(quantity),
                                                                                     symbol(std::move(symbol)),
                                                                                     side(side), filled(filled),
                                                                                     filled_at_price(filled_at_price),
                                                                                     limit_price(limit_price),
                                                                                     id(std::move(id)), type(type) {}

    Order::Order(json &j) {
        try {
            timestamp = j.at("timestamp").get<timestamp_t>();
            quantity = j.at("quantity").get<size_t>();
            *symbol = j.at("symbol").get<std::string>();
            filled = j.at("filled").get<size_t>();
            filled_at_price = j.at("filled_at_price").get<price_t>();
            limit_price = j.at("limit_price").get<price_t>();
            id = j.at("id").get<id_t>();
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
            throw OrderException("Error parsing Order json: " + std::string(e.what()));
        }
    }

    void Order::check_match_price(OHLC &ohlc) {
        if (ohlc.low <= this->limit_price <= ohlc.high && type == Type::LIMIT) {
            this->filled_at_price = this->limit_price;
        }
    }

    void Order::check_match_price(OHLCV &ohlc) {
        if (ohlc.low <= this->limit_price <= ohlc.high && type == Type::LIMIT) {
            this->filled_at_price = this->limit_price;
            if (ohlc.volume >= this->quantity) {
                this->filled = this->quantity;
            } else {
                this->filled = ohlc.volume;
            }
        }
    }

    json Order::to_json() const {
        json j;
        j["timestamp"] = timestamp;
        j["quantity"] = quantity;
        j["symbol"] = *symbol;
        j["filled"] = filled;
        j["filled_at_price"] = filled_at_price;
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
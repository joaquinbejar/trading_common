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

    Order::Order(timestamp_t timestamp, size_t quantity, symbol_t symbol, Side side,
                 size_t filled, price_t filled_at_price, price_t limit_price, id_t id,
                 Type type, Status status) : timestamp(timestamp),
                                             quantity(quantity),
                                             symbol(std::move(symbol)),
                                             side(side),
                                             filled(filled),
                                             filled_at_price(filled_at_price),
                                             limit_price(limit_price),
                                             id(std::move(id)),
                                             type(type),
                                             status(status) {}

    Order::Order(json &j) {
        try {
            if (j.contains("timestamp") && j["timestamp"].is_number()) {
                timestamp = j.at("timestamp").get<timestamp_t>();
            }
            if (j.contains("id") && j["id"].is_string()) {
                id = j.at("id").get<id_t>();
            }

            quantity = j.at("quantity").get<size_t>();
            *symbol = j.at("symbol").get<std::string>();
            filled = j.at("filled").get<size_t>();
            filled_at_price = j.at("filled_at_price").get<price_t>();
            limit_price = j.at("limit_price").get<price_t>();

            std::string side_str = ::common::to_upper(j["side"]);
            if (side_str == "BUY") {
                side = Side::BUY;
            } else if (side_str == "SELL") {
                side = Side::SELL;
            } else {
                side = Side::NONE;
            }
            std::string type_str = ::common::to_upper(j.at("type").get<std::string>());
            if (type_str == "MARKET") {
                type = Type::MARKET;
            } else if (type_str == "LIMIT") {
                type = Type::LIMIT;
            } else {
                type = Type::NONE;
            }
            std::string status_str = ::common::to_upper(j.at("status").get<std::string>());
            if (status_str == "OPEN") {
                status = Status::OPEN;
            } else if (status_str == "CLOSED") {
                status = Status::CLOSED;
            } else if (status_str == "FILLED") {
                status = Status::FILLED;
            } else if (status_str == "CANCELED") {
                status = Status::CANCELED;
            } else {
                status = Status::NONE;
            }
        } catch (json::exception &e) {
            throw OrderException("Error parsing Order json: " + std::string(e.what()));
        }
    }

    void Order::check_match_price(OHLC &ohlc) {
        if (ohlc.low <= this->limit_price <= ohlc.high && type == Type::LIMIT) {
            this->filled_at_price = this->limit_price;
            status = Status::FILLED;
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
            status = Status::FILLED;
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
        switch (status) {
            case Status::OPEN:
                j["status"] = "OPEN";
                break;
            case Status::CLOSED:
                j["status"] = "CLOSED";
                break;
            case Status::FILLED:
                j["status"] = "FILLED";
                break;
            case Status::CANCELED:
                j["status"] = "CANCELED";
                break;
            default:
                j["status"] = "NONE";
                break;
        }
        return j;
    }

    ValidateResult Order::validate() const {
        ValidateResult result;
        if (timestamp > 0
            && quantity == 0
            && symbol->empty()
            && side == Side::NONE
            && filled == 0
            && filled_at_price == 0
            && limit_price == 0
            && !id.empty()
            && type == Type::NONE
            && status == Status::NONE) {
            result.success = true;
        }

        if (quantity == 0) {
            result.success = false;
            result.message = "Quantity is 0";
        }
        if (symbol == nullptr) {
            result.success = false;
            result.message = "Symbol is null";
        }
        if (symbol->empty()) {
            result.success = false;
            result.message = "Symbol is empty";
        }
        if (side == Side::NONE) {
            result.success = false;
            result.message = "Side is NONE";
        }
        if (type == Type::NONE) {
            result.success = false;
            result.message = "Type is NONE";
        }
        if (status == Status::NONE) {
            result.success = false;
            result.message = "Status is NONE";
        }

        if ( type == Type::LIMIT && limit_price == 0) {
            result.success = false;
            result.message = "Type is LIMIT but limit_price is 0";
        }

        if (limit_price != 0 && type != Type::LIMIT) {
            result.success = false;
            result.message = "Type is not LIMIT but limit_price is not 0";
        }

        if (filled != 0 && filled_at_price == 0) {
            result.success = false;
            result.message = "Filled is not 0 but filled_at_price is 0";
        }

        if (filled_at_price != 0 && filled == 0) {
            result.success = false;
            result.message = "Filled_at_price is not 0 but filled is 0";
        }

        switch (status) {
            case Status::OPEN:
                if (filled != 0) {
                    result.success = false;
                    result.message = "Status is OPEN but filled is not 0";
                }
                break;
            case Status::CLOSED:
                break;
            case Status::FILLED:
                if (filled == 0) {
                    result.success = false;
                    result.message = "Status is FILLED but filled is 0";
                }
                break;
            case Status::CANCELED:
                break;
            default:
                result.success = false;
                result.message = "Status is NONE";

        }
        result.success = true;
        return result;
    }
}
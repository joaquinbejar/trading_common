//
// Created by Joaquin Bejar Garcia on 3/1/24.
//

#include <trading_common/position.h>

#include <utility>

namespace trading::position {

    Position::Position(id_t id, timestamp_t timestamp, size_t balance, symbol_t symbol, Side side, price_t entry_price,
                       price_t current_price, price_t pnl) : id(std::move(id)), timestamp(timestamp), balance(balance),
                                                             symbol(std::move(symbol)),
                                                             side(side), entry_price(entry_price),
                                                             current_price(current_price),
                                                             pnl(pnl) {}

    Position::Position(json &j) {
        if (j.contains("timestamp") && j["timestamp"].is_number()) {
            timestamp = j.at("timestamp").get<timestamp_t>();
        }
        if (j.contains("id") && j["id"].is_string()) {
            id = j.at("id").get<id_t>();
        }

        balance = j.at("balance").get<size_t>();
        *symbol = j.at("symbol").get<std::string>();
        entry_price = j.at("entry_price").get<price_t>();
        current_price = j.at("current_price").get<price_t>();
        *symbol = j.at("symbol").get<std::string>();
        pnl = j.at("pnl").get<price_t>();

        std::string side_str = j.at("side").get<std::string>();
        if (side_str == "LONG") {
            this->side = Side::LONG;
        } else if (side_str == "SHORT") {
            this->side = Side::SHORT;
        } else {
            this->side = Side::NONE;
        }
    }

    bool Position::validate() const {
        if (timestamp > 0
            && balance == 0
            && symbol->empty()
            && side == Side::NONE
            && current_price == 0
            && entry_price == 0
            && !id.empty()) {
            return true;
        }
        if (side == Side::NONE)
            return false;
        if (symbol->empty())
            return false;
        if (balance != 0 && entry_price == 0)
            return false;

        return true;
    }

    void Position::set_current_price(price_t cp) {
        this->current_price = cp;
        this->pnl = get_pnl();
    }

    price_t Position::get_pnl() const {
        if (current_price == 0 && side != Side::NONE)
            throw std::runtime_error("Current price is 0");
        if (side == Side::LONG) {
            return (price_t) balance * (current_price - entry_price);
        } else if (side == Side::SHORT) {
            return (price_t) balance * (entry_price - current_price);
        } else {
            return 0;
        }
    }

    [[nodiscard]] json Position::to_json() const {
        json j;
        j["id"] = id;
        j["timestamp"] = timestamp;
        j["balance"] = balance;
        j["symbol"] = *symbol;
        j["entry_price"] = entry_price;
        j["current_price"] = current_price;
        j["pnl"] = pnl;

        switch (side) {
            case Side::LONG:
                j["side"] = "LONG";
                break;
            case Side::SHORT:
                j["side"] = "SHORT";
                break;
            default:
                j["side"] = "NONE";
                break;
        }
        return j;
    }

    Position::ApplyOrderResult Position::apply_order(const trading::order::Order &order) {

        ApplyOrderResult result;
        if (!order.validate().success) {
            if (order.filled == 0) {
                result.message = "Order is not filled";
            } else {
                result.message = "Order is not valid";
            }
            result.success = false;
            return result;
        } else {
            std::cout << order.validate().message << std::endl;
        }

        if (symbol->empty()) {
            symbol = order.symbol;
        } else if (*symbol != *order.symbol) {
            result.message = "Symbol is not the same" + *symbol + "!=" + *order.symbol;
            result.success = false;
            return result;
        }

        // position is empty
        if (balance == 0) {
            if (order.side == trading::order::Side::BUY) {
                this->balance = order.filled;
                entry_price = order.filled_at_price;
                side = Side::LONG;
                result.pnl = pnl = get_pnl();
                result.success = true;
            } else if (order.side == trading::order::Side::SELL) {
                this->balance = order.quantity;
                entry_price = order.filled_at_price;
                side = Side::SHORT;
                result.pnl = pnl = get_pnl();
                result.success = true;
            } else {
                result.message = "Invalid side";
                result.success = false;
            }
        } else if (side == Side::LONG) {
            if (order.side == trading::order::Side::BUY) {
                auto new_balance = balance + order.filled;
                entry_price = (entry_price * (price_t) balance + order.filled_at_price * (price_t) order.filled) /
                              (price_t) new_balance;
                balance = new_balance;
                result.pnl = pnl = get_pnl();
                result.success = true;
            } else if (order.side == trading::order::Side::SELL) {
                if (balance >= order.filled) {
                    auto new_balance = balance - order.filled;
                    entry_price = (entry_price * (price_t) balance - order.filled_at_price * (price_t) order.filled) /
                                  (price_t) order.filled;
                    balance = new_balance;
                    result.pnl = pnl = get_pnl();
                    result.success = true;
                } else {
                    auto new_balance = -(balance - order.filled);
                    price_t add_to_pnl = (order.filled_at_price - entry_price) * (price_t) balance;
                    entry_price =  order.filled_at_price;
                    balance = new_balance;
                    side = Side::SHORT;
                    result.pnl = pnl = get_pnl() + add_to_pnl;
                    result.success = true;
                }
            } else {
                result.message = "Invalid side";
                result.success = false;
            }
        } else if (side == Side::SHORT) {
            if (order.side == trading::order::Side::BUY) {
                if (balance >= order.filled) {
                    balance -= order.filled;
                    result.pnl = pnl = get_pnl();
                    result.success = true;
                } else {
                    balance = order.filled - balance;
                    entry_price = order.filled_at_price;
                    side = Side::LONG;
                    result.pnl = pnl = get_pnl();
                    result.success = true;
                }
            } else if (order.side == trading::order::Side::SELL) {
                auto new_balance = balance + order.filled;
                entry_price = (entry_price * (price_t) balance + order.filled_at_price * (price_t) order.filled) /
                              (price_t) new_balance;
                balance = new_balance;
                result.pnl = pnl = get_pnl();
                result.success = true;
            } else {
                result.message = "Invalid side";
                result.success = false;

            }
        } else {
            result.message = "Invalid side";
            result.success = false;
        }
        return result;
    }
}
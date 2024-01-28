//
// Created by Joaquin Bejar Garcia on 27/1/24.
//

#ifndef COMMON_INSTRUCTIONS_H
#define COMMON_INSTRUCTIONS_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <common/common.h>

namespace common::instructions {

    using json = nlohmann::json;
    using common::find_key_from_value;
    typedef std::string type_t;
    typedef std::string selector_t;

    enum class Type {
        NONE = 0,
        TICKER = 1,
        OHLC = 2,
        MACD = 3,
        SMA = 4,
        EMA = 5
    };

    const std::map<Type, type_t> TypeNames = {
            {Type::NONE,   ""},
            {Type::TICKER, "ticker"},
            {Type::OHLC,   "ohlc"},
            {Type::MACD,   "macd"},
            {Type::SMA,    "sma"},
            {Type::EMA,    "ema"}
    };

    type_t get_type_name(Type type);

    Type get_type_from_string(const type_t &type);

    enum class Selector {
        NONE = 0,
        ALL = 1,
        ONE = 2,
        SET = 3
    };

    const std::map<Selector, selector_t> SelectorNames = {
            {Selector::NONE, ""},
            {Selector::ALL,  "all"},
            {Selector::ONE,  "one"},
            {Selector::SET,  "set"}
    };

    selector_t get_selector_name(Selector selector);

    Selector get_selector_from_string(const selector_t &selector);

    template<typename T>
    concept JsonSerializable = requires(T t, const nlohmann::json &j) {
        { t.to_json() } -> std::same_as<nlohmann::json>;
        { t.from_json(j) } -> std::same_as<void>;
    };

    template<typename T> requires JsonSerializable<T>
    struct Instructions {
        Type type = Type::NONE;
        Selector selector = Selector::NONE;
        std::vector<std::string> tickers;
        T other;

        [[nodiscard]] json to_json() const {
            json result;
            result["type"] = get_type_name(type);
            result["selector"] = get_selector_name(selector);
            result["tickers"] = tickers;
            result["other"] = other.to_json();
            return result;
        }

        void from_json(const json &j) {
            type = get_type_from_string(j["type"].get<std::string>());
            selector = get_selector_from_string(j["selector"].get<std::string>());
            tickers = j["tickers"].get<std::vector<std::string>>();
            if (j.contains("other"))
                other.from_json(j["other"]);
        }

        operator std::string() const {
            return to_string();
        }

        [[nodiscard]] std::string to_string() const {
            return to_json().dump();
        }

        void from_string(const std::string &s) {
            from_json(json::parse(s));
        }
    };


}

#endif //COMMON_INSTRUCTIONS_H

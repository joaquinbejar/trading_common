//
// Created by Joaquin Bejar Garcia on 3/1/24.
//

#ifndef TRADING_COMMON_OHLC_H
#define TRADING_COMMON_OHLC_H

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace trading::common {

    class OHLCException : public std::exception {
    private:
        std::string message{};
    public:
        explicit OHLCException(std::string message) : message(std::move(message)) {}

        [[nodiscard]] const char *what() const noexcept override {
            return message.c_str();
        }
    };

    struct OHLC {
        double open{};
        double high{};
        double low{};
        double close{};

        OHLC();

        OHLC(const json &j);

        OHLC(double open, double high, double low, double close);

        [[nodiscard]] json to_json() const;

    };

    struct HeikinAshi : public OHLC {

        HeikinAshi(const OHLC &current, const OHLC &previous);

        HeikinAshi(const OHLC &current);

    };

}

#endif //TRADING_COMMON_OHLC_H

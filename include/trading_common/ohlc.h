//
// Created by Joaquin Bejar Garcia on 3/1/24.
//

#ifndef TRADING_COMMON_OHLC_H
#define TRADING_COMMON_OHLC_H

#include "nlohmann/json.hpp"
#include <sstream>

using json = nlohmann::json;

namespace trading::common {

    std::string epoch_to_date_string(long long epoch); // TODO: moved to common

    class OHLCException : public std::exception {
    private:
        std::string message{};
    public:
        explicit OHLCException(std::string message) : message(std::move(message)) {}

        [[nodiscard]] const char *what() const noexcept override {
            return message.c_str();
        }
    };

    typedef long long timestamp_t;
    typedef std::string date_t;

    struct Timestamp {
        timestamp_t timestamp = 0;
        date_t date{};

        Timestamp() = default;

        explicit Timestamp(timestamp_t timestamp);

        explicit Timestamp(const json &j);

        [[nodiscard]] virtual json to_json() const ;
    };

    struct OHLC {
        double open{};
        double high{};
        double low{};
        double close{};

        OHLC();

        explicit OHLC(const json &j);

        OHLC(double open, double high, double low, double close);

        [[nodiscard]] virtual json to_json() const;

    };

    struct OHLCV : public Timestamp, public OHLC {
        size_t volume{};

        OHLCV();

        explicit OHLCV(const json &j);

        OHLCV(timestamp_t timestamp, double open, double high, double low, double close, size_t volume);

        [[nodiscard]] json to_json() const override;

    };



    struct HeikinAshi : public OHLC {

        explicit HeikinAshi(const OHLC &current, const OHLC &previous);

        explicit HeikinAshi(const OHLC &current);

    };

}

#endif //TRADING_COMMON_OHLC_H

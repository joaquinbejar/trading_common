//
// Created by Joaquin Bejar Garcia on 3/1/24.
//

#ifndef TRADING_COMMON_OHLC_H
#define TRADING_COMMON_OHLC_H

#include "nlohmann/json.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <common/dates.h>

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
    typedef std::shared_ptr<std::string> symbol_t;

    struct Symbol {
        symbol_t symbol{};

        Symbol();

        explicit Symbol(symbol_t symbol);

        explicit Symbol(const json &j);

        [[nodiscard]] virtual json to_json() const;

    };


    struct Timestamp {
        timestamp_t timestamp = 0;
        date_t date{};

        Timestamp() = default;

        explicit Timestamp(timestamp_t timestamp);

        explicit Timestamp(const json &j);

        [[nodiscard]] virtual json to_json() const;
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

    struct OHLCV : public Symbol, public Timestamp, public OHLC {
        size_t volume{};

        OHLCV();

        explicit OHLCV(const json &j);

        OHLCV(symbol_t symbol, timestamp_t timestamp, double open, double high, double low, double close,
              size_t volume);

        [[nodiscard]] json to_json() const override;

    };


    struct HeikinAshi : public OHLC {

        explicit HeikinAshi(const OHLC &current, const OHLC &previous);

        explicit HeikinAshi(const OHLC &current);

    };

    class SeriesOHLCV {
    private:
        std::map<timestamp_t, OHLCV> m_data;
        mutable std::mutex m_mutex;

    public:
        SeriesOHLCV() = default;

        explicit SeriesOHLCV(const json &j);

        [[nodiscard]] json to_json() const;

        bool empty();

        [[nodiscard]] size_t size();

        bool insert(const OHLCV &ohlc);

        bool insert(const SeriesOHLCV &ohlc);

        OHLCV &operator[](timestamp_t timestamp);

        OHLCV &operator[](const std::string &date);

        class iterator {
        private:
            std::map<timestamp_t, OHLCV>::const_iterator iter;
            const SeriesOHLCV *series;
            bool isEnd;

        public:
            iterator(std::map<timestamp_t, OHLCV>::const_iterator it, const SeriesOHLCV *s, bool end = false);

            ~iterator() ;

            iterator &operator++() ;

            iterator &operator--() ;

            bool operator!=(const iterator &other) const ;

            const std::pair<const timestamp_t, OHLCV> &operator*() const;

        };

        iterator begin() const ;

        iterator rbegin() const ;

        iterator end() const ;

        iterator rend() const ;

    };
}

#endif //TRADING_COMMON_OHLC_H

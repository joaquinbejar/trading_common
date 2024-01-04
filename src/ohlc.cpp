//
// Created by Joaquin Bejar Garcia on 3/1/24.
//

#include <trading_common/ohlc.h>

namespace trading::common {

    std::string epoch_to_date_string(long long epoch) {
        if (epoch > 10000000000) {
            epoch /= 1000;
        }

        std::chrono::time_point<std::chrono::system_clock> tp = std::chrono::system_clock::from_time_t(epoch);

        std::time_t time = std::chrono::system_clock::to_time_t(tp);
        std::tm *tm_ptr = std::localtime(&time);

        std::ostringstream oss;
        oss << std::put_time(tm_ptr, "%Y-%m-%d");

        return oss.str();
    }

    OHLC::OHLC() : open(0), high(0), low(0), close(0) {}

    OHLC::OHLC(const json &j) {
        try {
            open = j.at("open").get<double>();
            high = j["high"];
            low = j["low"];
            close = j["close"];
        } catch (json::exception &e) {
            throw OHLCException("Error parsing OHLC json: " + std::string(e.what()));
        }
    }

    OHLC::OHLC(double open, double high, double low, double close) : open(open), high(high), low(low), close(close) {}

    json OHLC::to_json() const {
        json j;
        j["open"] = open;
        j["high"] = high;
        j["low"] = low;
        j["close"] = close;
        return j;
    }

    OHLCV::OHLCV() : Timestamp(), OHLC(), volume(0) {}

    OHLCV::OHLCV(const json &j) : Timestamp(j), OHLC(j) {
        try {
            volume = j.at("volume").get<size_t>();
        } catch (json::exception &e) {
            throw OHLCException("Error parsing OHLC json: " + std::string(e.what()));
        }
    }

    OHLCV::OHLCV(timestamp_t timestamp, double open, double high, double low, double close, size_t volume) : Timestamp(
            timestamp), OHLC(open, high, low, close), volume(volume) {}

    json OHLCV::to_json() const {
        json j = Timestamp::to_json();
        j.merge_patch(OHLC::to_json());
        j["volume"] = volume;
        return j;
    }


    HeikinAshi::HeikinAshi(const OHLC &current, const OHLC &previous) {
        close = (current.open + current.high + current.low + current.close) / 4;
        open = (previous.open + previous.close) / 2;
        high = std::max({current.high, open, close});
        low = std::min({current.low, open, close});
    }

    HeikinAshi::HeikinAshi(const OHLC &current) {
        close = (current.open + current.high + current.low + current.close) / 4;
        // for the first candle, use the traditional open price
        open = current.open;
        high = std::max({current.high, open, close});
        low = std::min({current.low, open, close});
    }

}
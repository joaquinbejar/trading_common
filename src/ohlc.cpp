//
// Created by Joaquin Bejar Garcia on 3/1/24.
//

#include <trading_common/ohlc.h>

namespace trading::common {

    OHLC::OHLC() : open(0), high(0), low(0), close(0) {}

    OHLC::OHLC(const json &j) {
        try {
            open = j["open"];
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
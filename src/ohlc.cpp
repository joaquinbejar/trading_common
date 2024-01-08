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

    Symbol::Symbol() : symbol(std::make_shared<std::string>()) {}

    Symbol::Symbol(symbol_t symbol) : symbol(std::move(symbol)) {}

    Symbol::Symbol(const json &j) : symbol(std::make_shared<std::string>()) {
        try {
            *symbol = j.at("symbol").get<std::string>();
        } catch (json::exception &e) {
            throw OHLCException("Error parsing OHLC json: " + std::string(e.what()));
        }
    }

    json Symbol::to_json() const {
        return {{"symbol", *symbol}};
    }


    Timestamp::Timestamp(timestamp_t timestamp) : timestamp(std::move(timestamp)) {
        date = epoch_to_date_string(timestamp);
    }

    Timestamp::Timestamp(const json &j) {
        try {
            timestamp = j.at("timestamp").get<timestamp_t>();
        } catch (json::exception &e) {
            throw OHLCException("Error parsing OHLC json: " + std::string(e.what()));
        }
    }

    json Timestamp::to_json() const {
        json j;
        j["timestamp"] = timestamp;
        return j;
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

    OHLCV::OHLCV(symbol_t symbol, timestamp_t timestamp, double open, double high, double low, double close,
                 size_t volume) : Symbol(symbol), Timestamp(
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

    SeriesOHLCV::SeriesOHLCV(const json &j) {
        try {
            for (auto &item: j) {
                OHLCV ohlcv(item);
                m_data.insert({ohlcv.timestamp, ohlcv});
            }
        } catch (json::exception &e) {
            throw OHLCException("Error parsing OHLC json: " + std::string(e.what()));
        }
    }

    json SeriesOHLCV::to_json() const {
        json j;
        for (auto &item: m_data) {
            j.push_back(item.second.to_json());
        }
        return j;
    }

    bool SeriesOHLCV::empty() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_data.empty();
    }

    size_t SeriesOHLCV::size() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_data.size();
    }

    bool SeriesOHLCV::insert(const OHLCV &ohlc) {
        try {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_data.insert({ohlc.timestamp, ohlc});
            return true;
        } catch (std::exception &e) {
            return false;
        }
    }

    bool SeriesOHLCV::insert(const SeriesOHLCV &ohlc) {
        try {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_data.insert(ohlc.m_data.begin(), ohlc.m_data.end());
            return true;
        } catch (std::exception &e) {
            return false;
        }
    }

    OHLCV &SeriesOHLCV::operator[](timestamp_t timestamp) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_data[timestamp];
    }


    OHLCV &SeriesOHLCV::operator[](const std::string &date) {
        // check date format with a regex
        if (!::common::dates::is_valid_date_format(date)) {
            throw OHLCException("Invalid date format: " + date);
        }

        std::istringstream ss{date};
        std::tm tm{};
        ss >> std::get_time(&tm, "%Y-%m-%d"); // Here is the correct way to parse a date string

        // convert struct tm to time_t and then to system_clock::time_point
        auto timePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));

        // use `std::chrono::system_clock::to_time_t` to convert time_point to time_t
        auto tt = std::chrono::system_clock::to_time_t(timePoint);
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_data[tt];
        }
    }

    SeriesOHLCV::iterator::iterator(std::map<timestamp_t, OHLCV>::const_iterator it, const SeriesOHLCV *s, bool end)
            : iter(it), series(s), isEnd(end) {}

    SeriesOHLCV::iterator::~iterator() {
        if (!isEnd) {
            series->m_mutex.unlock();
        }
    }

    SeriesOHLCV::iterator &SeriesOHLCV::iterator::operator++() {
        ++iter;
        return *this;
    }

    SeriesOHLCV::iterator &SeriesOHLCV::iterator::operator--() {
        --iter;
        return *this;
    }

    bool SeriesOHLCV::iterator::operator!=(const iterator &other) const {
        return iter != other.iter;
    }

    const std::pair<const timestamp_t, OHLCV> &SeriesOHLCV::iterator::operator*() const {
        return *iter;
    }


    SeriesOHLCV::iterator SeriesOHLCV::begin() const {
        m_mutex.lock();
        return {m_data.begin(), this};
    }


    SeriesOHLCV::iterator SeriesOHLCV::end() const {
        return {m_data.end(), this, true};
    }

    SeriesOHLCV::iterator SeriesOHLCV::rbegin() const {
        m_mutex.lock();
        return {--m_data.end(), this};
    }

    SeriesOHLCV::iterator SeriesOHLCV::rend() const {
        return {m_data.begin(), this, true};
    }

}
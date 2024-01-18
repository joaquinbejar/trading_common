//
// Created by Joaquin Bejar Garcia on 3/1/24.
//

#ifndef TRADING_COMMON_COMMON_H
#define TRADING_COMMON_COMMON_H

#include <cstdlib>
#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace trading::common {
    typedef double price_t;
    typedef std::string symbol_value_t;
    typedef std::shared_ptr<symbol_value_t> symbol_t;
    typedef std::string date_t;
    typedef std::string id_t_;
    typedef unsigned long long timestamp_t;

    struct Result {
        bool success = false;
        std::string message{};
    };

};

#endif //TRADING_COMMON_COMMON_H
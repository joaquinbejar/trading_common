//
// Created by Joaquin Bejar Garcia on 3/1/24.
//

#ifndef TRADING_COMMON_PNL_H
#define TRADING_COMMON_PNL_H

#include <vector>
#include <memory>
#include <trading_common/common.h>
#include <trading_common/position.h>

namespace trading::pnl {

    using Position = trading::position::Position;

    class PnL {
    public:
        PnL() = default;

        void add_position(const Position& position) ;

        void add_position(const std::shared_ptr<Position>& position) ;

        void delete_position(const symbol_t& symbol);

        void delete_position(const symbol_value_t& symbol);

        void add_cash(price_t cashAmount);

        [[nodiscard]] price_t calculate_total_value() const;

    private:
        std::map<symbol_value_t, std::shared_ptr<Position>> positions;
        price_t cash = 0;
    };
}
#endif //TRADING_COMMON_PNL_H

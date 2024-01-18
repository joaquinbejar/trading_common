//
// Created by Joaquin Bejar Garcia on 3/1/24.
//
#include <trading_common/pnl.h>

namespace trading::pnl {

    void PnL::add_position(const Position& position) {
        this->add_position(std::make_shared<Position>(position));
    }

    void PnL::add_position(const std::shared_ptr<Position>& position) {
        positions[*position->symbol] = position;
    }

    void PnL::delete_position(const symbol_t& symbol) {
        positions.erase(*symbol);
    }

    void PnL::delete_position(const symbol_value_t& symbol) {
        positions.erase(symbol);
    }

    void PnL::add_cash(price_t cashAmount) {
        cash += cashAmount;
    }

    price_t PnL::calculate_total_value() const  {
        price_t totalValue = cash;
        for (const auto& [symbol, position] : positions) {
            totalValue += position->get_pnl();
        }
        return totalValue;
    }

}
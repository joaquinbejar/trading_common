//
// Created by Joaquin Bejar Garcia on 27/1/24.
//
#include <trading_common/instructions.h>

namespace trading::instructions {

    type_t get_type_name(Type type) {
        return TypeNames.at(type);
    }

    Type get_type_from_string(const type_t &type){
        return find_key_from_value(TypeNames, type, Type::NONE);
    }

    selector_t get_selector_name(Selector selector) {
        return SelectorNames.at(selector);
    }

    Selector get_selector_from_string(const selector_t &selector){
        return find_key_from_value(SelectorNames, selector, Selector::NONE);
    }
}

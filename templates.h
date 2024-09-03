#pragma once

namespace cons {
    template<typename DT>
    struct converter {
        DT value;
        converter(DT value) {
            this->value = value;
        }
        template<typename U>
        explicit operator converter<U>() {
            return static_cast<U>(this->value);
        }
    };
}
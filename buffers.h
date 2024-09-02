#include "functions.h"

namespace cons {
    template<typename BT>
    struct buffer_rw : public dim_prov, public i_buffer_sink<BT>, public i_buffer_source<BT> {
        buffer_rw(con_size width, con_size height) : dim_prov(width, height) {
            buffer = new BT[get(width, height)];
        }

        ~buffer_rw() {
            delete[] buffer;
        }

        BT* buffer;

        void write(con_norm x, con_norm y, BT value) override {
            buffer[get(x, y)] = value;
        }

        BT read(con_norm x, con_norm y) const override {
            return buffer[get(x, y)];
        }

        void copyTo(buffer_rw<BT>* buffer) {
            cons::copyTo(this, buffer);
        }
    };

    typedef buffer_rw<con_basic> buffer_basic;
    typedef buffer_rw<con_wide> buffer_wide;
    typedef buffer_rw<con_color> buffer_color;
}
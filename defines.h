#pragma once

namespace cons {
    typedef int con_pos;
    typedef int con_size;
    typedef int con_basic_key;
    typedef int err_ret;
    typedef unsigned char con_color;
    typedef unsigned char con_ubyte;
    typedef char con_basic;
    typedef wchar_t con_wide;

    struct ENUMS {
        enum ERROR_TYPES {
            NO_ERROR = 0,
            ERROR = 1
        };
    };

    struct con_key {
        con_key(con_basic_key key) : key(key) {}
        con_basic_key key;
        con_ubyte modifiers;
        enum modifiers {
            SHIFT = 1,
            CTRL = 2,
            ALT = 4,
            META = 8
        };
    };

    struct con_type {
        const con_basic *name;
    };
}

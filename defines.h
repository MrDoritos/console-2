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
    typedef float con_norm;

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

    template<typename character, typename color, typename alpha>
    struct _cc {
        character ch;
        color co;
        alpha a;
    };

    typedef _cc<con_basic, con_color, con_color> cpix_basic;
    typedef _cc<con_wide, con_color, con_color> cpix_wide;

    template<typename _bit>
    struct _pixel {
        _pixel()
        :r(0), g(0), b(0), a(255) {}
        _pixel(_bit r, _bit g, _bit b)
        :_pixel(r, g, b, 255) {}
        _pixel(_bit r, _bit g, _bit b, _bit a)
        :r(r), g(g), b(b), a(a) {}
        _bit r,g,b,a;
    };

    typedef _pixel<con_ubyte> pixel;

    template<typename T>
    struct _2d {
        T x, y;
        _2d(T x, T y) : x(x), y(y) {}
        _2d() : x(0), y(0) {}
    };
}

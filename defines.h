#pragma once

#include <cstring>
#include <cmath>

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

    struct con_mouse {
        con_mouse() : x(0), y(0), z(0), state(0), id(0) {}
        con_pos x, y, z;
        int state;
        int id;

        enum MOUSE {
            BUTTON1 = 0b1,
            BUTTON2 = 0b10,
            BUTTON3 = 0b100,
            BUTTON4 = 0b1000,
            BUTTON5 = 0b10000,
            PRESSED = 0b100000,
            RELEASED= 0b1000000,
            CLICKED = 0b10000000,
            DOUBLE_CLICKED = 0b100000000,
            TRIPLE_CLICKED = 0b1000000000,
            CTRL = 0b10000000000,
            SHIFT = 0b100000000000,
            ALT = 0b1000000000000,
        };
    };

    struct con_type {
        const con_basic *name;
    };


    template<typename _bit>
    struct _pixel {
        typedef _bit value_type;
        static constexpr _bit bit_max { std::numeric_limits<_bit>::max() };
        _pixel()
        :r(0), g(0), b(0), a(bit_max) {}
        //_pixel(int dummy) : _pixel() {}
        _pixel(_bit r, _bit g, _bit b)
        :_pixel(r, g, b, bit_max) {}
        _pixel(_bit r, _bit g, _bit b, _bit a)
        :r(r), g(g), b(b), a(a) {}
        _bit r,g,b,a;
        _bit value() {
            return ((int(r) + g + b) / 3);
        }
        _bit luminance() {
            return ((r * 0.299) +
                    (g * 0.587) +
                    (b * 0.114));
        }
    };

    typedef _pixel<con_ubyte> pixel;

    template<typename T>
    T getCharacter(pixel color) {
        const char* values = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'.";

        //float value = color.value() / 255.0f;
        float value = 1-(color.luminance() / 255.0f);
        float a = color.a / 255.0f;

        if (a < 0.1f) return (T)' ';
        value *= a;

        return (T)values[int(value * strlen(values))];
    }

    template<typename character, typename color, typename alpha>
    struct _cc {
        static constexpr color color_max { std::numeric_limits<color>::max() };
        static const con_color default_color = 15;
        _cc() : ch(0), co(default_color), a(color_max) {}
        _cc(pixel pix) : _cc(pix,getCharacter<character>(pix)) {}
        _cc(pixel pix, character ch) : ch(ch), co(pix.value()), a(pix.a) {}
        _cc(character ch, color co, alpha a = color_max) : ch(ch), co(co), a(a) {}
        _cc(character ch) : ch(ch), co(default_color), a(color_max) {}
        character ch;
        color co;
        alpha a;
    };

    typedef _cc<con_basic, con_color, con_color> cpix_basic;
    typedef _cc<con_wide, con_color, con_color> cpix_wide;
    template<typename Tchar>
    using cpix_Tchar = _cc<Tchar, con_color, con_color>;

    template<typename T>
    struct _2dlength;

    template<typename T>
    struct _2d {
        T x, y;
        _2d(T _x, T _y) : x(_x), y(_y) {}
        _2d(_2dlength<T> len) : _2d(len.width, len.height) {}
        _2d() : _2d(0,0) {}
        //_2d(const i_op2<T> &iop) : _2d(iop._a, iop._b) {}

        T distance(_2d<T> other) {
            return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
        }

        T product() {
            return x * y;
        }

        _2d<T> operator+(const _2d<T> &other) const {
            return _2d<T>(x + other.x, y + other.y);
        }

        _2d<T> operator-(const _2d<T> &other) const {
            return _2d<T>(x - other.x, y - other.y);
        }

        _2d<T> operator*(const _2d<T> &other) const {
            return _2d<T>(x * other.x, y * other.y);
        }

        _2d<T> operator/(const _2d<T> &other) const {
            return _2d<T>(x / other.x, y / other.y);
        }

        template<typename I>
        operator _2d<I>() const {
            return _2d<I>(I(x), I(y));
        }

    };

    template<typename T>
    struct _2dlength {
        T width, height;
        _2dlength(T _width, T _height) : width(_width), height(_height) {}
        _2dlength() : _2dlength(0,0) {}
        //_2dlength(const i_op2<T> &iop) : _2dlength(iop._a, iop._b) {}
        _2dlength(const _2d<T> &pos) : _2dlength(pos.x, pos.y) {}

        template<typename I>
        operator _2dlength<I>() const {
            return _2dlength<I>(I(width), I(height));
        }

        operator _2d<T>() const {
            return _2d<T>(width, height);
        }

        operator const _2d<T>&() const {
            return *(const _2d<T>*)this;
        }

        _2dlength<T> operator+(const _2dlength<T> &other) const {
            return _2dlength<T>(width + other.width, height + other.height);
        }
    };

    template<typename T>
    struct _2dsize : _2d<T>, _2dlength<T> {
        _2dsize(const T &x, const T &y, const T &width, const T &height) : _2d<T>(x, y), _2dlength<T>(width, height) {}
        _2dsize(const T &width, const T &height) : _2dlength<T>(width,height) {}
        _2dsize() : _2d<T>(), _2dlength<T>() {}
        _2dsize(const _2d<T> &pos, _2d<T> const &len) : _2d<T>(pos), _2dlength<T>(len) {}

        typedef _2d<T> pos;
        typedef _2dlength<T> length;

        template<typename I>
        operator _2dsize<I>() const {
            return _2dsize<I>(I(pos::x), I(pos::y), I(length::width), I(length::height));
        }

        template<typename V>
        _2dsize<T> operator+(const V &other) const {
            if constexpr(std::is_same_v<_2dsize<T>, V>)
                return _2dsize<T>(pos::operator+(other.getStart()), length::operator+(other.getLength()));
            else
            if constexpr(std::is_same_v<_2d<T>, V>)
                return _2dsize<T>(pos::operator+(other), getLength());
            else
            if constexpr(std::is_same_v<_2dlength<T>, V>)
                return _2dsize<T>(getStart(), length::operator+(other));
            else
                return _2dsize<T>(pos::operator+(other), length::operator+(other));
        }

        template<typename IN>
        _2dsize<T> norm(const _2dsize<IN> &bound) const {
            const pos &p = getStart();
            const pos &len = getLength();
            const pos bp = bound.getStart();
            const pos blen = (pos)bound.getLength();
            return _2dsize<T>((p-bp) / blen, len / blen);
        }
        template<typename IN>
        _2dsize<T> denorm(const _2dsize<IN> &bound) const {
            const pos &p = getStart();
            const pos &len = (pos&)getLength();
            const pos bp = bound.getStart();
            const pos blen = (pos)bound.getLength();
            return _2dsize<T>((p*blen) + bp, len * blen);
        }

        const _2dlength<T> &getLength() const {
            return (const length&) *this;
        }

        _2dlength<T> getLength() {
            return (length&)*this;
        }

        const _2d<T> &getStart() const {
            return (const pos&) *this;
        }

        _2d<T> getStart() {
            return (pos&)*this;
        }
    };

    template<typename T>
    struct _2dpair {
        _2d<T> start, end;
    };

    typedef _2d<con_size> posi;
    typedef _2d<con_norm> posf;
    typedef _2dsize<con_size> sizei;
    typedef _2dsize<con_norm> sizef;
    typedef _2dpair<con_size> pairi;
    typedef _2dpair<con_norm> pairf;
}

#pragma once
#include "defines.h"

namespace cons {
    struct i_write {
        virtual ssize_t write(const con_basic* buf, size_t count) = 0;
        virtual void flush() {}
    };

    struct i_write_wide : public i_write {
        virtual ssize_t write(const con_wide* buf, size_t count) = 0;
    };

    struct i_seek {
        virtual void seek(con_pos pos) {}
        virtual con_pos tell() { return 0; }
    };

    struct i_write_seek : public i_write, public i_seek {
        virtual ssize_t write(const con_basic* buf, size_t start, size_t count) { return 0; }
    };

    struct i_read {
        virtual ssize_t read(con_basic* buf, size_t count) = 0;
    };

    struct i_read_seek : public i_read, public i_seek {
        virtual ssize_t read(con_basic* buf, size_t start, size_t count) = 0;
    };

    struct i_dimension {
        virtual con_size getWidth() = 0;
        virtual con_size getHeight() = 0;
    };

    struct i_dimension_set : public i_dimension {
        virtual void setWidth(con_size x) = 0;
        virtual void setHeight(con_size y) = 0;
    };

    struct i_keyboard {
        virtual con_basic_key readKey() = 0;
        virtual con_basic_key readKeyAsync() = 0;
    };

    struct i_cursor {
        virtual con_pos getCursorX() = 0;
        virtual con_pos getCursorY() = 0;
    };

    struct i_cursor_set : public i_cursor {
        virtual void setCursor(con_pos x, con_pos y) = 0;
    };

    struct i_color {
        virtual void setColor(con_color color) = 0;
    };

    struct i_other {
        virtual void sleep(int millis) {
            usleep(millis * 1000);
        }
    };

    struct i_state {
        virtual bool isReady() = 0;
        virtual err_ret open() = 0;
        virtual err_ret close() = 0;
    };

    struct i_console_basic_write :
    public i_cursor_set,
    public i_dimension,
    public i_write_seek,
    public i_other,
    public i_state {
        virtual void clear() = 0;
        virtual con_type getType() = 0;
        virtual ssize_t write(con_pos x, con_pos y, const con_basic* text) {
            setCursor(x, y);
            return write(text, strlen(text));
        }
        virtual ssize_t write(const con_basic* text) {
            return write(text, strlen(text));
        }
        ssize_t write(const con_basic* buf, size_t count) override {
            return write(buf, count);
        }
    };

    struct i_console_basic : 
    public i_console_basic_write,
    public i_write_wide,
    public i_keyboard {
        virtual ssize_t write(con_pos x, con_pos y, const con_wide* text) {
            setCursor(x, y);
            return write(text, wcslen(text));
        }
        virtual ssize_t write(const con_wide* text) {
            return write(text, wcslen(text));
        }
        virtual ssize_t write(const con_wide* text, size_t count) {
            return 0;
        }
    };

    template <typename T>
    struct automatic_console : public T {
        automatic_console() {
            T::open();
        }
        ~automatic_console() {
            T::close();
        }
    };
}
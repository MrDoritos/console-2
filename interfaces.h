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

    struct buffer_sink;
    struct buffer_source;

    struct i_norm_dimension {
        virtual con_norm getNormWidth() = 0;
        virtual con_norm getNormHeight() = 0;
    };

    struct i_norm_dimension_set : public i_norm_dimension {
        virtual void setNormWidth(con_norm x) = 0;
        virtual void setNormHeight(con_norm y) = 0;
    };

    template<typename T1, typename T2>
    struct i_dimension_convert {
        virtual T2 getWidth(T1 v) = 0;
        virtual T1 getWidth(T2 v) = 0;
        virtual T1 getWidth() = 0;
        virtual T2 getHeight(T1 v) = 0;
        virtual T1 getHeight(T2 v) = 0;
        virtual T1 getHeight() = 0;
        virtual T1 get(T1 x, T1 y) {
            return x * getWidth() + y;   
        }
        virtual T1 get(T2 x, T2 y) {
            return get(getWidth(x), getHeight(y));
        }
    };

    struct dim_prov : public i_dimension_convert<con_size, con_norm> {
        dim_prov(con_size width, con_size height) : width(width), height(height) {}
        dim_prov(): width(0), height(0) {}
        con_size width, height;
        con_size getWidth(con_norm v) override {
            return v * getWidth();
        }
        con_norm getWidth(con_size v) override {
            return v / con_norm(getWidth());
        }
        con_size getHeight(con_norm v) override {
            return v * getHeight();
        }
        con_norm getHeight(con_size v) override {
            return v / con_norm(getHeight());
        }
        con_norm getWidthStep() {
            return 1.0 / con_norm(getWidth());
        }
        con_norm getHeightStep() {
            return 1.0 / con_norm(getHeight());
        }
        con_size getWidth() override {
            return width;
        }
        con_size getHeight() override {
            return height;
        }
    };

    template<typename BUFFER_TYPE>
    struct i_buffer_sink {
        virtual void write(const buffer_source* source) = 0;
        virtual void write(con_norm x, con_norm y, BUFFER_TYPE value) = 0;
    };
    
    template<typename BUFFER_TYPE>
    struct i_buffer_source {
        virtual void read(buffer_sink* sink) = 0;
        virtual BUFFER_TYPE read(con_norm x, con_norm y) = 0;
    };
}
#pragma once
#include "defines.h"
#include "functions.h"

namespace cons {
    template<typename T1, typename T2>
    struct i_dimension_convert {
        
        virtual T2 getSampleWidth(T1 v) = 0;
        virtual T2 getSampleWidth() = 0;
        virtual T2 getSampleHeight(T1 v) = 0;
        virtual T2 getSampleHeight() = 0;
        virtual T2 getSampleWidthStep() = 0;
        virtual T2 getSampleHeightStep() = 0;

        virtual T1 getWidth(T2 v) = 0;
        virtual T1 getWidth() = 0;
        virtual T1 getHeight(T2 v) = 0;
        virtual T1 getHeight() = 0;

        virtual void setWidth(T1 width) = 0;
        virtual void setHeight(T1 height) = 0;

        T1 get(const T1 &x, const T1 &y) {
            return (y * getWidth()) + x;   
        }
        T1 getSample(const T2 &x, const T2 &y) {
            return get(getWidth(x), getHeight(y));
        }
    };

    typedef i_dimension_convert<con_size, con_norm> i_dim_std;

    struct dim_prov : public i_dim_std {
        dim_prov(con_size width, con_size height) : width(width), height(height) {}
        dim_prov(): width(0), height(0) {}

        con_size width, height;

        con_size getWidth(con_norm v) override {
            return v * getWidth();
        }
        con_norm getSampleWidth(con_size v) override {
            return v / con_norm(getWidth());
        }
        con_size getHeight(con_norm v) override {
            return v * getHeight();
        }
        con_norm getSampleHeight(con_size v) override {
            return v / con_norm(getHeight());
        }
        con_norm getSampleWidthStep() override {
            return 1.0f / con_norm(getWidth());
        }
        con_norm getSampleHeightStep() override {
            return 1.0f / con_norm(getHeight());
        }
        con_size getWidth() override {
            return width;
        }
        con_size getHeight() override {
            return height;
        }
        void setWidth(con_size width) override {
            this->width = width;
        }
        void setHeight(con_size height) override {
            this->height = height;
        }
        con_size getSize() {
            return getWidth() * getHeight();
        } 
        con_norm getSampleWidth() override {
            return 1.0f;
        }
        con_norm getSampleHeight() override {
            return 1.0f;
        }
    };

    template<typename BT>
    struct i_buffer_source;
    template<typename BT>
    struct i_buffer_sink;

    template<typename BT>
    struct i_buffer_sink {
        virtual void writeSample(con_norm x, con_norm y, BT value) = 0;
        //virtual void write(con_pos x, con_pos y, const BT* value, size_t start, size_t count) = 0;
        virtual void write(con_pos x, con_pos y, BT value) = 0;
        virtual ssize_t write(con_pos x, con_pos y, const BT *value) = 0;
        virtual ssize_t write(const BT* buf, size_t start, size_t count) = 0;
        virtual ssize_t write(const BT* buf, size_t count) = 0;
        virtual ssize_t write(const BT* buf) = 0;
        virtual void clear(con_pos width, con_pos height) = 0;
        virtual void clear() = 0;
    };
    
    template<typename BT>
    struct i_buffer_source {
        virtual BT readSample(con_norm x, con_norm y) = 0;
        virtual BT read(con_pos x, con_pos y) = 0;
        //virtual void read(con_pos x, con_pos y, BT* buf, size_t start, size_t count) = 0;
        virtual ssize_t read(BT* buf, size_t start, size_t count) = 0;
        virtual ssize_t read(BT* buf, size_t count) = 0;
    };

    template<typename BT>
    struct i_buffer_rw : public i_buffer_source<BT>, public i_buffer_sink<BT>
    {
        i_buffer_rw() {}
    };    

    template<typename BT>
    struct i_buffer_sink_dim : public dim_prov, public i_buffer_sink<BT> {
        i_buffer_sink_dim(con_size width, con_size height) : dim_prov(width, height) {}
        i_buffer_sink_dim() : dim_prov() {}
    };

    template<typename BT>
    struct i_buffer_rw_dim : public dim_prov, public i_buffer_rw<BT> {
        i_buffer_rw_dim(con_size width, con_size height) : dim_prov(width, height) {}
        i_buffer_rw_dim() : dim_prov() {}

        virtual void copyTo(i_buffer_sink_dim<BT>* buffer) = 0;
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
        virtual void sleep(int millis) = 0;
    };

    struct i_state {
        virtual bool isReady() = 0;
        virtual err_ret open() = 0;
        virtual err_ret close() = 0;
    };

    template<typename T>
    struct i_console_sink : 
    public i_buffer_sink<T>,
    public virtual i_cursor_set,
    public virtual dim_prov {
        ssize_t write(con_pos x, con_pos y, const T* text) override {
            this->setCursor(x, y);
            return this->write(text, getLength(text));
        }
        void write(con_pos x, con_pos y, T ch) override {
            this->setCursor(x, y);
            this->write(&ch, sizeof(ch));
        }
        void writeSample(con_norm x, con_norm y, T ch) override {
            this->write(this->getWidth(x), this->getHeight(y), ch);
        }
        ssize_t write(const T* text) override {
            return this->write(text, getLength(text));
        }
        ssize_t write(const T* buf, size_t count) override {
            return this->write(buf, 0, count);
        }
        ssize_t write(const T* buf, size_t start, size_t count) override {
            return 0;
        }

        void clear(con_pos width, con_pos height) override { }
        void clear() override { }
    };

    template<typename ascii_dt>
    struct i_console_basic_ascii :
    public i_keyboard,
    public i_other,
    public i_state,
    public i_console_sink<ascii_dt> {
        using ascii = i_console_sink<ascii_dt>;
        virtual con_type getType() = 0;
        ascii *getAscii() { return this; }
    };

    template<typename ascii_dt, typename unicode_dt>
    struct i_console_basic : 
    public i_console_basic_ascii<ascii_dt>,
    public i_console_sink<unicode_dt> {
        using unicode = i_console_sink<unicode_dt>;
        unicode *getUnicode() { return this; }
    };
}
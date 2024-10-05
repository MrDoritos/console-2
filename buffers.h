#pragma once
#include "functions.h"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib/stb_image_write.h"

namespace cons {
    /*
    Read/Write memory buffer interface with dimensions
    */
    template<typename T>
    struct buffer : public i_buffer_rw_dim<T> {
        using ib = i_buffer_rw_dim<T>;
        buffer(con_size width, con_size height) : buffer() {
            make(width, height);
        }
        buffer(_2dlength<con_size> size) : buffer(size.width, size.height) {}
        buffer() : i_buffer_rw_dim<T>() {
            _buffer = nullptr;
        }

        ~buffer() {
            free();
        }

        size_t getBytes() {
            return ib::getSize() * sizeof(T);
        }

        void free() {
            if (_buffer != nullptr) {
                delete[] _buffer;
            }
            _buffer = nullptr;
        }

        virtual void make(con_size width, con_size height) {
            this->width = width;
            this->height = height;
            make();
        }

        virtual void make() {
            free();
            _buffer = new T[ib::getSize()];
        }

        virtual void make(_2d<con_size> size) {
            make(size.x, size.y);
        }

        void clear() override {
            for (size_t i = 0; i < ib::getSize(); i++) {
                if constexpr (std::is_constructible_v<T>)
                    _buffer[i] = T();
                else
                    _buffer[i] = T((int)' ');
            }
        }

        void clear(con_pos width, con_pos height) override {
            clear();
        }

        T* _buffer;


        T readSample(con_norm x, con_norm y) override {
            return _buffer[ib::getSample(x, y)];
        }

        T read(con_pos x, con_pos y) override {
            return _buffer[ib::get(x, y)];
        }

        ssize_t read(T* buf, size_t start, size_t count) override {
            memcpy(&buf[0], &_buffer[start], count * sizeof(*buf));
            return count;
        }

        ssize_t read(T* buf, size_t count) override {
            return read(buf, 0, count);
        }

        void writeSample(con_norm x, con_norm y, T value) override {
            _buffer[ib::getSample(x, y)] = value;
        }

        void write(con_pos x, con_pos y, T value) override {
            _buffer[ib::get(x, y)] = value;
        }

        ssize_t write(con_pos x, con_pos y, const T* value) override {
            size_t pos = ib::get(x, y);
            return write(value, pos, getLength(value));
        }

        ssize_t write(const T* buf, size_t start, size_t count) override {
            memcpy(&_buffer[start], &buf[0], count * sizeof(*buf));
            return count;
        }

        ssize_t write(const T* buf, size_t count) override {
            return write(buf, 0, count);
        }

        ssize_t write(const T* buf) override {
            return write(buf, 0, getBytes());
        }

        void copyTo(i_buffer_sink_dim<T>* buffer) override {
            cons::copyTo(this, buffer);
        }
    };

    template<template<typename> typename INB, typename INT,
            template<typename> typename OUTB, typename OUTT>
    struct source_convert : OUTB<OUTT> {
        INB<INT> *source;

        source_convert(INB<INT> * b):source(b)  {}

        OUTT readSample(con_norm x, con_norm y) override {
            return convert(source->readSample(x, y));
        }

        OUTT read(con_pos x, con_pos y) override {
            return convert(source->read(x, y));
        }

        virtual OUTT convert(INT in) {
            return OUTT(in);
        }

        ssize_t read(OUTT* buf, size_t start, size_t count) override {
            INT inbuf[count];
            ssize_t cnt = source->read(&inbuf[0], start, count);
            for (int i = 0; i < count; i++)
                buf[i] = convert(inbuf[i]);
            return count;
        }

        ssize_t read(OUTT* buf, size_t count) override {
            INT inbuf[count];
            ssize_t cnt = source->read(&inbuf[0], count);
            for (int i = 0; i < count; i++)
                buf[i] = convert(inbuf[i]);
            return count;
        }
    };
    /*
    load/save image buffer
    */
    template<typename pixT>
    struct image : public buffer<pixT> {
        image() {
            bpp = 4;
            stbi = false;
        }

        image(con_size width, con_size height) 
        : buffer<pixT>(width, height) {
            bpp = 4;
            stbi = false;
        }

        bool stbi;
        int bpp;

        virtual int save(const char* filename) {
            if (!this->_buffer)
                return ENUMS::ERROR;

            int bpp = this->getBytes() / this->getDimensions().product();

            return stbi_write_png(filename, this->getWidth(), 
                                  this->getHeight(), bpp, this->_buffer, 0);
        }

        virtual int load(const char* filename) {
            int w, h, n;

            unsigned char *data;
            data = stbi_load(filename, &w, &h, &n, 0);

            if (!data)
                return ENUMS::ERROR;

            bpp = n;
            this->make(w, h);
            memcpy((void*)this->_buffer, data, w * h * n);
            stbi_image_free(data);
            stbi = true;

            return ENUMS::NO_ERROR;
        }
    };

    template<typename T, template<typename> typename bufT>
    struct atlas_fragment;

    /*
    texture atlas
    */
    template<typename T, template<typename> typename bufT>
    struct atlas : public bufT<T> {
        int spriteSize;
        atlas(int spriteSize):bufT<T>() {
            this->spriteSize = spriteSize;
        }

        atlas_fragment<T, bufT> fragment(sizei sprite_units);
    };

    /*
    maps to atlas
    */
    template<typename T, template<typename> typename bufT>
    struct atlas_fragment : public i_buffer_rw_dim<T> {
        bufT<T>* sourceAtlas;
        sizei atlasPixelArea;
        sizei atlasSpriteArea;

        atlas_fragment():i_buffer_rw_dim<T>() {
            sourceAtlas = nullptr;
        }

        posf normToAtlas(const posf &p) {
            posf r;
            r.x = atlasPixelArea.x + (p.x * atlasPixelArea.width);
            r.y = atlasPixelArea.y + (p.y * atlasPixelArea.height);
            r.x /= sourceAtlas->getWidth();
            r.y /= sourceAtlas->getHeight();
            return r;
        }

        posi mapToAtlas(const posi &p) {
            posi r;
            r.x = atlasPixelArea.x + (p.x * atlasPixelArea.width);
            r.y = atlasPixelArea.y + (p.y * atlasPixelArea.height);
            return r;
        }

        T readSample(con_norm x, con_norm y) override {
            auto atl = normToAtlas({x, y});
            return sourceAtlas->readSample(atl.x, atl.y);
        }

        T read(con_size x, con_size y) override {
            auto atl = mapToAtlas({x, y});
            return sourceAtlas->read(atl.x, atl.y);
        }

        ssize_t read(T* buf, size_t start, size_t count) override {
            for (size_t i = 0; i < count; i++) {
                int x = i % atlasSpriteArea.width;
                int y = i / atlasSpriteArea.width;
                auto atl = mapToAtlas({x, y});
                buf[i] = sourceAtlas->read(atl.x, atl.y);
            }
            return count;
        }

        ssize_t read(T* buf, size_t count) override {
            return read(buf, 0, count);
        }

        void writeSample(con_norm x, con_norm y, T value) override {
            auto atl = normToAtlas({x, y});
            sourceAtlas->writeSample(atl.x, atl.y, value);
        }

        void write(con_size x, con_size y, T value) override {
            auto atl = mapToAtlas({x, y});
            sourceAtlas->write(atl.x, atl.y, value);            
        }


        ssize_t write(const T* buf, size_t start, size_t count) override {
            for (size_t i = 0; i < count; i++) {
                int x = i % atlasPixelArea.width;
                int y = i / atlasPixelArea.width;
                posf atl = mapToAtlas({x, y});
                sourceAtlas->write(atl.x, atl.y, buf[i]);
            }
            return count;
        }

        ssize_t write(const T* buf, size_t count) override {
            return write(buf, 0, count);
        }

        ssize_t write(const T* buf) override {
            return write(buf, 0, getLength(buf));
        }

        ssize_t write(con_size x, con_size y, const T* value) override {
            return write(value, mapToAtlas({x, y}).product(), getLength(value));
        }

        void copyTo(i_buffer_sink_dim<T>* buffer) override {
            cons::copyTo(this, buffer);
        }
    };

    template<typename T, template<typename> typename bufT>
    atlas_fragment<T, bufT> atlas<T, bufT>::fragment(sizei sprite_units) {
        atlas_fragment<T, bufT> frag;
        sizei pixel_units;
        frag.sourceAtlas = this;

        pixel_units.x = sprite_units.x * spriteSize;
        pixel_units.y = sprite_units.y * spriteSize;
        pixel_units.width = sprite_units.width * spriteSize;
        pixel_units.height = sprite_units.height * spriteSize;

        frag.atlasPixelArea = pixel_units;
        frag.atlasSpriteArea = sprite_units;

        return frag;
    }
}
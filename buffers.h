#pragma once
#include "functions.h"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"

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

        void clear() override {
            memset(_buffer, 0, getBytes());  
        }

        void clear(con_pos width, con_pos height) override {
            clear();
        }

        T* _buffer;

        void writeSample(con_norm x, con_norm y, T value) override {
            _buffer[ib::getSample(x, y)] = value;
        }

        void write(con_pos x, con_pos y, T value) override {
            _buffer[ib::get(x, y)] = value;
        }

        T read(con_pos x, con_pos y) override {
            return _buffer[ib::get(x, y)];
        }

        T readSample(con_norm x, con_norm y) override {
            return _buffer[ib::getSample(x, y)];
        }

        ssize_t write(con_pos x, con_pos y, const T* value) override {
            size_t pos = ib::get(x, y);
            return write(value, pos, getLength(value));
        }

        ssize_t write(const T* buf, size_t start, size_t count) override {
            memcpy(_buffer + start, buf, count);
            return count;
        }

        ssize_t write(const T* buf, size_t count) override {
            return write(buf, 0, count);
        }

        ssize_t write(const T* buf) override {
            return write(buf, 0, getBytes());
        }

        ssize_t read(T* buf, size_t start, size_t count) override {
            memcpy(buf, _buffer + start, count);
            return count;
        }

        ssize_t read(T* buf, size_t count) override {
            return read(buf, 0, count);
        }

        void copyTo(i_buffer_sink_dim<T>* buffer) override {
            cons::copyTo(this, buffer);
        }
    };

    typedef buffer<con_basic> buffer_basic;
    typedef buffer<con_wide> buffer_wide;
    typedef buffer<con_color> buffer_color;
    typedef buffer<pixel> buffer_pixel;

    /*
    pixel buffer + conversion buffer
    */
    template<typename T>
    struct pixel_image : public buffer_pixel, public buffer<T> {
        pixel_image(con_size width, con_size height) 
        :buffer_pixel(width, height),
         buffer<T>(width, height) {
            bpp = 0;
            stbi = false;
        }
        pixel_image():pixel_image(0, 0) {}

        void make() override {
            buffer_pixel::make();
            buffer<T>::make();
        }

        void make(con_size width, con_size height) override {
            buffer_pixel::make(width, height);
            buffer<T>::make(width, height);
        }

        void compose() {
            cons::copyTo(buffer_pixel::sink(), buffer<T>::sink());
        }

        int load(const char* filename) {
            int w, h, n;

            unsigned char *data;
            data = stbi_load(filename, &w, &h, &n, 0);

            bpp = n;
            buffer_pixel::make(w, h);
            buffer<T>::make(w, h);

            pixel *dest = buffer_pixel::_buffer;

            memcpy((void*)dest, data, w * h * n);

            stbi_image_free(data);

            if (!data)
                return ENUMS::ERROR;

            stbi = true;

            compose();

            return ENUMS::NO_ERROR;
        }


        int bpp;
        bool stbi;
    };

    template<typename T>
    struct atlas_fragment;

    /*
    texture atlas
    */
    template<typename T>
    struct atlas : public pixel_image<T> {
        int spriteSize;
        atlas(int spriteSize):pixel_image<T>() {
            this->spriteSize = spriteSize;
        }

        atlas_fragment<T> fragment(sizei sprite_units);
    };

    /*
    maps to atlas
    */
    template<typename T>
    struct atlas_fragment : public i_buffer_rw_dim<T> {
        using ib = i_buffer_rw_dim<T>;
        using ip = i_buffer_rw_dim<pixel>;
        atlas<T>* sourceAtlas;
        sizei atlasPixelArea;
        sizei atlasSpriteArea;

        atlas_fragment():i_buffer_rw_dim<T>() {
            sourceAtlas = nullptr;
        }

        posf mapToAtlas(posf p) {
            posf r;
            r.x = atlasPixelArea.x + (p.x * atlasPixelArea.width);
            r.y = atlasPixelArea.y + (p.y * atlasPixelArea.height);
            r.x /= ((ib*)sourceAtlas)->getWidth();
            r.y /= ((ib*)sourceAtlas)->getHeight();
            return r;
        }

        pixel sampleImage(con_norm x, con_norm y) {
            posf atl = mapToAtlas(posf(x, y));
            return ((ip*)sourceAtlas)->readSample(atl.x, atl.y);
        }

        T readSample(con_norm x, con_norm y) override {
            posf atl = mapToAtlas(posf(x, y));
            return ((ib*)sourceAtlas)->readSample(atl.x, atl.y);
        }

        void writeSample(con_norm x, con_norm y, T value) override {
            posf atl = mapToAtlas(posf(x, y));
            ((ib*)sourceAtlas)->writeSample(atl.x, atl.y, value);
        }

        T read(con_size x, con_size y) override {
            posf atl = mapToAtlas(posf(x, y));
            return ((ib*)sourceAtlas)->read(atl.x, atl.y);
        }

        void write(con_size x, con_size y, T value) override {
            posf atl = mapToAtlas(posf(x, y));
            ((ib*)sourceAtlas)->write(atl.x, atl.y, value);
            
        }

        ssize_t write(con_pos x, con_pos y, const T* value) override {
            return 0;
        }

        ssize_t write(const T* buf, size_t start, size_t count) override {
            return 0;
        }

        ssize_t write(const T* buf, size_t count) override {
            return 0;
        }

        ssize_t write(const T* buf) override {
            return 0;
        }

        ssize_t read(T* buf, size_t start, size_t count) override {
            return 0;
        }

        ssize_t read(T* buf, size_t count) override {
            return 0;
        }

        void copyTo(i_buffer_sink_dim<T>* buffer) override {
            cons::copyTo(this, buffer);
        }
    };

    template<typename T>
    atlas_fragment<T> atlas<T>::fragment(sizei sprite_units) {
        atlas_fragment<T> frag;
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
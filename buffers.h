#pragma once
#include "functions.h"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"

namespace cons {
    /*
    Read/Write memory buffer interface with dimensions
    */
    template<typename BT>
    struct buffer_rw : public i_buffer_rw_dim<BT> {
        using ib = i_buffer_rw_dim<BT>;
        buffer_rw(con_size width, con_size height) : buffer_rw() {
            make(width, height);
        }
        buffer_rw() : i_buffer_rw_dim<BT>() {
            buffer = nullptr;
        }

        ~buffer_rw() {
            free();
        }

        size_t getBytes() {
            return ib::getSize() * sizeof(BT);
        }

        void free() {
            if (buffer != nullptr) {
                delete[] buffer;
            }
            buffer = nullptr;
        }

        virtual void make(con_size width, con_size height) {
            this->width = width;
            this->height = height;
            make();
        }

        virtual void make() {
            free();
            buffer = new BT[ib::getSize()];
        }

        void clear() override {
            memset(buffer, 0, getBytes());  
        }

        void clear(con_pos width, con_pos height) override {
            clear();
        }

        BT* buffer;

        void writeSample(con_norm x, con_norm y, BT value) override {
            buffer[ib::getSample(x, y)] = value;
        }

        void write(con_pos x, con_pos y, BT value) override {
            buffer[ib::get(x, y)] = value;
        }

        BT read(con_pos x, con_pos y) override {
            return buffer[ib::get(x, y)];
        }

        BT readSample(con_norm x, con_norm y) override {
            return buffer[ib::getSample(x, y)];
        }

        ssize_t write(con_pos x, con_pos y, const BT* value) override {
            size_t pos = ib::get(x, y);
            return write(value, pos, getLength(value));
        }

        ssize_t write(const BT* buf, size_t start, size_t count) override {
            memcpy(buffer + start, buf, count);
            return count;
        }

        ssize_t write(const BT* buf, size_t count) override {
            return write(buf, 0, count);
        }

        ssize_t write(const BT* buf) override {
            return write(buf, 0, getBytes());
        }

        ssize_t read(BT* buf, size_t start, size_t count) override {
            memcpy(buf, buffer + start, count);
            return count;
        }

        ssize_t read(BT* buf, size_t count) override {
            return read(buf, 0, count);
        }

        void copyTo(i_buffer_sink_dim<BT>* buffer) override {
            cons::copyTo(this, buffer);
        }
    };

    typedef buffer_rw<con_basic> buffer_basic;
    typedef buffer_rw<con_wide> buffer_wide;
    typedef buffer_rw<con_color> buffer_color;
    typedef buffer_rw<pixel> buffer_pixel;

    /*
    pixel buffer + conversion buffer
    */
    template<typename con_char>
    struct pixel_image : public buffer_pixel, public buffer_rw<con_char> {
        pixel_image(con_size width, con_size height) 
        :buffer_pixel(width, height),
         buffer_rw<con_char>(width, height) {
            bpp = 0;
            stbi = false;
        }
        pixel_image():pixel_image(0, 0) {}

        void make() override {
            buffer_pixel::make();
            buffer_rw<con_char>::make();
        }

        void make(con_size width, con_size height) override {
            buffer_pixel::make(width, height);
            buffer_rw<con_char>::make(width, height);
        }

        void compose() {
            for (con_pos y = 0; y < buffer_pixel::height; y++) {
                for (con_pos x = 0; x < buffer_pixel::width; x++) {                    
                    pixel p = buffer_pixel::read(x, y);
                    buffer_rw<con_char>::write(x, y, (con_char)p.value());
                }
            }
        }

        int load(const char* filename) {
            int w, h, n;

            unsigned char *data;
            data = stbi_load(filename, &w, &h, &n, 0);

            bpp = n;
            buffer_pixel::make(w, h);
            buffer_rw<con_char>::make(w, h);

            pixel *dest = buffer_pixel::buffer;

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

    template<typename con_char>
    struct atlas_fragment;

    /*
    texture atlas
    */
    template<typename con_char>
    struct atlas : public pixel_image<con_char> {
        int spriteSize;
        atlas(int spriteSize):pixel_image<con_char>() {
            this->spriteSize = spriteSize;
        }

        atlas_fragment<con_char> fragment(sizei sprite_units);
    };

    /*
    maps to atlas
    */
    template<typename BT>
    struct atlas_fragment : public i_buffer_rw_dim<BT> {
        using ib = i_buffer_rw_dim<BT>;
        using ip = i_buffer_rw_dim<pixel>;
        atlas<BT>* sourceAtlas;
        sizei atlasPixelArea;
        sizei atlasSpriteArea;

        atlas_fragment():i_buffer_rw_dim<BT>() {
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

        BT readSample(con_norm x, con_norm y) override {
            posf atl = mapToAtlas(posf(x, y));
            return ((ib*)sourceAtlas)->readSample(atl.x, atl.y);
        }

        void writeSample(con_norm x, con_norm y, BT value) override {
            posf atl = mapToAtlas(posf(x, y));
            ((ib*)sourceAtlas)->writeSample(atl.x, atl.y, value);
        }

        BT read(con_size x, con_size y) override {
            posf atl = mapToAtlas(posf(x, y));
            return ((ib*)sourceAtlas)->read(atl.x, atl.y);
        }

        void write(con_size x, con_size y, BT value) override {
            posf atl = mapToAtlas(posf(x, y));
            ((ib*)sourceAtlas)->write(atl.x, atl.y, value);
            
        }

        ssize_t write(con_pos x, con_pos y, const BT* value) override {
            return 0;
        }

        ssize_t write(const BT* buf, size_t start, size_t count) override {
            return 0;
        }

        ssize_t write(const BT* buf, size_t count) override {
            return 0;
        }

        ssize_t write(const BT* buf) override {
            return 0;
        }

        ssize_t read(BT* buf, size_t start, size_t count) override {
            return 0;
        }

        ssize_t read(BT* buf, size_t count) override {
            return 0;
        }

        void copyTo(i_buffer_sink_dim<BT>* buffer) override {
            cons::copyTo(this, buffer);
        }
    };

    template<typename con_char>
    atlas_fragment<con_char> atlas<con_char>::fragment(sizei sprite_units) {
        atlas_fragment<con_char> frag;
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
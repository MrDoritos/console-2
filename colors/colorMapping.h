#pragma once
#include <string>

#include "../defines.h"
#include "../functions.h"

namespace cons {
    template<typename T>
    struct i_color_map {
        virtual void init() = 0;
        virtual void setCharacters(const T* ch) = 0;
        virtual cpix_Tchar<T> getCpix(pixel &pix) = 0;
    };

    template<typename T>
    struct color_map : i_color_map<T> {
        bool have_table;
        std::basic_string<T> characters;
        typedef cpix_Tchar<T> cpix;

        void init() override { }

        cpix getCpix(pixel &pix) override { return cpix(); }

        void setCharacters(const T* ch) override {
            characters = ch;

            if (have_table)
                init();
        }

        color_map() requires (std::is_same_v<T, con_wide>) {
            setCharacters(L" ░▒");
            have_table = false;
        }

        color_map() requires (std::is_same_v<T, con_basic>) {
            setCharacters(" O8");
            have_table = false;
        }
    };
}
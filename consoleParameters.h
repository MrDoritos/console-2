#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define GENERIC 0
#define CMD 1
#define NCURSES 2

namespace cons {
    struct parameters {
        int width, height;
        int fontWidth, fontHeight;
        int system_id;

        virtual void set(cons::parameters* source) {
            memcpy(this, source, sizeof(cons::parameters));
        }
    };


}
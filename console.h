#pragma once
#include "consoleParameters.h"
#include "consoleColors.h"
#include "ncursesw/ncurses.h"

namespace cons {
    struct console : parameters {
        typedef FILE CONFILE;
        CONFILE* input, output, error;
        color_t last_color;

        console() {
            this->input = stdin;
            this->output = stdout;
            this->error = stderr;

            initscr();
        }

        virtual void copy(cons::console *source) {
            this->set(source);

            this->input = source->input;
            // TO-DO finish
        }

        virtual void write(int x, int y, const char *text) {

        }
    };
}
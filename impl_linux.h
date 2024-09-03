#pragma once
#include <ncursesw/ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include "interfaces.h"

namespace cons {
    struct console_ncurses : public i_console_basic {

        winsize w;
        WINDOW* win;
        SCREEN* scr;
        bool ready = false;
        virtual void refreshCon() {
            wrefresh(getWindow());
        }
        virtual void refreshSize() {
            ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        }
        virtual bool isReady() { return ready; }
        virtual con_size getWidth() { refreshSize(); return w.ws_col; }
        virtual con_size getHeight() { refreshSize(); return w.ws_row; }
        virtual err_ret open() {
            win = initscr();
            //scr = newterm(getenv("TERM"), stdout, stdin);
            //set_term(scr);
            //win = stdscr;
            scr = nullptr;
            setlocale(LC_ALL, "");

            setWidth(getWidth());
            setHeight(getHeight());

            curs_set_sp(getScreen(), 0);
            set_escdelay_sp(getScreen(), 0);
            cbreak_sp(getScreen());
            noecho_sp(getScreen());

            noecho();
            cbreak();
            set_escdelay(0);
            curs_set(0);


            keypad(getWindow(), true);
            ready = true;
            return ENUMS::NO_ERROR;
        }
        virtual err_ret close() {
            //endwin_sp(getScreen());
            //set_term(nullptr);
            //delscreen(getScreen());
            endwin();
            ready = false;
            return ENUMS::NO_ERROR;
        }
        virtual void setCursor(con_pos x, con_pos y) {
            //move(y, x);
            wmove(getWindow(), y, x);
        }
        virtual con_pos getCursorX() { return getcurx(stdscr); }
        virtual con_pos getCursorY() { return getcury(stdscr); }



        ssize_t write(const con_basic* buf, size_t count) override {
            waddnstr(getWindow(), buf, count);
            this->refreshCon();
            return count;
        }
        ssize_t write(const con_basic* text) {
            return i_console_basic_write::write(text);
        }
        ssize_t write(const con_wide* buf, size_t count) override {
            waddnwstr(getWindow(), buf, count);
            this->refreshCon();
            return count;
        }
        ssize_t write(const con_wide* text) {
            return i_console_basic::write(text);
        }



        virtual con_type getType() { return con_type{"ncursesw"}; }
        virtual con_basic_key getKey() {
            return wgetch(getWindow());
        }
        virtual con_basic_key readKey() { timeout(-1); return getKey(); }
        virtual con_basic_key readKeyAsync() { 
            timeout(0);
            con_basic_key key = getKey();
            return key == -1 ? 0 : key;
        }
        virtual void clear() { 
            wclear(getWindow());
            refreshCon();
        }
        virtual WINDOW* getWindow() { return win; }
        virtual SCREEN* getScreen() { return scr; }
        virtual void flush() { refreshCon(); }
        void sleep(int millis) override {
            usleep(millis * 1000);
        }
    };

    template<typename CON_NCURSES>
    struct console_ncurses_color : public CON_NCURSES, public i_color {
        virtual void setColor(con_color color) {
            attron(COLOR_PAIR(color));
        }
        virtual err_ret open() {
            err_ret ret = CON_NCURSES::open();
            if (ret != ENUMS::NO_ERROR) 
                return ret | ENUMS::ERROR;
            start_color();
            init_pair(1, COLOR_WHITE, COLOR_BLACK);
            init_pair(2, COLOR_RED, COLOR_BLACK);
            init_pair(3, COLOR_GREEN, COLOR_BLACK);
            init_pair(4, COLOR_YELLOW, COLOR_BLACK);
            init_pair(5, COLOR_BLUE, COLOR_BLACK);
            init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
            init_pair(7, COLOR_CYAN, COLOR_BLACK);
            init_pair(8, COLOR_BLACK, COLOR_WHITE);
            init_pair(9, COLOR_BLACK, COLOR_RED);
            init_pair(10, COLOR_BLACK, COLOR_GREEN);
            init_pair(11, COLOR_BLACK, COLOR_YELLOW);
            init_pair(12, COLOR_BLACK, COLOR_BLUE);
            init_pair(13, COLOR_BLACK, COLOR_MAGENTA);
            init_pair(14, COLOR_BLACK, COLOR_CYAN);
            init_pair(15, COLOR_BLACK, COLOR_BLACK);
            return ENUMS::NO_ERROR;
        }
        con_color last_color;

        template<typename T>
        void write(T text, con_color color = 1) {
            setColor(2);
            CON_NCURSES::write(text);
        }

        template<typename T>
        void write(con_pos x, con_pos y, T text, con_color color = 1) {
            setColor(3);
            CON_NCURSES::write(x, y, text);
        }

        template<typename T>
        void write(con_pos x, con_pos y, T text, const con_color *color) {
            CON_NCURSES::setCursor(x,y);
            for (size_t i = 0; i < wcslen(text); i++) {
                setColor(color[i]);
                CON_NCURSES::write(text[i], 1);
            }
        }    
    };

    typedef console_ncurses console;
    typedef console_ncurses_color<console> console_color;
}
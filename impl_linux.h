#pragma once
#include <ncursesw/ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include "interfaces.h"

namespace cons {
    template<typename ascii_dt, typename unicode_dt>
    struct console_ncurses : public i_console_basic<ascii_dt, unicode_dt> {
        using ic = i_console_basic<ascii_dt, unicode_dt>;
        //using ic_a = (i_console_sink<ascii_dt>*)ic;
        //using ic_u = (i_console_sink<unicode_dt>*)ic;

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
        bool isReady() override { return ready; }
        con_size getWidth() override { refreshSize(); return w.ws_col; }
        con_size getHeight() override { refreshSize(); return w.ws_row; }
        err_ret open() override {
            win = initscr();
            //scr = newterm(getenv("TERM"), stdout, stdin);
            //set_term(scr);
            //win = stdscr;
            scr = nullptr;
            setlocale(LC_ALL, "");

            ic::setWidth(ic::getWidth());
            ic::setHeight(ic::getHeight());

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
        err_ret close() override {
            //endwin_sp(getScreen());
            //set_term(nullptr);
            //delscreen(getScreen());
            endwin();
            ready = false;
            return ENUMS::NO_ERROR;
        }
        void setCursor(con_pos x, con_pos y) override {
            //move(y, x);
            wmove(getWindow(), y, x);
        }
        con_pos getCursorX() override { return getcurx(stdscr); }
        con_pos getCursorY() override { return getcury(stdscr); }



        ssize_t write(const con_basic* buf, size_t start, size_t count) override {
            waddnstr(getWindow(), buf, count);
            this->refreshCon();
            return count;
        }
        
        ssize_t write(const con_wide* buf, size_t start, size_t count) override {
            waddnwstr(getWindow(), buf, count);
            this->refreshCon();
            return count;
        }



        con_type getType() override { return con_type{"ncursesw"}; }
        virtual con_basic_key getKey() {
            return wgetch(getWindow());
        }
        con_basic_key readKey() override { timeout(-1); return getKey(); }
        con_basic_key readKeyAsync() override { 
            timeout(0);
            con_basic_key key = getKey();
            return key == -1 ? 0 : key;
        }
        void clear() override { 
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

    template<typename con_base>
    struct console_ncurses_color : public con_base, public i_color {
        void setColor(con_color color) override {
            attron(COLOR_PAIR(color));
        }
        err_ret open() override {
            err_ret ret = con_base::open();
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
            con_base::write(text, 0, getLength(text));
        }

        template<typename T>
        void write(con_pos x, con_pos y, T text, con_color color = 1) {
            setColor(3);
            con_base::write(x, y, text);
        }

        template<typename T>
        void write(con_pos x, con_pos y, T text, const con_color *color) {
            con_base::setCursor(x,y);
            for (size_t i = 0; i < getLength(text); i++) {
                setColor(color[i]);
                con_base::write(text[i], 0, 1);
            }
        }    
    };

    typedef console_ncurses<con_basic,con_wide> console;
    typedef console_ncurses_color<console> console_color;
}
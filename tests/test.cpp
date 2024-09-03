#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <ncursesw/ncurses.h>
#include <sys/ioctl.h>
#include <string>
#include <string.h>

//#include "../autoconsole.h"
#include "../impl_linux.h"
#include "../buffers.h"

using namespace cons;

int main() {
    console_ncurses_color<console_ncurses<con_basic, con_wide>> con;

    con.open();

    con.write("Hello, world!\n");

    using at = atlas<con_wide>;
    using atf = atlas_fragment<con_wide>;

    at atlas(8);

    if (atlas.load("textures.png") != ENUMS::NO_ERROR) {
        con.write("Failed to load textures.png\n");
        con.sleep(1000);
        return ENUMS::ERROR;
    }
        
    atf frag = atlas.fragment({1, 1, 1, 1});
    
    copyTo(frag.sink<con_wide>(), con.sink<con_wide>(), {0.25, 0.25, 0.5, 0.5});
    //copyTo(frag.sink<con_wide>(), con.unicode::sink<con_wide>());
    //copyTo(&frag, con.sink<con_wide>());
    //copyTo(frag.sink<converter<con_wide>>(), con.sink<converter<con_wide>>());

    const con_wide *texts[] = {
        L"weirds: ▛▖ ▗▜ ▙ ▞▚ ",
        L"blocks: ▀ ▁ ▂ ▃ ▄ ▅ ▆ ▇ █ ▉ ▊ ▋ ▌ ▍ ▎ ▏",
        L"arrows: ↑ → ↓ ← ↗ ↘ ↙ ↖",
        L"shapes: ▰ ▱ ▲ △ ▴ ▵ ▶ ▷ ▸ ▹ ▼ ▽ ▾ ▿ ◀ ◁ ◂ ◃ ◄ ◅ ◆ ◇ ◈ ◉ ◊ ○ ◌ ◍ ◎ ● ◐ ◑ ◒ ◓ ◔ ◕ ◖ ◗ ◘ ◙ ◚ ◛ ◜ ◝ ◞ ◟ ◠ ◡ ◢ ◣ ◤ ◥ ◦ ◧ ◨ ◩ ◪ ◫ ◬ ◭ ◮ ◯"
    };

    con.setCursor(0, 0);

    for (int i = 0; i < 4; i++) {
        con.write(texts[i]);
        con.write("\n");
    }


    while (con_key(con.readKey()).key != 'q') {
        con.write("Hello, world!\n");
    }

    con.close();

    return ENUMS::NO_ERROR;
}
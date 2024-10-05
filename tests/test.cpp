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
    console_ncurses_mouse<console_ncurses_color<console_ncurses<con_basic, con_wide>>> con;

    using cpix_sink = sink_wcpix;

    con.open();

    con.write("Hello, world!\n");

    using at = atlas<pixel, image>;
    using atf = atlas_fragment<pixel, image>;

    at atlas(8);

    if (atlas.load("textures.png") != ENUMS::NO_ERROR) {
        con.write("Failed to load textures.png\n");
        con.sleep(1000);
        return ENUMS::ERROR;
    }
        
    atf frag = atlas.fragment({1, 1, 1, 1});
    auto frag_con = source_convert<i_buffer_rw_dim, pixel, buffer, cpix_wide>(frag.sink());
    
    copyTo(frag_con.sink(), con.cpix_sink::sink(), {0.25, 0.25, 0.5, 0.5});
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


    while (con_key(con.readKeyAsync()).key != 'q') {
        con.write("Hello, world!\n");
        con_mouse m = con.readMouse();
        copyTo(frag_con.sink(), con.cpix_sink::sink(), {float(m.x) / con.getWidth(), float(m.y) / con.getHeight(), 0.5, 0.5});
        fprintf(stderr, "Mouse: %d %d %d %d %d\n", m.x, m.y, m.z, m.state, m.id);
    }

    con.close();

    return ENUMS::NO_ERROR;
}
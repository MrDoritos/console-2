#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <ncursesw/ncurses.h>
#include <sys/ioctl.h>
#include <string>
#include <string.h>

#include "../autoconsole.h"
#include "../buffers.h"

using namespace cons;

int main() {
    con.write("Hello, world!\n");

    using at = atlas<con_wide>;
    using atf = atlas_fragment<con_wide>;

    at atlas(8);

    
    if (atlas.load("textures.png")) {
        con.write("Failed to load textures.png\n");
        con.sleep(1000);
        //return ENUMS::ERROR;
    }
    

    atf frag = atlas.fragment({0, 0, 6, 6});
    
    //frag.copyTo((i_buffer_sink_dim)&con);
    copyTo(&frag, &con);

    const con_wide *texts[] = {
        L"weirds: ▛▖ ▗▜ ▙ ▞▚ ",
        L"blocks: ▀ ▁ ▂ ▃ ▄ ▅ ▆ ▇ █ ▉ ▊ ▋ ▌ ▍ ▎ ▏",
        L"arrows: ↑ → ↓ ← ↗ ↘ ↙ ↖",
        L"shapes: ▰ ▱ ▲ △ ▴ ▵ ▶ ▷ ▸ ▹ ▼ ▽ ▾ ▿ ◀ ◁ ◂ ◃ ◄ ◅ ◆ ◇ ◈ ◉ ◊ ○ ◌ ◍ ◎ ● ◐ ◑ ◒ ◓ ◔ ◕ ◖ ◗ ◘ ◙ ◚ ◛ ◜ ◝ ◞ ◟ ◠ ◡ ◢ ◣ ◤ ◥ ◦ ◧ ◨ ◩ ◪ ◫ ◬ ◭ ◮ ◯"
    };

    for (int i = 0; i < 4; i++) {
        con.write(texts[i]);
        con.write("\n");
    }

    while (con_key(con.readKey()).key != 'q') {
        con.write("Hello, world!\n");
    }

    return ENUMS::NO_ERROR;
}
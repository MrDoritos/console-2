#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <ncursesw/ncurses.h>
#include <sys/ioctl.h>
#include <string>
#include <string.h>

#define CONSOLE_IMPL "impl_linux.h"
#include "autoconsole.h"

using namespace cons;

int main() {
    con.write("Hello, world!\n");

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
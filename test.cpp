#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <ncursesw/ncurses.h>
#include <sys/ioctl.h>
#include <string>
#include <string.h>

#include "autoconsole.h"

using namespace cons;

template<typename BT>
struct buffer_rw : public dim_prov, public i_buffer_sink<BT>, public i_buffer_source<BT> {
    buffer_rw(con_size width, con_size height) : dim_prov(width, height) {
        buffer = new BT[get(width, height)];
    }

    ~buffer_rw() {
        delete[] buffer;
    }

    void write(con_norm x, con_norm y, BT value) override {
        buffer[get(x, y)] = value;
    }

    void write(const buffer_source* source) override {
        for (con_norm y = 0; y < 1; y+=source->getHeightStep()) {
            for (con_norm x = 0; x < 1; x+=source->getWidthStep()) {
                write(x, y, source->read(x, y));
            }
        }
    }

    BT read(con_norm x, con_norm y) const override {
        return buffer[get(x, y)];
    }

    void read(buffer_sink* sink) const override {
        for (con_norm y = 0; y < 1; y+=getHeightStep()) {
            for (con_norm x = 0; x < 1; x+=getWidthStep()) {
                sink->write(x, y, read(x, y));
            }
        }
    }
};

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
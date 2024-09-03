#pragma once
#ifndef CONSOLE_IMPL
#ifdef __linux__
#define CONSOLE_IMPL "impl_linux.h"
#endif
#endif
#include CONSOLE_IMPL

namespace cons {
    #warning "autoconsole used"
    automatic_console<console_color> con;
}
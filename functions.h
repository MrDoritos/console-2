#pragma once

#include "interfaces.h"

namespace cons {
    template<template<typename> typename T, typename BT>
    void copyTo(const T<BT>* source, T<BT>* sink) {
        for (con_norm y = 0; y < sink->getHeight(sink->getHeight()); y+=sink->getHeightStep()) {
            for (con_norm x = 0; x < sink->getWidth(sink->getWidth()); x+=sink->getWidthStep()) {
                sink->write(x, y, source->read(x, y));
            }
        }
    }

    template<template<typename,typename,typename> typename T, typename _ch>
    T<_ch, con_color, con_color> get_cpix(_ch ch, con_color co, con_color a) {
        T pix;
        pix.ch = ch;
        pix.co = co;
        pix.a = a;
        return pix;
    }
};
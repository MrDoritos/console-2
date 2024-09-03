#pragma once
#include "interfaces.h"

namespace cons {
    template<typename T1, typename T2>
    void copyTo(T1* source, T2* sink) {
        for (con_norm y = 0; y < sink->getSampleHeight(); y+=sink->getSampleHeightStep()) {
            for (con_norm x = 0; x < sink->getSampleWidth(); x+=sink->getSampleWidthStep()) {
                sink->writeSample(x, y, source->readSample(x, y));
            }
        }
    }

    template<typename CH, typename CO, typename A>
    _cc<CH,CO,A> get_cpix(CH ch, CO co, A a) {
        _cc<CH,CO,A> pix;
        pix.ch = ch;
        pix.co = co;
        pix.a = a;
        return pix;
    }

    template<template<typename> typename T, typename P>
    _2d<P> length(T<P> buffer) {
        return _2d<P>(buffer.width, buffer.height);
    }
};
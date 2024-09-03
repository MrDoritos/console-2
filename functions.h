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

    template<typename T1, typename T2>
    void copyTo(T1* source, T2* sink, sizef size) {
        for (con_norm y = 0; y < size.height; y+=sink->getSampleHeightStep()) {
            for (con_norm x = 0; x < size.width; x+=sink->getSampleWidthStep()) {
                sink->writeSample(x + size.x, y + size.y, source->readSample(x / size.width, y / size.width));
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

    size_t getLength(const con_wide *str) {
        return wcslen(str);
    }

    size_t getLength(const con_basic *str) {
        return strlen(str);
    }

    template<typename T>
    size_t getLength(const T *str) {
        int i = 0;
        while (*(char*)(str + i) != 0) {
            i++;
        }
        return i;
    }
};
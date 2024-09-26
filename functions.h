#pragma once
#include "interfaces.h"
#include "templates.h"

namespace cons {
    const sizef sample = {0,0,1,1};

    //template<template<typename> typename T1, template<typename> typename T2,typename DT>
    //void copyTo(T1<DT>* source, T2<DT>* sink) {
    template<typename T1, typename T2>
    void copyTo(T1* source, T2* sink) {
        for (con_norm y = 0.0; y < sink->getSampleHeight(); y+=sink->getSampleHeightStep()) {
            for (con_norm x = 0.0; x < sink->getSampleWidth(); x+=sink->getSampleWidthStep()) {
                sink->writeSample(x, y, source->readSample(x, y));
            }
        }
    }

    /*
    Will be used for fast copying (that's why it's separate from sampleTo)
    */
    template<template<typename> typename T1, template<typename> typename T2,typename DT>
    void copyTo(T1<DT>* source, T2<DT>* sink) {
    //template<typename T1, typename T2>
    //void copyTo(T1* source, T2* sink, sizef size) {
        if (source->getWidth() == sink->getWidth() && source->getHeight() == sink->getHeight()) {
            int width = sink->getWidth();
            size_t size = sizeof(DT);
            DT *buffer = (DT*)alloca(width * size);
            for (con_size y = 0; y < sink->getHeight(); y++) {
                size_t offset = y * width;
                source->read((DT*)buffer, offset, width);
                sink->write((DT*)buffer, offset, width);
            }
            return;
        }

        for (con_norm y = 0; y < sink->getSampleHeight(); y+=sink->getSampleHeightStep()) {
            for (con_norm x = 0; x < sink->getSampleWidth(); x+=sink->getSampleWidthStep()) {
                sink->writeSample(x, y, source->readSample(x, y));
            }
        }
    }

    /*
    Will be used for fast copying (that's why it's separate from sampleTo)
    */
    template<template<typename> typename T1, template<typename> typename T2,typename DT>
    void copyTo(T1<DT>* source, T2<DT>* sink, sizef size) {
    //template<typename T1, typename T2>
    //void copyTo(T1* source, T2* sink, sizef size) {
        for (con_norm y = 0; y < size.height; y+=sink->getSampleHeightStep()) {
            for (con_norm x = 0; x < size.width; x+=sink->getSampleWidthStep()) {
                sink->writeSample(x + size.x, y + size.y, source->readSample(x / size.width, y / size.width));
            }
        }
    }

    /*
    Sample from the sink dimensions the source one-by-one
    */
    template<template<typename> typename T1, template<typename> typename T2,typename DT>
    void sampleTo(T1<DT> *source, T2<DT>* sink, sizef size = sample) {
    //template<typename T1, typename T2>
    //void sampleTo(T1 *source, T2* sink, sizef size = sample) {
        copyTo(source, sink, size);
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
        int skip = sizeof(T);
        while (*(char*)(i * skip + str) != 0) {
            i++;
        }
        return i;
    }
};
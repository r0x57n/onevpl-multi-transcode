#ifndef TRANSCODER_H
#define TRANSCODER_H

#include "vpl/mfx.h"
#include "Config.hpp"
// TODO: C headers remove
#include <cstdio>
#include <vector>

// A small subset of the available configuration properties of the dispatcher.
// https://spec.oneapi.io/onevpl/latest/programming_guide/VPL_prg_session.html#onevpl-dispatcher-configuration-properties
enum ConfigProperty {
    HardwareAccelerated = 0,
    SoftwareAccelerated,
};

/*
 * A simple wrapper for the oneVPL API.
 */
class Transcoder {
private:
    Config cfg;
    mfxLoader loader   = NULL;
    std::vector<mfxConfig> configs;
    mfxSession session = NULL;
    mfxVideoParam decodeParam;

    int currentConfig = 0;
public:
    Transcoder(Config cfg);
    int init();
    int addRequirement(ConfigProperty prop);
    int addBitstream();
    int decode();
    int printImplementation();
    int cleanup();

    static void print(mfxLoader loader);
};

#endif

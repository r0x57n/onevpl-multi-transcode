#ifndef TRANSCODER_H
#define TRANSCODER_H

#include <vpl/mfx.h>
#include <cstdio>
#include <vector>
#include "Errors.hpp"
#include "Config.hpp"
#include "Muxer.hpp"

// A small subset of the available configuration properties of the dispatcher.
// https://spec.oneapi.io/onevpl/latest/programming_guide/VPL_prg_session.html#onevpl-dispatcher-configuration-properties
enum ConfigProperty {
    HardwareAccelerated = 0,
    SoftwareAccelerated,
    APIVersion2_2,
};

/*
 * A simple wrapper for the oneVPL API.
 */
class Transcoder {
private:
    Config cfg;
    std::vector<mfxConfig> configs;
    mfxVideoParam decodeParam           = { };
    mfxLoader loader                    = NULL;
    mfxSession session                  = NULL;

public:
    Transcoder(Config cfg);
    int init();
    int addRequirement(ConfigProperty prop);
    int decode();
    int printImplementation();
    int cleanup();

    static void print(mfxLoader loader);
};

#endif

#ifndef TRANSCODER_H
#define TRANSCODER_H

#include <cstdio>
#include <vector>
#include <fstream>
#include <unistd.h>

#include "Errors.hpp"
#include "Config.hpp"

#define WAIT_100_MILLISECONDS       100
#define TARGETKBPS                  4000

// A small subset of the available configuration properties of the dispatcher.
// https://spec.oneapi.io/onevpl/latest/programming_guide/VPL_prg_session.html#onevpl-dispatcher-configuration-properties
enum ConfigProperty {
    HardwareAccelerated = 0,
    SoftwareAccelerated,
    APIVersion2_2,
    HasAvcDecoder,
    HasMpeg2Encoder,
};

class Transcoder {
private:
    Config cfg;
    std::vector<mfxConfig> configs;
    std::vector<mfxSession*> sessions;
    mfxU32 streamSize                   = 0;
    mfxU8* streamData                   = NULL;
    mfxLoader loader                    = NULL;
    mfxVideoParam decodeParams          = { };
    mfxVideoParam encodeParams          = { };
    const int decodeCodec               = MFX_CODEC_AVC;
    const int encodeCodec               = MFX_CODEC_MPEG2;

public:
    Transcoder(Config cfg);
    ~Transcoder();
    int init();
    int setCodecParams(mfxSession* parentSession);
    int initCodec(mfxSession* session);
    int addRequirement(ConfigProperty prop);
    int transcode(int thread = 1);
};

#endif

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
    ApiVersion2_2,
    HasAvcDecoder,
    HasMpeg2Encoder,
};

class Transcoder {
private:
    const int decodeCodec               = MFX_CODEC_AVC;
    const int encodeCodec               = MFX_CODEC_MPEG2;
    Config cfg;
    std::vector<mfxConfig> configs;
    std::vector<mfxSession*> sessions;
    mfxU32 streamSize                   = 0;
    mfxU8* streamData                   = NULL;
    mfxLoader loader                    = NULL;
    mfxVideoParam decodeParams          = { };
    mfxVideoParam encodeParams          = { };

    int initCodec(mfxSession* session);
    int setCodecParams(mfxSession* parentSession);
    int addRequirement(ConfigProperty prop);
public:
    Transcoder(Config cfg);
    int init();
    int transcode(int thread = 1);
    int cleanUp();
};

#endif

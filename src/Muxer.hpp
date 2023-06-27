#ifndef MUXER_H
#define MUXER_H

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/pixdesc.h>
}

#include <vpl/mfx.h>
#include <vector>
#include <fstream>
#include "Config.hpp"
#include "Errors.hpp"

/*
 * Interfaces with libavformat/libavcode to demux the stream.
 * Also has helper functions to translate between libav and oneVPL.
 */
class Muxer {
private:
    Config cfg;
    std::vector<int> streams;
    int videoIndex                  = -1;
    AVFormatContext *inputCtx       = NULL;
    AVFormatContext *outputCtx      = NULL;

    static int libavCodecToMfx(AVCodecID codecId);
    static int libavProfileToMfx(AVCodecID codecId, int profile);
public:
    Muxer(Config cfg);
    int init();
    int demux();
    int getDecodeParam(mfxVideoParam *param);
    int readFrame(AVPacket *frame);
    int cleanup();
};

#endif

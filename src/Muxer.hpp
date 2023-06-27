#ifndef MUXER_H
#define MUXER_H

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
}

#include "Config.hpp"
#include <vector>


class Muxer {
private:
    Config cfg;
    AVFormatContext *inputCtx = NULL;
    AVFormatContext *outputCtx = NULL;
    AVCodec *codec = NULL;
    AVCodecParameters *codecParams = NULL;
    AVCodecContext *codecContext = NULL;
    std::vector<int> streams;
public:
    int videoIndex = -1;

    Muxer(Config cfg);
    int init();
    int readFrame(AVPacket *frame);
    int cleanup();
};

#endif

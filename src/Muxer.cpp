#include "Muxer.hpp"

Muxer::Muxer(Config cfg) : cfg(cfg) {
    if (!cfg.verbose) {
        av_log_set_level(AV_LOG_QUIET);
    }
}

int Muxer::init() {
    inputCtx = avformat_alloc_context();

    // Read format context from file
    int ret = avformat_open_input(&inputCtx, cfg.filename.c_str(), NULL, NULL);
    if (ret < 0) {
        return -1;
    }

    // Create context for the output file
    avformat_alloc_output_context2(&outputCtx, NULL, NULL, "/tmp/test.ts");

    // Read in all streams
    if (avformat_find_stream_info(inputCtx,  NULL) < 0) {
        return -1;
    }

    // We gather all streams of interest for easier access later
    streams.resize(inputCtx->nb_streams);
    for (int i = 0; i < inputCtx->nb_streams; i++) {
        AVStream *outStream;
        AVStream *inStream = inputCtx->streams[i];

        codecParams = inputCtx->streams[i]->codecpar;

        if (codecParams->codec_type != AVMEDIA_TYPE_AUDIO &&
            codecParams->codec_type != AVMEDIA_TYPE_VIDEO &&
            codecParams->codec_type != AVMEDIA_TYPE_SUBTITLE) 
        {
            streams[i] = -1;
            continue;
        }

        // Create streams for the output for after the decoding
        outStream = avformat_new_stream(outputCtx, NULL);
        avcodec_parameters_copy(outStream->codecpar, codecParams);
    }

    return 0;
}

int Muxer::readFrame(AVPacket *frame) {
    return av_read_frame(inputCtx, frame);
}

int Muxer::cleanup() {
    avformat_close_input(&inputCtx);
    avcodec_free_context(&codecContext);

    return 0;
}

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

    // Allocate memory for the output
    avformat_alloc_output_context2(&outputCtx, NULL, NULL, "/tmp/test.ts");

    // Read in all streams
    if (avformat_find_stream_info(inputCtx,  NULL) < 0) {
        return -1;
    }

    // We create an output stream for every input stream of interest (audio/video/subtitles)
    streams.resize(inputCtx->nb_streams);
    for (int i = 0; i < inputCtx->nb_streams; i++) {
        AVStream *outStream;
        AVStream *inStream = inputCtx->streams[i];
        AVCodecParameters *codecParams = inputCtx->streams[i]->codecpar;

        if (codecParams->codec_type != AVMEDIA_TYPE_AUDIO &&
            codecParams->codec_type != AVMEDIA_TYPE_VIDEO &&
            codecParams->codec_type != AVMEDIA_TYPE_SUBTITLE) 
        {
            streams[i] = -1;
            continue;
        }

        if (codecParams->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;
        }

        outStream = avformat_new_stream(outputCtx, NULL);
        avcodec_parameters_copy(outStream->codecpar, codecParams);
    }

    avformat_close_input(&inputCtx);

    return 0;
}

int Muxer::demux() {
    std::string base = "ffmpeg -y -hide_banner -nostats -loglevel warning -i " + cfg.filename + " ";
    std::string op;

    op = base + "-map 0:0 -codec copy " + cfg.demuxedVideo;
    system(op.c_str());
    op = base + "-map 0:1 -codec copy " + cfg.demuxedAudio;
    system(op.c_str());

    return 0;
}

int Muxer::getDecodeParam(mfxVideoParam *param) {
    const AVPixFmtDescriptor *desc;
    mfxStatus status                = MFX_ERR_NONE;
    //AVCodecParameters *codecParams  = inputCtx->streams[videoIndex]->codecpar;

    // TODO: dont hardcode
    /*std::string codec_name = "mpeg2video";
    AVCodec *video_avc = avcodec_find_encoder_by_name(codec_name.c_str());
    AVCodecContext *avCtx = avcodec_alloc_context3(video_avc);*/

    /*param->mfx.CodecId      = libavCodecToMfx(avCtx->codec_id);
    param->IOPattern        = MFX_IOPATTERN_OUT_VIDEO_MEMORY;*/
    //MFXVideoDECODE_DecodeHeader(s)

    /*param->AllocId       = 0;
    param->AsyncDepth    = 0;
    param->Protected     = 0;
    param->IOPattern     = MFX_IOPATTERN_OUT_SYSTEM_MEMORY;
    param->ExtParam      = 0;
    param->NumExtParam   = 0;

    param->mfx.CodecId         = libavCodecToMfx(avCtx->codec_id);
    param->mfx.CodecProfile    = libavProfileToMfx(avCtx->codec_id, codecParams->profile);
    param->mfx.CodecLevel      = avCtx->level == FF_LEVEL_UNKNOWN ? MFX_LEVEL_UNKNOWN : avCtx->level;
    param->mfx.Quality         = 50;

    desc = av_pix_fmt_desc_get(avCtx->sw_pix_fmt);

    param->mfx.FrameInfo.BitDepthLuma    = desc->comp[0].depth;
    param->mfx.FrameInfo.BitDepthChroma  = desc->comp[0].depth;
    param->mfx.FrameInfo.Shift           = desc->comp[0].depth > 8;
    param->mfx.FrameInfo.FourCC          = 0;
    param->mfx.FrameInfo.Width           = avCtx->coded_width;
    param->mfx.FrameInfo.Height          = avCtx->coded_height;
    param->mfx.FrameInfo.ChromaFormat    = MFX_CHROMAFORMAT_YUV420;

    switch (codecParams->field_order) {
        case AV_FIELD_PROGRESSIVE:
            param->mfx.FrameInfo.PicStruct = MFX_PICSTRUCT_PROGRESSIVE;
            break;
        case AV_FIELD_TT:
            param->mfx.FrameInfo.PicStruct = MFX_PICSTRUCT_FIELD_TFF;
            break;
        case AV_FIELD_BB:
            param->mfx.FrameInfo.PicStruct = MFX_PICSTRUCT_FIELD_BFF;
            break;
        default:
            param->mfx.FrameInfo.PicStruct = MFX_PICSTRUCT_UNKNOWN;
            break;
    }*/

    return 0;
}

int Muxer::readFrame(AVPacket *frame) {
    return av_read_frame(inputCtx, frame);
}

int Muxer::cleanup() {
    //avformat_close_input(&inputCtx);

    return 0;
}


/*
 * UTILS
 */

int Muxer::libavCodecToMfx(AVCodecID codecId) {
    switch (codecId) {
        case AV_CODEC_ID_H264:
            return MFX_CODEC_AVC;
        case AV_CODEC_ID_HEVC:
            return MFX_CODEC_HEVC;
        case AV_CODEC_ID_MPEG2VIDEO:
            return MFX_CODEC_MPEG2;
        default:
            printf("No such codec specified as mfx: %d\n", codecId);
            return -1;
    }
}

int Muxer::libavProfileToMfx(AVCodecID codecId, int profile) {
    if (FF_PROFILE_UNKNOWN) {
        return MFX_PROFILE_UNKNOWN;
    }

    switch (codecId) {
        case AV_CODEC_ID_HEVC:
        case AV_CODEC_ID_H264:
            return profile;
        case AV_CODEC_ID_MPEG2VIDEO:
            return 0x10 * profile;
        default:
            return MFX_PROFILE_UNKNOWN;
    }
}

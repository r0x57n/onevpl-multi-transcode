#include "Transcoder.hpp"

Transcoder::Transcoder(Config cfg) : cfg(cfg) { }

int Transcoder::init() {
    mfxStatus status = MFX_ERR_NONE;
    mfxSession* parentSession = new mfxSession;

    loader = MFXLoad();
    if (loader == NULL) {
        printf("MFXLoad failed.\n");
        return -1;
    }
    
    // Add requirements for the implementation we want.
    addRequirement(HasAvcDecoder);
    addRequirement(HasMpeg2Encoder);
    addRequirement(ApiVersion2_2);

    if (cfg.swi) {
        addRequirement(SoftwareAccelerated);
    } else {
        addRequirement(HardwareAccelerated);
    }

    // Create sessions
    for (int i = 0; i < cfg.threads; i++) {
        mfxSession* session = new mfxSession;
        status = MFXCreateSession(loader, 0, session);
        if (status != MFX_ERR_NONE) {
            sessionError(status);
            return -1;
        }

        // Initialize codec params if it's the first session (e.g. parent session).
        if (i == 0) {
            parentSession = session;
            if (setCodecParams(session) < 0) {
                return -1;
            }
        } else {
            MFXJoinSession(*parentSession, *session);
        }

        sessions.push_back(session);

        if (initCodec(session) < 0) {
            return -1;
        }
    }

    if (!cfg.quiet)
        printImplementationInfo();

    return 0;
}

int Transcoder::addRequirement(ConfigProperty prop) {
    mfxU8* name;
    mfxVariant value;
    mfxStatus status = MFX_ERR_NONE;

    // One configuration object can handle only one filter property.
    mfxConfig config = MFXCreateConfig(loader);
    if(config == NULL) {
        printf("MFXCreateConfig failed.\n");
        return -1;
    }
    this->configs.push_back(config);

    switch(prop) {
        case HardwareAccelerated:
            name            = (mfxU8*)"mfxImplDescription.Impl";
            value.Type      = MFX_VARIANT_TYPE_U32;
            value.Data.U32  = MFX_IMPL_TYPE_HARDWARE;
            break;
        case SoftwareAccelerated:
            name            = (mfxU8*)"mfxImplDescription.Impl";
            value.Type      = MFX_VARIANT_TYPE_U32;
            value.Data.U32  = MFX_IMPL_TYPE_SOFTWARE;
            break;
        case ApiVersion2_2:
            name            = (mfxU8*)"mfxImplDescription.ApiVersion.Version";
            value.Type      = MFX_VARIANT_TYPE_U32;
            value.Data.U32  = (2 << 16 | 2);
            break;
        case HasAvcDecoder:
            name            = (mfxU8*)"mfxImplDescription.mfxDecoderDescription.decoder.CodecID";
            value.Type      = MFX_VARIANT_TYPE_U32;
            value.Data.U32  = MFX_CODEC_AVC;
            break;
        case HasMpeg2Encoder:
            name            = (mfxU8*)"mfxImplDescription.mfxEncoderDescription.encoder.CodecID";
            value.Type      = MFX_VARIANT_TYPE_U32;
            value.Data.U32  = MFX_CODEC_MPEG2;
            break;
        default:
            printf("No such requirement exists in this wrapper.\n");
            return -1;
    }

    status = MFXSetConfigFilterProperty(this->configs.back(), name, value);
    if(status != MFX_ERR_NONE) {
        printf("Couldn't add requirement: %d\n", status);
        return -1;
    }

    return 0;
}

int Transcoder::setCodecParams(mfxSession* session) {
    mfxStatus status = MFX_ERR_NONE;

    // Read in size of file and create a reading buffer to store it in memory (for bitstreams).
    std::ifstream file(cfg.inputFile, std::ios::binary | std::ios::ate);
    streamSize = file.tellg();
    file.seekg(0, std::ios::beg);
    streamData = (mfxU8*)calloc(streamSize, sizeof(mfxU8));
    file.read(reinterpret_cast<char*>(streamData), streamSize);
    file.close();

    // Set shared decode parameters.
    decodeParams.mfx.CodecId    = decodeCodec;
    decodeParams.IOPattern      = MFX_IOPATTERN_OUT_VIDEO_MEMORY;

    // We create a temporary bitstream just to read header from.
    mfxBitstream headerStream   = { };
    headerStream.CodecId        = decodeCodec;
    headerStream.MaxLength      = streamSize;
    headerStream.DataLength     = streamSize;
    headerStream.Data           = streamData;

    // Read in params from header in input file.
    status = MFXVideoDECODE_DecodeHeader(*session, &headerStream, &decodeParams);
    if (status != MFX_ERR_NONE) {
        decodeHeaderError(status);
        return -1;
    }
    
    // Set shared encoding parameters.
    encodeParams.IOPattern                      = MFX_IOPATTERN_IN_VIDEO_MEMORY;
    encodeParams.mfx.CodecId                    = encodeCodec;
    encodeParams.mfx.TargetUsage                = MFX_TARGETUSAGE_BALANCED;
    encodeParams.mfx.TargetKbps                 = TARGETKBPS;
    encodeParams.mfx.RateControlMethod          = MFX_RATECONTROL_VBR;
    encodeParams.mfx.FrameInfo.FrameRateExtN    = decodeParams.mfx.FrameInfo.FrameRateExtN;
    encodeParams.mfx.FrameInfo.FrameRateExtD    = decodeParams.mfx.FrameInfo.FrameRateExtD;
    encodeParams.mfx.FrameInfo.FourCC           = decodeParams.mfx.FrameInfo.FourCC;
    encodeParams.mfx.FrameInfo.ChromaFormat     = decodeParams.mfx.FrameInfo.ChromaFormat;
    encodeParams.mfx.FrameInfo.CropW            = decodeParams.mfx.FrameInfo.CropW;
    encodeParams.mfx.FrameInfo.CropH            = decodeParams.mfx.FrameInfo.CropH;
    encodeParams.mfx.FrameInfo.Width            = decodeParams.mfx.FrameInfo.Width;
    encodeParams.mfx.FrameInfo.Height           = decodeParams.mfx.FrameInfo.Height;

    return 0;
}

int Transcoder::initCodec(mfxSession* session) {
    mfxStatus status = MFX_ERR_NONE;

    status = MFXVideoDECODE_Init(*session, &decodeParams);
    if (status != MFX_ERR_NONE) {
        codecInitError("decoder", status);
        return -1;
    }

    status = MFXVideoENCODE_Init(*session, &encodeParams);
    if (status != MFX_ERR_NONE) {
        codecInitError("encoder", status);
        return -1;
    }

    return 0;
}

void Transcoder::printImplementationInfo() {
    mfxIMPL impl;
    mfxVersion version = { 0, 1 };

    mfxStatus sts = MFXQueryIMPL(*sessions[0], &impl);

    sts = MFXQueryVersion(*sessions[0], &version);

    printf("Session loaded:\n \tApiVersion = %d.%d\n \timplementation = ", version.Major, version.Minor);

    switch (impl) {
        case MFX_IMPL_SOFTWARE:
            printf("Software\n");
            break;
        case MFX_IMPL_HARDWARE | MFX_IMPL_VIA_VAAPI:
            printf("Hardware:VAAPI\n");
            break;
        case MFX_IMPL_HARDWARE | MFX_IMPL_VIA_D3D11:
            printf("Hardware:D3D11\n");
            break;
        case MFX_IMPL_HARDWARE | MFX_IMPL_VIA_D3D9:
            printf("Hardware:D3D9\n");
            break;
        default:
            printf("Unknown\n");
            break;
    }
}


int Transcoder::transcode(int thread) {
    bool transcoding                    = true;
    bool drainingDecoder                = false;
    bool drainingEncoder                = false;
    int encodedFramesCount              = 0;
    mfxStatus status                    = MFX_ERR_NONE;
    mfxStatus status_r                  = MFX_ERR_NONE;
    mfxFrameSurface1* decodeSurface     = NULL;
    mfxSyncPoint syncp                  = { };
    mfxBitstream decodeStream           = { };
    mfxBitstream encodedStream          = { };
    const std::string outputFile        = cfg.outputDir + "/t" + std::to_string(thread) + "_" + cfg.outputFile;
    std::ofstream output(outputFile, std::ios::binary);

    // We need separate bitstreams for each thread, but they can all point to the same (reading) data ...
    decodeStream.CodecId                = decodeCodec;
    decodeStream.MaxLength              = streamSize;
    decodeStream.DataLength             = streamSize;
    decodeStream.Data                   = streamData;
    // ... however, they'll need a unique buffer to write to.
    encodedStream.MaxLength             = streamSize;
    encodedStream.Data                  = (mfxU8*)calloc(encodedStream.MaxLength, sizeof(mfxU8));

    while (transcoding) {
        if (!drainingEncoder) { // when decoding has finished we'll only be draining the encoder
            status = MFXVideoDECODE_DecodeFrameAsync(*sessions[thread],
                                                    (drainingDecoder == true) ? NULL : &decodeStream,
                                                    NULL,
                                                    &decodeSurface,
                                                    &syncp);

            switch (status) {
                case MFX_ERR_NONE: // a frame is ready for encoding
                    break;
                case MFX_ERR_MORE_DATA: // needs more data from bitstream to decode
                    // Since the bitstream contains the full stream to be decoded
                    // this should mean the decoding has finished. We drain remaining
                    // cached frames from the decoder.
                    if (!drainingDecoder) {
                        drainingDecoder = true;
                    } else { 
                        // We've drained what we can from the decoder. Drain the encoder instead.
                        drainingEncoder = true;
                    }
                    break;
                case MFX_WRN_VIDEO_PARAM_CHANGED: // probably a bad header
                    if (!cfg.quiet)
                        printf("[%d] New sequence header found while decoding, ignoring frame...\n", thread);
                    continue;
                    break;
                default:
                    if (!cfg.quiet)
                        decodingError(status, thread);

                    transcoding = false;
                    break;
            }
        }

        status = MFXVideoENCODE_EncodeFrameAsync(*sessions[thread],
                                                NULL,
                                                (drainingEncoder == true) ? NULL : decodeSurface,
                                                &encodedStream,
                                                &syncp);

        // Release surfaces no longer in use.
        if (!drainingEncoder && decodeSurface) {
            status_r = decodeSurface->FrameInterface->Release(decodeSurface);
            if (status_r != MFX_ERR_NONE) {
                printf("[%d] Couldn't release surface...\n", thread);
            }
        }

        switch (status) {
            case MFX_ERR_NONE: // function completed succesfully
                if (syncp) {
                    // Encoded output is not available on CPU until sync operation completes.
                    status = MFXVideoCORE_SyncOperation(*sessions[thread], syncp, WAIT_100_MILLISECONDS);
                    if (status != MFX_ERR_NONE && !cfg.quiet)
                        syncError(status, thread);

                    // Write encoded stream to output file.
                    output.write(reinterpret_cast<char*>(encodedStream.Data + encodedStream.DataOffset), encodedStream.DataLength);
                    encodedStream.DataLength = 0;
                    encodedFramesCount++;
                }
                break;
            case MFX_ERR_MORE_DATA: // more data needed to generate output
                if (drainingEncoder == true) {
                    // No more data to drain from encoder, we're finished.
                    transcoding = false;
                }
                break;
            default:
                if (!cfg.quiet)
                    encodingError(status, thread);

                transcoding = false;
                break;
        }
    }

    free(encodedStream.Data);
    output.close();
    
    if (!cfg.quiet)
        printf("[%d] Finished, encoded %d frames.\n", thread, encodedFramesCount);

    return 0;
}

int Transcoder::cleanUp() {
    for (int i = cfg.threads - 1; i >= 0; i--) {
        MFXDisjoinSession(*sessions[i]);
        MFXClose(*sessions[i]);
        sessions.pop_back();
    }

    MFXUnload(loader);
    free(streamData);

    return 0;
}


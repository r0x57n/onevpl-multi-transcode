#include "Transcoder.hpp"

Transcoder::Transcoder(Config cfg) : cfg(cfg) { }

int Transcoder::init() {
    mfxStatus status = MFX_ERR_NONE;

    loader = MFXLoad();
    if(loader == NULL) {
        printf("MFXLoad failed.\n");
        return -1;
    }

    if (cfg.hwa) {
        addRequirement(HardwareAccelerated);
    } else {
        addRequirement(SoftwareAccelerated);
    }

    addRequirement(APIVersion2_2);

    status = MFXCreateSession(this->loader, 0, &this->session);
    if(status != MFX_ERR_NONE) {
        printf("Can't connect, couldn't create session: %d\n", status);
        return -1;
    }

    if (cfg.verbose) {
        printImplementation();
    }

    // Init decoder 
    mfxBitstream bs;
    std::ifstream file(cfg.demuxedVideo, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    bs.CodecId = MFX_CODEC_AVC;
    bs.MaxLength = 2000000;
    bs.DataLength = size;
    bs.Data = (mfxU8*)calloc(bs.MaxLength, sizeof(mfxU8));
    file.read(reinterpret_cast<char*>(bs.Data), size);

    decodeParam.mfx.CodecId = MFX_CODEC_AVC;
    decodeParam.IOPattern = MFX_IOPATTERN_OUT_VIDEO_MEMORY;
    status = MFXVideoDECODE_DecodeHeader(session, &bs, &decodeParam);
    if (status != MFX_ERR_NONE) {
        decodeHeaderError(status);
        return -1;
    }

    file.close();

    status = MFXVideoDECODE_Init(session, &decodeParam);
    if (status != MFX_ERR_NONE) {
        decodeInitError(status);
        return -1;
    }

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
    }
    this->configs.push_back(config);

    switch(prop) {
        case HardwareAccelerated:
            name = (mfxU8*)"mfxImplDescription.Impl";
            value.Type = MFX_VARIANT_TYPE_U32;
            value.Data.U32 = MFX_IMPL_HARDWARE;
            break;
        case SoftwareAccelerated:
            name = (mfxU8*)"mfxImplDescription.Impl";
            value.Type = MFX_VARIANT_TYPE_U32;
            value.Data.U32 = MFX_IMPL_SOFTWARE;
            break;
        case APIVersion2_2:
            name = (mfxU8*)"mfxImplDescription.ApiVersion.Version";
            value.Type     = MFX_VARIANT_TYPE_U32;
            value.Data.U32 = (2 << 16 | 2);
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

int Transcoder::decode() {
    mfxStatus status = MFX_ERR_MORE_DATA;

    mfxFrameSurface1 *surfaceOut;
    mfxSyncPoint syncp;

    for (;;) {
           //status = MFXVideoDECODE_DecodeFrameAsync(session, bitstream, NULL, &surfaceOut, &syncp);
           if (status == MFX_ERR_NONE) {
              /*disp->FrameInterface->Synchronize(disp, INFINITE); // or MFXVideoCORE_SyncOperation(session,syncp,INFINITE)
              do_something_with_decoded_frame(disp);
              disp->FrameInterface->Release(disp);*/
           } else {
               decodeError(status);
               return 1;
           }
    }

    MFXVideoDECODE_Close(session);
}

int Transcoder::printImplementation() {
    mfxStatus status = MFX_ERR_NONE;
    mfxImplDescription* implementation;

    status = MFXEnumImplementations(this->loader, 0, MFX_IMPLCAPS_IMPLDESCSTRUCTURE, (mfxHDL *)&implementation);
    if(status != MFX_ERR_NONE) {
        printf("Couldn't fetch current implementation: %d\n", status);
    }

    printf("----------------------\n");
    printf("Implementation\n");
    printf("----------------------\n");
    printf("Version: %d.%d\n", implementation->Version.Major,
                               implementation->Version.Minor);
    printf("API Version: %d.%d\n", implementation->ApiVersion.Major,
                                   implementation->ApiVersion.Minor);
    printf("HWA: ");
    switch(implementation->Impl) {
        case MFX_IMPL_TYPE_HARDWARE:
            printf("On\n");
            break;
        default:
            printf("Off\n");
            break;
    }
    printf("AccelerationMode via: ");
    switch (implementation->AccelerationMode) {
        case MFX_ACCEL_MODE_NA:
            printf("NA\n");
            break;
        case MFX_ACCEL_MODE_VIA_D3D9:
            printf("D3D9\n");
            break;
        case MFX_ACCEL_MODE_VIA_D3D11:
            printf("D3D11\n");
            break;
        case MFX_ACCEL_MODE_VIA_VAAPI:
            printf("VAAPI\n");
            break;
        case MFX_ACCEL_MODE_VIA_VAAPI_DRM_MODESET:
            printf("VAAPI_DRM_MODESET\n");
            break;
        case MFX_ACCEL_MODE_VIA_VAAPI_GLX:
            printf("VAAPI_GLX\n");
            break;
        case MFX_ACCEL_MODE_VIA_VAAPI_X11:
            printf("VAAPI_X11\n");
            break;
        case MFX_ACCEL_MODE_VIA_VAAPI_WAYLAND:
            printf("VAAPI_WAYLAND\n");
            break;
        case MFX_ACCEL_MODE_VIA_HDDLUNITE:
            printf("HDDLUNITE\n");
            break;
        default:
            printf("unknown\n");
            break;
    }
    printf("DeviceID: %s\n", implementation->Dev.DeviceID);
    printf("----------------------\n");
    fflush(stdout);

    MFXDispReleaseImplDescription(loader, implementation);

    return 0;
}

int Transcoder::cleanup() {
    MFXClose(this->session);
    MFXUnload(loader);
    return 0;
}

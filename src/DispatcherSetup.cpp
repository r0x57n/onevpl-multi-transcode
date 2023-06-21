#include "DispatcherSetup.hpp"

DispatcherSetup::DispatcherSetup() {
    loader = MFXLoad();
    if(loader == NULL) {
        printf("MFXLoad failed.\n");
    }

    // Initialize config
    config = MFXCreateConfig(loader);
    if(config == NULL) {
        printf("MFXCreateConfig failed.\n");
    }
}

int DispatcherSetup::addRequirement(ConfigProperty prop) {
    mfxU8* name;
    mfxVariant value;
    mfxStatus status = MFX_ERR_NONE;

    switch(prop) {
        case HardwareAccelerated:
            name = (mfxU8*)"mfxImplDescription.Impl";
            value.Type = MFX_VARIANT_TYPE_U32;
            value.Data.U32 = MFX_IMPL_TYPE_HARDWARE;
            break;
        case SoftwareAccelerated:
            name = (mfxU8*)"mfxImplDescription.Impl";
            value.Type = MFX_VARIANT_TYPE_U32;
            value.Data.U32 = MFX_IMPL_TYPE_SOFTWARE;
            break;
        default:
            printf("No such requirement exists in this wrapper.\n");
            return 1;
    }

    status = MFXSetConfigFilterProperty(this->config, name, value);
    if(status != MFX_ERR_NONE) {
        printf("Couldn't add requirement: %d\n", status);
        return 1;
    }

    return 0;
}

int DispatcherSetup::connect() {
    mfxStatus status = MFX_ERR_NONE;

    status = MFXCreateSession(this->loader, 0, &this->session);
    if(status != MFX_ERR_NONE) {
        printf("Can't connect, couldn't create session: %d\n", status);
        return 1;
    }

    return 0;
}

int DispatcherSetup::printImplementation() {
    mfxStatus status = MFX_ERR_NONE;
    mfxImplDescription* implementation;

    status = MFXEnumImplementations(loader, 0, MFX_IMPLCAPS_IMPLDESCSTRUCTURE, (mfxHDL *)&implementation);
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

int DispatcherSetup::cleanup() {
    MFXClose(this->session);
    MFXUnload(loader);
    return 0;
}

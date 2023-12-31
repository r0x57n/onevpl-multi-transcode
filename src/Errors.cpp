#include "Errors.hpp"
#include <vpl/mfxdefs.h>

void decodeHeaderError(mfxStatus sts) {
    printf("Couldn't decode header: ");
    
    switch (sts) {
        case MFX_ERR_MORE_DATA:
            printf("The function requires more bitstream data.\n");
            break;
        case MFX_ERR_UNSUPPORTED:
            printf("CodecId field of the mfxVideoParam structure indicates some unsupported codec.\n");
            break;
        case MFX_ERR_INVALID_HANDLE:
            printf("Session is not initialized.\n");
            break;
        case MFX_ERR_NULL_PTR:
            printf("bitstream or video param pointer is NULL.\n");
            break;
        default:
            printf("Unknown error: %d\n", sts);
            break;
    }
}

void codecInitError(std::string codec, mfxStatus sts) {
    printf("Couldn't initialize %s: ", codec.c_str());

    switch (sts) {
        case MFX_ERR_INVALID_VIDEO_PARAM:
            printf("Invalid video parameters; they may be out of the valid range, or the combination of them resulted in incompatibility.\n");
            break;
        case MFX_WRN_PARTIAL_ACCELERATION:
            printf("The underlying hardware does not fully support the specified video parameters. The encoding may be partially accelerated. Only hardware implementations may return this status code.\n");
            break;
        case MFX_WRN_INCOMPATIBLE_VIDEO_PARAM:
            printf("The function detected some video parameters were incompatible with others; incompatibility resolved.\n");
            break;
        case MFX_ERR_UNDEFINED_BEHAVIOR:
            printf("The function is called twice without a close.\n");
            break;
        default:
            printf("Unknown error: %d\n", sts);
            break;
    }
}

void decodingError(mfxStatus sts, int thread) {
    printf("[%d] Decoding error: ", thread);

    switch (sts) {
       case MFX_ERR_NONE:
            printf("The function completed successfully and the output surface is ready for decoding.\n");
            break;
        case MFX_ERR_MORE_DATA:
            printf("The function requires more bitstream at input before decoding can proceed.\n");
            break;
        case MFX_ERR_MORE_SURFACE:
            printf("The function requires more frame surface at output before decoding can proceed.\n");
            break;
        case MFX_ERR_DEVICE_LOST:
            printf("Hardware device was lost.\n");
            break;
        case MFX_WRN_DEVICE_BUSY:
            printf("Hardware device is currently busy. Call this function again after MFXVideoCORE_SyncOperation or in a few milliseconds.\n");
            break;
        case MFX_WRN_VIDEO_PARAM_CHANGED:
            printf("The decoder detected a new sequence header in the bitstream. Video parameters may have changed.\n");
            break;
        case MFX_ERR_INCOMPATIBLE_VIDEO_PARAM:
            printf("The decoder detected incompatible video parameters in the bitstream and failed to follow them.\n");
            break;
        case MFX_ERR_REALLOC_SURFACE:
            printf("Bigger surface_work required. May be returned only if mfxInfoMFX::EnableReallocRequest was set to ON during initialization.\n");
            break;
        case MFX_WRN_ALLOC_TIMEOUT_EXPIRED:
            printf("Timeout expired for internal output frame allocation (if set with mfxExtAllocationHints and NULL passed as surface_work). Repeat the call in a few milliseconds or re-initialize decoder with higher surface limit.\n");
            break;
        case MFX_ERR_NOT_INITIALIZED:
            printf("Decoder not initialized.\n");
            break;
        default:
            printf("Unknown error: %d\n", sts);
            break;
   }
}

void encodingError(mfxStatus sts, int thread) {
    printf("[%d] Encoding error: ", thread);

    switch (sts) {
        case MFX_ERR_NOT_ENOUGH_BUFFER:
            printf("The bitstream buffer size is insufficient.");
            break;
        case MFX_ERR_MORE_DATA:
            printf("The function requires more data to generate any output.");
            break;
        case MFX_ERR_DEVICE_LOST:
            printf("Hardware device was lost. See the Working with Microsoft* DirectX* Applications section for further information.");
            break;
        case MFX_WRN_DEVICE_BUSY:
            printf("Hardware device is currently busy. Call this function again after MFXVideoCORE_SyncOperation or in a few milliseconds.");
            break;
        case MFX_ERR_INCOMPATIBLE_VIDEO_PARAM:
            printf("Inconsistent parameters detected not conforming to Configuration Parameter Constraints.");
            break;
        default:
            printf("Unknown error: %d\n", sts);
            break;
    }
}

void sessionError(mfxStatus sts) {
    printf("Couldn't create session: ");

    switch (sts) {
        case MFX_ERR_NULL_PTR:
            printf("loader/session is null.\n");
            break;
        case MFX_ERR_NOT_FOUND:
            printf("Provided index is out of possible range (requirements not fulfilled).\n");
            break;
        default:
            printf("Unknown error: %d\n", sts);
            break;
    }
}

void syncError(mfxStatus sts, int thread) {
    printf("[%d] Syncoperation failed: ", thread);

    switch (sts) {
        case MFX_ERR_NONE_PARTIAL_OUTPUT:
            printf("The function completed successfully, bitstream contains a portion of the encoded frame according to required granularity.\n");
            break;
        case MFX_WRN_IN_EXECUTION:
            printf("The specified asynchronous function is in execution.\n");
            break;
        case MFX_ERR_ABORTED:
            printf("The specified asynchronous function aborted due to data dependency on a previous asynchronous function that did not complete.\n");
            break;
        case MFX_ERR_DEVICE_FAILED:
            printf("Device operation failure.\n");
            break;
        default:
            printf("Unknown error: %d\n", sts);
            break;
    }
}

#include <vpl/mfx.h>
#include <cstdio>
#include <string>

void sessionError(mfxStatus sts);
void decodeHeaderError(mfxStatus sts);
void codecInitError(std::string codec, mfxStatus sts);
void decodingError(mfxStatus sts, int thread);
void encodingError(mfxStatus sts, int thread);
void syncError(mfxStatus sts, int thread);

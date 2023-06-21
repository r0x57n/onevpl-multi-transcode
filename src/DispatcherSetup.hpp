#include "vpl/mfxvideo.h"
#include "vpl/mfxdispatcher.h"
#include <stddef.h>
#include <stdio.h>

// A small subset of the available configuration properties of the dispatcher.
// https://spec.oneapi.io/onevpl/latest/programming_guide/VPL_prg_session.html#onevpl-dispatcher-configuration-properties
enum ConfigProperty {
    HardwareAccelerated = 0,
};

/*
 * A simple wrapper for the oneVPL API.
 */
class DispatcherSetup {
public:
    mfxLoader loader   = NULL;
    mfxConfig config   = NULL;
    mfxSession session = NULL;

    DispatcherSetup();
    int addRequirement(ConfigProperty prop);
    int connect();
    int printImplementation();
    int cleanup();
private:
};

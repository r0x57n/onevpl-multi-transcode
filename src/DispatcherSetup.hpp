#include "vpl/mfx.h"
// TODO: C headers remove
#include <cstdio>
#include <vector>

// A small subset of the available configuration properties of the dispatcher.
// https://spec.oneapi.io/onevpl/latest/programming_guide/VPL_prg_session.html#onevpl-dispatcher-configuration-properties
enum ConfigProperty {
    HardwareAccelerated = 0,
    SoftwareAccelerated,
};

/*
 * A simple wrapper for the oneVPL API.
 */
class DispatcherSetup {
private:
    mfxLoader loader   = NULL;
    std::vector<mfxConfig> configs;
    mfxSession session = NULL;

    int currentConfig = 0;
public:
    DispatcherSetup();
    int addRequirement(ConfigProperty prop);
    int connect();
    int printImplementation();
    int cleanup();

    static void print(mfxLoader loader);
};

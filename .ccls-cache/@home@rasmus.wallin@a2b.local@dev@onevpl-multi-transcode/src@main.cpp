#include "DispatcherSetup.hpp"

int main(int argc, char* argv[]) {
    DispatcherSetup setup;

    // Set requirements of the wanted implementation
    //setup.addRequirement(HardwareAccelerated);
    //setup.addRequirement(SoftwareAccelerated);

    if(setup.connect()) {
        return 1;
    }

    setup.printImplementation();

    setup.cleanup();
}

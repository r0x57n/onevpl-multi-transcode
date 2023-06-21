#include "DispatcherSetup.hpp"

int main(int argc, char* argv[]) {
    DispatcherSetup setup;

    // Set requirements of the wanted implementation
    setup.addRequirement(HardwareAccelerated);

    setup.connect();

    setup.printImplementation();

    setup.cleanup();
}

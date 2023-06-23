#include <string>
#include <iostream>
#include <fstream>

#include "DispatcherSetup.hpp"
#include "Demuxer.hpp"

int main(int argc, char* argv[]) {
    bool verbose = false;
    bool hwa = false;
    int streams = 1;
    std::string filename;

    // Parse args
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg[0] == '-') {
           switch(arg[1]) {
                case 'h':
                   hwa = true;
                   break;
                case 'n':
                   streams = std::stoi(argv[i+1]);
                   i++;
                   break;
                default:
                   printf("Unknown flag: -%c\n", arg[1]);
                   return 1;
                   break;
           }
        }
        if (i == argc - 1) {
            filename = arg;
        }
    }

    if (filename.empty()) {
        printf("Enter a filename.\n");
        return 1;
    }

    // Open file
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Couldn't open file: " << filename << std::endl;
    }

    // Demux file
    {
        Demuxer demux(file);
        demux.video();
    }

    file.close();

    return 0;
}

int dispatch() {
    DispatcherSetup setup;

    // Set requirements of the wanted implementation
    //setup.addRequirement(HardwareAccelerated);
    //setup.addRequirement(SoftwareAccelerated);

    if(setup.connect()) {
        return 1;
    }

    setup.printImplementation();

    setup.cleanup();

    return 0;
}

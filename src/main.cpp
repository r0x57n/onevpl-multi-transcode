#include <string>
#include <iostream>
#include <fstream>
#include "Transcoder.hpp"
#include "Muxer.hpp"
#include "Config.hpp"

extern "C" {
    #include <libavcodec/avcodec.h>
}

Config parseArguments(int argc, char* argv[]) {
    Config cfg;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg[0] == '-') {
           switch(arg[1]) {
                case 'h':
                   cfg.hwa = true;
                   break;
                case 'n':
                   cfg.streams = std::stoi(argv[i+1]);
                   i++;
                   break;
                case 'v':
                    cfg.verbose = true;
                    break;
                default:
                   printf("Unknown flag: -%c\n", arg[1]);
                   exit(-1);
                   break;
           }
        }
        if (i == argc - 1) {
            cfg.filename = arg;
        }
    }

    if (cfg.filename.empty()) {
        printf("Enter a filename.\n");
        exit(-1);
    }

    return cfg;
}

int main(int argc, char* argv[]) {
    Config cfg = parseArguments(argc, argv);

    Muxer muxer(cfg);
    if (muxer.init() < 0) {
        printf("Couldn't initialize muxer.\n");
        return -1;
    }

    muxer.demux();

    Transcoder transcoder(cfg);
    if (transcoder.init() < 0) {
        printf("Couldn't initialize transcoder.\n");
        return -1;
    }


    transcoder.cleanup();
    muxer.cleanup();
    
    return 0;
}

#include <thread>
#include <chrono>

#include "Transcoder.hpp"
#include "Config.hpp"

Config parseArguments(int argc, char* argv[]) {
    Config cfg;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg[0] == '-') {
           switch(arg[1]) {
                case 'h':
                    printf("Takes an h264 elementary stream and transcodes it to h262.\n\n");
                    printf("Example usage:\t\t./multi-transcoder -a ./input.264.\n");
                    printf("Expected results:\tA file in the same directory called t0_out.262 that was transcoded with hardware acceleration.\n\n");
                    printf("Flags:\n");
                    printf("-q\tRun as quietly as possible.\n");
                    printf("-a\tTurn on hardware acceleration.\n");
                    printf("-n\tThe number of threads to run (will increase number of output files).\n");
                    printf("-h\tPrint this help message.\n");
                    exit(0);
                    break;
                case 's':
                    cfg.swi = true;
                    break;
                case 'q':
                    cfg.quiet = true;
                    break;
                case 'n':
                    cfg.threads = std::stoi(argv[i+1]);
                    i++;
                    break;
                default:
                    printf("Unknown flag: -%c\n", arg[1]);
                    exit(-1);
                    break;
           }
        }
        if (i == argc - 1) {
            cfg.inputFile = arg;
        }
    }

    if (cfg.inputFile.empty()) {
        printf("Enter a filename.\n");
        exit(-1);
    } else if (cfg.inputFile == "./in.264") {
        printf("Using as input: ./in.264.\n");
    }

    return cfg;
}

int main(int argc, char* argv[]) {
    Config cfg = parseArguments(argc, argv);
    std::vector<std::thread*> threads;

    Transcoder transcoder(cfg);
    if (transcoder.init() < 0) {
        return -1;
    }

    // Create threads for all transcodes.
    for (int i = 0; i < cfg.threads; i++) {
        std::thread* transcodeThread = new std::thread(&Transcoder::transcode, transcoder, i);
        threads.push_back(transcodeThread);

        if (!cfg.quiet)
            printf("[%d] Starting transcoding on thread %d...\n", i, i);
    }

    // Wait for all threads to finish.
    for (auto t : threads) {
        t->join();
    }

    transcoder.cleanUp();
    
    return 0;
}

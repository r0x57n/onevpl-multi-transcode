#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config {
    bool swi                = false;
    int threads             = 1;
    std::string inputFile   = "./in.264";
    std::string outputFile  = "out.262";
};

#endif

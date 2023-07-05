#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config {
    bool quiet              = false;
    bool swi                = false;
    int threads             = 1;
    std::string inputFile   = "./in.264";
    std::string outputDir   = "./";
    std::string outputFile  = "out.m2v";
};

#endif

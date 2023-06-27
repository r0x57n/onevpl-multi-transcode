#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config {
    bool verbose = true;
    bool hwa = true;
    int streams = 1;
    std::string filename = "";
};

#endif

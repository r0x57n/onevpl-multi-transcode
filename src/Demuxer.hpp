#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <stdint.h>
#include <unordered_set>
#include <cstring>

enum PidDescriptors {
    PAT = 0x00,
    PMT = 0x02,
    SDT = 0x11,
};

enum TSHeaderBitmask {
    Sync  = 0xFF000000,
    TEI   = 0x00800000, // Transport error indicator
    PUSI  = 0x00400000, // Payload unit start indicator
    Prio  = 0x00200000, // Transport priority
    PID   = 0x001fff00,
    TSC   = 0x000000c0, // Transport scrambling control
    AFC   = 0x00000030, // Adaptation field control
    CC    = 0x0000000f, // Continuity counter
};

struct TSHeader {
    uint8_t sync;
    uint8_t TEI;
    uint8_t PUSI;
    uint8_t Prio;
    uint16_t PID;
    uint8_t TSC;
    uint8_t AFC;
    uint8_t CC;
};

struct TSTable {
    uint8_t pointer;
    uint8_t tableID;
    uint8_t SYI;
    uint8_t sectionLength;
    uint8_t syntaxSection;
};

class Demuxer {
private:
    std::ifstream& file;
    std::map<int, int> serviceToPMT;
    std::unordered_set<uint16_t> pmtPids;

public:
    Demuxer(std::ifstream& file);
    void video();
    TSHeader parseHeader(uint32_t header);
    void printHeader(int n);
    void scanPids();

    /*
     * This is a very rudimentary demuxer which leads to not taking many thins in consideration.
     * Example, the pointer field is expected to be 0 always.
     */
    void parseTable(uint8_t* payload);

    void parsePAT(uint8_t* payload);
    void debug();
};

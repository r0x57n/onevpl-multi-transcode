#include "Demuxer.hpp"
#include <unordered_set>
#include <cstring>

bool DEBUG = true;

template <typename T>
void log(T t)
{
    if (DEBUG) {
        std::cout << t << std::endl;
    }
}

template<typename T, typename... Args>
void log(T t, Args... args) // recursive variadic function
{
    if (DEBUG) {
        std::cout << t <<std::endl;
    }

    log(args...) ;
}

uint32_t byteswap32(uint32_t b) {
    uint32_t newB = 0;
    newB += (b & 0xFF000000) >> 24;
    newB += (b & 0x00FF0000) >> 8;
    newB += (b & 0x0000FF00) << 8;
    newB += (b & 0x000000FF) << 24;
    return newB;
}

uint32_t byteswap16(uint16_t b) {
    uint16_t newB = 0;
    newB += (b & 0xFF00) >> 8;
    newB += (b & 0x00FF) << 8;
    return newB;
}

Demuxer::Demuxer(std::ifstream& file) : file(file) { }

void Demuxer::video() {
    for(;;) {
        // Find sync byte
        while (!file.eof()) {
            if (file.get() == 'G') { 
                file.unget();
                break;
            }
        }

        // Read the header
        uint32_t header;
        file.read(reinterpret_cast<char*>(&header), sizeof header);
        TSHeader parsed = parseHeader(header);

        // Fetch the payload
        uint8_t payload[184];
        file.read(reinterpret_cast<char*>(&payload), sizeof payload);

        switch(parsed.PID) {
            case PAT:
            case SDT:
                parseTable(payload);
                break;
            default:
                break;
        }
        
        if(parsed.PID == PAT) break;
    }
}

// TODO: dont byteswap
TSHeader Demuxer::parseHeader(std::uint32_t header) {
    TSHeader parsedHeader;
    header = byteswap32(header);
    parsedHeader.sync = (header & TSHeaderBitmask::Sync);
    parsedHeader.TEI  = (header & TSHeaderBitmask::TEI)     >> 23;
    parsedHeader.PUSI = (header & TSHeaderBitmask::PUSI)    >> 22;
    parsedHeader.Prio = (header & TSHeaderBitmask::Prio)    >> 21;
    parsedHeader.PID  = (header & TSHeaderBitmask::PID)     >> 8;
    parsedHeader.TSC  = (header & TSHeaderBitmask::TSC)     >> 6;
    parsedHeader.AFC  = (header & TSHeaderBitmask::AFC)     >> 4;
    parsedHeader.CC   = (header & TSHeaderBitmask::CC);
    return parsedHeader;
}

void Demuxer::parseTable(std::uint8_t* payload) {
    // Parse table header
    uint8_t tableID = payload[1];
    uint16_t tableLength = ((payload[2] << 8) | payload[3]) & 0x0FFF;

    uint16_t tableData[tableLength];
    //std::memcpy(tableData, payload + 8, tableLength * sizeof(uint8_t));
    std::memcpy(tableData, payload + 8, 8 * sizeof(uint8_t));

    switch(tableID) {
        case PAT:
            std::cout << "TES" << std::endl;
            break;
    }

}

void Demuxer::parsePAT(std::uint8_t* payload) {
    uint8_t tableID = payload[1];

    uint16_t tableLength = ((payload[2] << 8) | payload[3]) & 0x0FFF;

    if (tableLength > 0) {
        uint16_t TSID = ((payload[4] << 8) | payload[5]);
        uint16_t programMapID = ((payload[8] << 8) | payload[9]) & 0x1FFF;
        std::cout << TSID << " -> " << programMapID << std::endl;
        this->serviceToPMT[TSID] = programMapID;
    }
    std::cout << tableLength << std::endl;
}


/*
 * DEBUG STUFF
 */

void Demuxer::printHeader(int n) {
    int count = 0;

    while (!file.eof()) {
        // Find sync byte
        while (!file.eof()) {
            if (file.get() == 'G') { 
                file.unget();
                break;
            }
        }

        // Read the header
        std::uint32_t header;
        file.read(reinterpret_cast<char*>(&header), sizeof header);
        TSHeader parsed = parseHeader(header);

        if (count == n) {
            // Print info
            printf("Header: %x\n", header);
            printf("Sync: %x\n",   parsed.sync);
            printf("TEI: %x\n",    parsed.TEI);
            printf("PUSI: %x\n",   parsed.PUSI);
            printf("Prio: %x\n",   parsed.Prio);
            printf("PID: %d\n",    parsed.PID);
            printf("TSC: %x\n",    parsed.TSC);
            printf("AFC: %x\n",    parsed.AFC);
            printf("Cont: %x\n",   parsed.CC);
            break;
        }

        count++;
    }
}

void Demuxer::scanPids() {
    std::unordered_set<uint16_t> pids;

    for (;;) {
        // Find sync byte
        while (!(file.eof())) {
            if (file.get() == 'G') { 
                file.unget();
                break;
            }
        }

        // Read the header
        std::uint32_t header;
        file.read(reinterpret_cast<char*>(&header), sizeof header);
        TSHeader parsed = parseHeader(header);
        pids.insert(parsed.PID);

        if(file.eof())
            break;

        // Skip the payload
        file.seekg(184, std::ios_base::cur);
    }

    printf("All PIDs:\n");
    for (auto pid : pids) {
        printf("%d\n", pid);
    }
}

void Demuxer::debug() {
    std::unordered_set<uint16_t> pids;

    while (!file.eof()) {
        // Find sync byte
        while (!file.eof()) {
            if (file.get() == 'G') { 
                file.unget();
                break;
            }
        }

        // Read the header
        std::uint32_t header;
        file.read(reinterpret_cast<char*>(&header), sizeof header);
        TSHeader parsed = parseHeader(header);
        pids.insert(parsed.PID);

        // Skip the payload
        file.seekg(184, std::ios_base::cur);

        char c;
        file.read(reinterpret_cast<char*>(&c), sizeof c);
        std::cout << "First (" << file.tellg() << "): " << c << std::endl;
        
        int pos1 = file.tellg();

        if (c != 'G') {
            while (true) {
                if (file.get() == 'G') { 
                    file.unget();
                    break;
                }
            }

            file.read(reinterpret_cast<char*>(&c), sizeof c);
            std::cout << "Second (" << file.tellg() << "): " << c << std::endl;
            int pos2 = file.tellg();
            std::cout << "Diff:" << pos2 - pos1 << std::endl;
            std::cout << "--------" << std::endl;
            file.seekg(-1, std::ios_base::cur);
        }
    }

    printf("All PIDs:\n");
    for (auto pid : pids) {
        //printf("%d\n", pid);
    }
}

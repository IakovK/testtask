#ifndef STRUCTS_H
#define STRUCTS_H

#include <cstdint> 

#define MAX_SIZE 1600
#define MIN_SIZE 600
#define SENTINEL_VALUE 0xffffffff
struct header
{
    std::uint32_t seqNum;       // packet number
    std::uint32_t byteCount;    // length of the payload in bytes
    std::uint64_t sendTime;     // time when sent
    std::uint8_t md5Hash[16];   // MD5 checksum
};

struct packet
{
    header h;
    std::uint16_t payload[MAX_SIZE];    // payload
};

#endif // STRUCTS_H

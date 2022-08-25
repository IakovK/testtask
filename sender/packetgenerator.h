#ifndef PACKETGENERATOR_H
#define PACKETGENERATOR_H

#include "../common/structs.h"

/**
 * @todo write docs
 */
class PacketGenerator
{
public:
    // packetNum is number of packet within portion; used to determine packet size and contents
    // seqNum is sequence number to put into the packet header
    void Generate(packet &p, int packetNum, int seqNum);
private:
};

#endif // PACKETGENERATOR_H

#include <string.h>
#include <openssl/md5.h>
#include "packetgenerator.h"

// packetNum is number of packet within portion; used to determine packet size and contents
// packet size will be MIN_SIZE+packetNum 2-byte words
// seqNum is sequence number to put into the packet header
// payload[j] will be packetNum + j;
void PacketGenerator::Generate(packet &p, int packetNum, int seqNum)
{
    memset(&p, 0, sizeof p);
    
    int payloadSize = MIN_SIZE + packetNum;
    
    p.h.seqNum = seqNum;
    p.h.byteCount = 2 * payloadSize;

    for (int j = 0; j < payloadSize; j++)
    {
        p.payload[j] = packetNum + j;
    }
    
    MD5((std::uint8_t *)p.payload, p.h.byteCount, p.h.md5Hash);
}



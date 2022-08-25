#ifndef RECEIVER_H
#define RECEIVER_H

#include <string>
#include <boost/asio.hpp>
#include "packetbuffer.h"
#include "../common/structs.h"

/**
 * @todo write docs
 */
class Receiver
{
    std::string _port;
    PacketBuffer<packet> _pb;
public:
    Receiver(std::string port);
    void run();
private:
    void ReadPackets(boost::asio::ip::tcp::socket &socket);
    void readData();
    void processData();
};

#endif // RECEIVER_H

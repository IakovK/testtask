#ifndef SENDER_H
#define SENDER_H

#include <string>
#include <boost/asio.hpp>
#include "../common/structs.h"

/**
 * @todo write docs
 */
class Sender
{
    std::string _host;
    std::string _port;
    int nextNumber;
public:
    Sender(std::string host, std::string port);
    void run();
private:
    void SendPackets(boost::asio::ip::tcp::socket &s);
    void SendPortion(boost::asio::ip::tcp::socket &s, int PortionSize);
    void SendPacket(packet &p, boost::asio::ip::tcp::socket &s);
};

#endif // SENDER_H

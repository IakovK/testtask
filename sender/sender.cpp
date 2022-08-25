#include <iostream>
#include <chrono>
#include <thread>
#include <boost/endian/conversion.hpp>

#include "sender.h"
#include "packetgenerator.h"
#include "../common/date/include/date/date.h"

uint64_t timeSinceEpochMillisec()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void ConvertToBigEndian(packet &p)
{
}

using boost::asio::ip::tcp;

Sender::Sender (std::string host, std::string port)
    :_host (host)
    ,_port (port)
    ,nextNumber(1)
{
}

void Sender::run()
{
    std::cout << "Sender::run: host = " << _host << ", port = " << _port << std::endl;
    try
    {

        boost::asio::io_service io_service;

        // Connect to the receiver
        tcp::resolver resolver (io_service);
        tcp::resolver::query query (_host, _port);
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::resolver::iterator end;

        tcp::socket socket (io_service);
        boost::system::error_code error = boost::asio::error::host_not_found;
        while (error && endpoint_iterator != end)
        {
            socket.close();
            socket.connect(*endpoint_iterator++, error);
        }
        if (error)
        {
            // connection failure
            throw boost::system::system_error(error);
        }
        else
        {
            // connect successful, send packets
            SendPackets(socket);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void Sender::SendPackets(boost::asio::ip::tcp::socket &s)
{
    using namespace std::chrono_literals;

    int PortionSize = 1000;

    SendPortion(s, PortionSize);
    
    std::this_thread::sleep_for(10s);

    SendPortion(s, PortionSize);
}

void Sender::SendPortion(boost::asio::ip::tcp::socket &s, int PortionSize)
{
    using namespace std::chrono_literals;
    packet p;
    PacketGenerator pg;
    for (int j = 0; j < PortionSize; j++)
    {
        auto currentTime = std::chrono::system_clock::now();
        auto nextTime = currentTime + 10ms;
        pg.Generate(p, j, nextNumber);
        SendPacket(p, s);
        std::this_thread::sleep_until(nextTime);
        nextNumber++;
    }
}

void Sender::SendPacket(packet &p, boost::asio::ip::tcp::socket &s)
{
    int bufferSize = p.h.byteCount;
    int packetNum = p.h.seqNum;
    p.h.sendTime = timeSinceEpochMillisec();

    // convert packet data to big endian (to be architecture independent)
    int wordCount = p.h.byteCount / 2;
    boost::endian::native_to_big_inplace(p.h.seqNum);
    boost::endian::native_to_big_inplace(p.h.byteCount);
    boost::endian::native_to_big_inplace(p.h.sendTime);
    // convert payload to big endian. Disabled for speed
#if 0
    for (int j = 0; j < wordCount; j++)
    {
        boost::endian::native_to_big_inplace(p.payload[j]);
    }
#endif

    boost::system::error_code error;
    boost::asio::write(s, boost::asio::buffer(&p.h, sizeof(header)), error);
    boost::asio::write(s, boost::asio::buffer(&p.payload, bufferSize), error);
    auto t = std::chrono::system_clock::now();
    using namespace date;
    std::cout << "Sent: " << packetNum << ", " << t << std::endl;
}

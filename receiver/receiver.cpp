#include <iostream>
#include <chrono>
#include <boost/endian/conversion.hpp>
#include <string.h>
#include <openssl/md5.h>
#include "receiver.h"
#include "../common/date/include/date/date.h"

using boost::asio::ip::tcp;

bool IsMD5Valid(const packet &p)
{
    std::uint8_t md5Hash[16];   // MD5 checksum
    MD5((std::uint8_t *)p.payload, p.h.byteCount, md5Hash);
    return memcmp(md5Hash, p.h.md5Hash, sizeof md5Hash) == 0;
}

Receiver::Receiver(std::string port)
    :_port(port)
    ,_pb(16)
{
}

void Receiver::readData()
{
    boost::asio::io_service service;
    try
    {
        boost::asio::io_service io_service;

        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), std::stoi(_port)));

        tcp::socket socket(io_service);
        acceptor.accept(socket);
        ReadPackets(socket);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void Receiver::processData()
{
    using namespace std::chrono_literals;
    using namespace date;
    int numPackets = 0;
    while (true)
    {
        packet p;
        _pb.pop_back(&p);
        if (p.h.byteCount == SENTINEL_VALUE)
            break;
        auto currentTime = std::chrono::system_clock::now();
        auto nextTime = currentTime + 15ms;
        bool valid = IsMD5Valid(p);
        std::this_thread::sleep_until(nextTime);
        currentTime = std::chrono::system_clock::now();
        std::cout << "Processed: " << p.h.seqNum << ", " << currentTime << ", " << (valid?"PASS":"FAIL") << std::endl;
        numPackets++;
    }
    std::cout << "Total processed packets: " << numPackets << std::endl;
}

void Receiver::run()
{
    std::cout << "Receiver::run: port = " << _port << std::endl;
    std::thread reader(&Receiver::readData, this);
    std::thread processor(&Receiver::processData, this);
    reader.join();
    processor.join();
    std::cout << "Receiver::run: exit" << std::endl;
}

void Receiver::ReadPackets(boost::asio::ip::tcp::socket &socket)
{
    try
    {
        for (;;)
        {
            packet p;
            boost::system::error_code error;

            size_t len = boost::asio::read(socket, boost::asio::buffer(&p.h, sizeof(header)), error);
            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.

            // convert packet data from big endian to native format
            boost::endian::big_to_native_inplace(p.h.seqNum);
            boost::endian::big_to_native_inplace(p.h.byteCount);
            boost::endian::big_to_native_inplace(p.h.sendTime);

            len = boost::asio::read(socket, boost::asio::buffer(&p.payload, p.h.byteCount), error);
            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.

            // convert payload from big endian to native format. Disabled for speed
#if 0
            int wordCount = p.h.byteCount / 2;
            for (int j = 0; j < wordCount; j++)
            {
                boost::endian::native_to_big_inplace(p.payload[j]);
            }
#endif
            auto rcvTime = std::chrono::system_clock::now();
            using namespace date;

            bool valid = IsMD5Valid(p);
            _pb.push_front(p);
            std::cout << "Received: " << p.h.seqNum << ", " << rcvTime << ", " << (valid?"PASS":"FAIL") << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    // push sentinel value to have processing thread cleanly terminate
    packet p;
    memset(&p, 0, sizeof p);
    p.h.byteCount = SENTINEL_VALUE;
    _pb.push_front(p);
}



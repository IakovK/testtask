#include <iostream>
#include "sender.h"

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cout << "Usage: Sender <host> <port>" << std::endl;
        return 1;
    }
    std::string host = argv[1];
    std::string port = argv[2];
    Sender sender(host, port);
    sender.run();
    std::cout << "Completed" << std::endl;
    return 0;
}

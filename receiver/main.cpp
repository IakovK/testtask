#include <iostream>
#include "receiver.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: Receiver <port>" << std::endl;
        return 1;
    }
    std::string port = argv[1];
    Receiver receiver(port);
    receiver.run();
    return 0;
}

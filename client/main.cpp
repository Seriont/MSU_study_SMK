#include <iostream>
#include "client.h"

int main(void)
{
    Client client;
    if (client.start())
    {
        std::cout << "Connect Succesful" << std::endl;
    }
    else
    {
        std::cout << "Didn't connect" << std::endl;
    }
    if (!client.process())
    {
        std::cout << "connect was broken" << std::endl;
    }
    return 0;
}


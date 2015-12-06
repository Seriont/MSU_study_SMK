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
        // generate twrow
        std::cout << "Didn't connect" << std::endl;
        return 1;
    }
    if (!client.process())
    {
        std::cout << "Connect was broken" << std::endl;
        return 2;
    }
    return 0;
}


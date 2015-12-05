#include <iostream>
#include "client.h"

int main(void)
{
    Client client;
    if(client.start())
    {
        std::cout << "Connect Succesful" << std::endl;
    }
    else
    {
        std::cout << "Didn't connect" << std::endl;
    }
    client.process();
    return 0;
}


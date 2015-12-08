#include <iostream>
#include "client.h"
#include "../errorexept.h"


int main(void)
{
    Client client;
    try
    {
        client.start();
        std::cout << "Connect Succesful" << std::endl;
        client.process();
    }
    catch (const ErrorExept& exeption)
    {
        exeption.printError();
    }
    return 0;
}


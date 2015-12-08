#include "server.h"
#include "../errorexept.h"
#include <iostream>


int main(void)
{
	Server server;
	try
	{
		if (server.startServer())
		{
			std::cout << "Server's started successfully" << std::endl;
		}

		server.process();
	}
	catch(const ErrorExept& exeption)
	{
		std::cout << exeption.getErrorMessage() << std::endl;
		std::cout << strerror(exeption.getErrorCode()) << std::endl;
		return 1;
	}
	catch(...)
	{
		std::cout << "error: unknown error" << std::endl;
		return 2;
	}
	return 0;
}

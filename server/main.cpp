#include "server.h"
#include "../errorexept.h"
#include <iostream>


int main(void)
{
	Server server;
	try
	{
		server.startServer();
		std::cout << "Server's started successfully" << std::endl;
		server.process();
	}
	catch(const ErrorExept& exeption)
	{
		exeption.printError();
	}
	catch(...)
	{
		std::cout << "error: unknown error" << std::endl;
	}
	return 0;
}

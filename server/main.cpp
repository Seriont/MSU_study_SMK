#include "server.h"
#include <iostream>


int main(void)
{
	Server server;
	if (server.startServer())
	{
		std::cout << "Server's started successfully" << std::endl;
	}
	
	if (!server.process())
	{
		std::cout << "Server's fallen" << std::endl;
		server.sendServerMessage((char *)"Server's fallen");
	}

	return 0;
}

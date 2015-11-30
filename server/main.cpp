#include "server.h"
#include <iostream>


int main(void)
{
	Server server;
	server.startServer();
	if (!server.process())
	{
		cout << "Server felt" << std::endl;
		server.sendServerMessage("Server felt");
	}

	return 0;
}

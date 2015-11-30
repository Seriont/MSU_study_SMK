#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <vector>
#include <set>
#include <cstring>
#include <string>
#include <unistd.h>
#include <iostream>
#include "../constants.h"
#include <cstdio>


class Client
{
public:
	bool startClient();
	void sendMessage(char message[]);
	Client();
	~Client();
private:
	int socket;
};



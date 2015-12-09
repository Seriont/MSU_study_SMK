#include <sys/types.h>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include "../constants.h"
#include "../errorexept.h"
#include <iostream>
#include <arpa/inet.h>


class Client
{
public:
    Client() {}
    bool start();
    bool process();
private:
    int connect_socket;
    bool getInputMessage(char message[]);
    bool getMessage(char message[]);
    bool sendMessage(char message[]);
};


bool Client::getInputMessage(char message[])
{
    int i = 0, character;
    while((character = getchar()) != '\n' && i < MAX_MESSAGE_LEN)
    {
        message[i++] = character;
    }
    message[i] = '\0';
    return true;
}


bool Client::start()
{
    connect_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (connect_socket < 0)
    {
        throw ErrorExept("error:couldn't call socket");
    }
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    if (!inet_aton(IP_ADDRESS, &(addr.sin_addr)))
	{
		throw ErrorExept("error: invalid IP");
	}
    if (0 != connect(connect_socket, (struct sockaddr *)&addr, sizeof(addr)))
    {
        close(connect_socket);
        throw ErrorExept("error: couldn't connect");
    }
    return true;
}


bool Client::process()
{
    fd_set read_fds;
    int max_ds = connect_socket;
    char *message = new char [MAX_MESSAGE_LEN+1];
    for(;;)
    {
        FD_ZERO(&read_fds);
        FD_SET(connect_socket, &read_fds);
        FD_SET(0, &read_fds);
        
        if (select(max_ds+1, &read_fds, NULL, NULL, NULL) < 0)
        {
            throw ErrorExept("error: connect has broken");
        }
        if (FD_ISSET(connect_socket, &read_fds))
        {
            if (!getMessage(message))
            {
                std::cout << "Server's stopped, I'm going to close" << std::endl;
                return true;
            }
            else
            {
                std::cout << message << std::endl;
            }
        }
        if (FD_ISSET(0, &read_fds))
        {
            try
            {
                getInputMessage(message);
                sendMessage(message);
            }
            catch (const ErrorExept& exeption)
            {
                exeption.printError();
            }
        }
    }
    delete[] message;
}


bool Client::sendMessage(char *message)
{
    if(write(connect_socket, message, strlen(message)) < 0)
    {
        delete [] message;
        throw ErrorExept("error: couldn't call write");
    }
    return true;
}


bool Client::getMessage(char message[])
{
    int len;
    if ((len = read(connect_socket, message, MAX_MESSAGE_LEN)) < 0)
    {
        delete[] message;
        throw ErrorExept("error: couldn't call read");
    }
    if (len == 0)
    {
        return false;
    }
    message[len] = '\0';
    return true;
}

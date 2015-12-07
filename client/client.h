#include <sys/types.h>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include "../constants.h"
#include <iostream>


class Client
{
public:
    Client() {}
    bool start();
    bool process();
private:
    int connect_socket;
    char* getInputMessage();
    bool getMessage();
    bool sendMessage(char s[]);
};


char* Client::getInputMessage()
{
    char *message = new char [MAX_MESSAGE_LEN+1];
    int i = 0, character;
    while((character = getchar()) != '\n' && i < MAX_MESSAGE_LEN)
    {
        message[i++] = character;
    }
    message[i] = '\0';
    return message;
}


bool Client::start()
{
    connect_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (connect_socket < 0)
    {
        // generate throw
        return false;
    }
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (0 != connect(connect_socket, (struct sockaddr *)&addr, sizeof(addr)))
    {
        // generate throw
        close(connect_socket);
        return false;
    }
    return true;
}


bool Client::process()
{
    fd_set read_fds;
    int max_ds = connect_socket;
    for(;;)
    {
        FD_ZERO(&read_fds);
        FD_SET(connect_socket, &read_fds);
        FD_SET(0, &read_fds);
        
		if (select(max_ds+1, &read_fds, NULL, NULL, NULL) < 0)
        {
            // generate throw
            return false;
        }
        if (FD_ISSET(connect_socket, &read_fds))
        {
            // get message from server
            if (!getMessage())
            {
                std::cout << "Server's stopped, I'm going to close" << std::endl;
                return true;
            }
        }
        if (FD_ISSET(0, &read_fds))
        {
            char *message = getInputMessage();
            sendMessage(message);
        }
    }
}


bool Client::sendMessage(char *message)
{
    if(write(connect_socket, message, strlen(message)) < 0)
    {
        // generate throw
        return false;
    }
    delete [] message;
    return true;
}


bool Client::getMessage()
{
    char *message = new char [MAX_MESSAGE_LEN+1];
    int len;
    if ((len = read(connect_socket, message, MAX_MESSAGE_LEN)) <= 0)
    {
        // generate throw
        return false;
    }
    // if (len == 0)
    // {
        // std::cout << "Server stopped work" << std::endl;
        // return true;
    // }
    message[len] = '\0';
    std::cout << message << std::endl;
    delete [] message;
    return true;
}

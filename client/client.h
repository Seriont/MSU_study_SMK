#include <sys/types.h>
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include "../constants.h"


class Client
{
public:
    Client() {}
    bool start();
    bool process();
private:
    int connect_socket;
    char* getMessage();
    bool sendMessage(char s[]);
};


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
        }
        if (FD_ISSET(0, &read_fds))
        {
            // send message to server
        }
    }
}
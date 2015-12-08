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
#include "../errorexept.h"
#include <cstdio>

//we should describe the User class, whether it should
// be global or not, and what demands should it meet

// all arguments, that are marked as 'void' will be replaced
// by the person, who will code that function


class User 
{
public:
	int socket;
	User() {}
};


class Server 
{
public:
	bool startServer();
	bool process();
	void sendServerMessage(char message[]);
	Server() {}
private:
	char* getInputMessage();
	bool getMessage(char message[], User *user);
	void sendToAllMessage(char message[]);
	bool addNewClient();
	// bool isNameValid(void); // checking for forbidden characters
	// bool isNameUsed(void); // checking for equal usernames
	std::set<User *> clients;
	int listen_socket;
};


char* Server::getInputMessage()
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


bool Server::startServer() 
{
	listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket < 0)
	{
		throw ErrorExept("error: couldn't call socket");
	}
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT); // PORT is a constant
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(listen_socket, (struct sockaddr *) &addr, sizeof(addr)) != 0)
	{
		close(listen_socket);
		throw ErrorExept("error: couldn't call bind");
	}
	if (listen(listen_socket, MAX_REQUESTS))
	{
		close(listen_socket);
		throw ErrorExept("error: couldn't call listen");
	}
	return true;
}


bool Server::addNewClient()
{
	int socket = accept(listen_socket, NULL, NULL);
	if (socket < 0)
	{
		throw ErrorExept("error: couldn't connect user");
	}
	User *new_user = new User();
	new_user->socket = socket;
	clients.insert(new_user);
	return true;
}


bool Server::getMessage(char message[], User *user)
{
	size_t bytes_read = read(user->socket, message, MAX_MESSAGE_LEN);
	if (bytes_read < 0)
	{
		throw ErrorExept("error: couldn't get message");
	}
	message[bytes_read/sizeof(char)] = '\0';
	return true;
}


void Server::sendToAllMessage(char message[])
{
	for (std::set<User *>::iterator it = clients.begin();
		it != clients.end(); it++)
	{
		if (write((*it)->socket, message, strlen(message) + 1) < 0)
		{
			std::cout << "Didn't manage to send the message to user[";
			std::cout << (*it)->socket << "]!" << std::endl;
		}
	}
}


void Server::sendServerMessage(char message[])
{
	sendToAllMessage(message);
}


bool Server::process()
{
	fd_set read_fds;
	while (true)
	{
		// read_fds initialization
		FD_ZERO(&read_fds);
		int max_number = listen_socket;
		FD_SET(listen_socket, &read_fds);
		FD_SET(0, &read_fds);
		for (std::set<User *>::iterator it = clients.begin(); 
			it != clients.end(); it++)
		{
			FD_SET((*it)->socket, &read_fds);
			if ((*it)->socket > max_number)
				max_number = (*it)->socket;
		}
		// query handling

		// waiting for queries

		// the last pointer is a timer
		if (select(max_number + 1, &read_fds, NULL, NULL, NULL) < 1)
		{
			sendServerMessage((char *)"server's fallen");
			throw ErrorExept("error: server's fallen");
		}

		// new client adding
		if (FD_ISSET(listen_socket, &read_fds))
		{
			try
			{
				addNewClient();
			}
			catch(const ErrorExept& exeption)
			{
				std::cout << exeption.getErrorMessage() << std::endl;
				std::cout << strerror(exeption.getErrorCode()) << std::endl;
			}
		}
		if (FD_ISSET(0, &read_fds))
		{
			char *message = getInputMessage();
            sendToAllMessage(message);
		}

		// message reading and deleting closed sockets

		// <current_clients> is a list of clients, who were in session
		// in the beginning of the current iteration of the cycle
		User **current_clients = new User*[(const int)clients.size()];
		int i = 0;
		for (std::set<User *>::iterator it = clients.begin();
			it != clients.end(); it++, i++)
		{
			current_clients[i] = *it;
		}
		for (int i = 0; i < (int)clients.size(); i++)
		{
			if (FD_ISSET(current_clients[i]->socket, &read_fds))
			{
                char *message = new char [MAX_MESSAGE_LEN+1];
                try
                {
					getMessage(message, current_clients[i]);
                }
                catch (const ErrorExept& exeption)
				{
					std::cout << exeption.getErrorMessage() << std::endl;
					std::cout << strerror(exeption.getErrorCode()) << std::endl;					
				}
				if (strlen(message) == 0)
				{
					// in this case user's quitened and his socket
					// should be deleted
					close(current_clients[i]->socket);
					clients.erase(current_clients[i]);
					continue;
				}
				sendToAllMessage(message);
                delete [] message;
			}
		}
		delete [] current_clients;
	}
}

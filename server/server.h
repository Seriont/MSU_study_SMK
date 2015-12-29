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
#include "../functions.h"
#include <cstdio>


class User 
{
	friend class Server;
private:
	int socket;

public:
	char *username;
	bool is_online;
	Status status;
	User();
	~User();
};


class Server 
{
public:
	void startServer();
	void process();
	void sendServerMessage(char message[]);
	Server() {}

private:
	void getInputMessage(char message[]);
	void getMessage(char message[], User *user, DataType type);
	void sendToAllMessage(char message[]);
	void addNewClient();
	void makeMessage(char message[], char username[], char text[]);
	// bool isNameValid(void); // checking for forbidden characters
	// bool isNameUsed(void); // checking for equal usernames
	std::set<User *> clients;
	int listen_socket;
};


User::User()
{
	is_online = true;
	status = USERNAME_REQUIRED;
}


User::~User()
{
	delete[] username;
}


void Server::getInputMessage(char message[])
{
    int i = 0, character;
    while((character = getchar()) != '\n' && i < MAX_MESSAGE_LEN)
    {
        message[i++] = character;
    }
    message[i] = '\0';
}


void Server::startServer() 
{
	listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket < 0)
	{
		throw ErrorExept("error: couldn't call socket");
	}
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(listen_socket, (struct sockaddr *) &addr, sizeof(addr)) != 0)
	{
		close(listen_socket);
		throw ErrorExept("error: couldn't call bind");
	}
	if (listen(listen_socket, MAX_REQUESTS) != 0)
	{
		close(listen_socket);
		throw ErrorExept("error: couldn't call listen");
	}
}


void Server::addNewClient()
{
	// here we can save the IP adress of the client
	int socket = accept(listen_socket, NULL, NULL);
	if (socket < 0)
	{
		throw ErrorExept("error: couldn't connect user");
	}
	User *new_user = new User();
	new_user->socket = socket;
	clients.insert(new_user);
}


void Server::getMessage(char message[], User *user, DataType type)
{
	size_t bytes_read;
	if (type == MESSAGE)
		bytes_read = read(user->socket, message, MAX_MESSAGE_LEN);
	else if (type == USERNAME)
		bytes_read = read(user->socket, message, USERNAME_MAX_LEN);
	if (bytes_read < 0)
	{
		throw ErrorExept("error: couldn't get message");
	}
	message[bytes_read/sizeof(char)] = '\0';
	normalizeString(message);
}


void Server::sendToAllMessage(char message[])
{
	for (std::set<User *>::iterator it = clients.begin();
		it != clients.end(); it++)
	{
		if ((*it)->is_online == false)
			continue;
		if (write((*it)->socket, message, strlen(message) + 1) < 0)
		{
			throw ErrorExept("error: couldn't send message");
		}
	}
}


void Server::sendServerMessage(char message[])
{
	sendToAllMessage(message);
}


void Server::makeMessage(char message[], char username[], char text[])
{
	int i = 0;
	while (username[i] != '\0')
	{
		message[i] = username[i];
		i++;
	}
	message[i++] = ':';
	message[i++] = ' ';
	int j = 0;
	while (text[j] != '\0')
	{
		message[i] = text[j];
		i++;
		j++;
	}
	message[i] = '\0';
}


void Server::process()
{
	fd_set read_fds;
	while (true)
	{
		// read_fds initialization
		FD_ZERO(&read_fds);
		int max_ds = listen_socket;
		FD_SET(listen_socket, &read_fds);
		FD_SET(0, &read_fds);
		for (std::set<User *>::iterator it = clients.begin(); 
			it != clients.end(); it++)
		{
			FD_SET((*it)->socket, &read_fds);
			if ((*it)->socket > max_ds)
				max_ds = (*it)->socket;
		}
		// query handling

		// waiting for queries

		// the last pointer is a timer
		if (select(max_ds + 1, &read_fds, NULL, NULL, NULL) < 1)
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
				std::cout << "New connection." << std::endl;
			}
			catch(const ErrorExept& exeption)
			{
				exeption.printError();
			}
		}
		if (FD_ISSET(0, &read_fds))
		{
			char *message = new char[MAX_MESSAGE_LEN+1];
			getInputMessage(message);
            sendToAllMessage(message);
			delete[] message;
		}

		// message reading and deleting closed sockets
		for (std::set<User *>::iterator it = clients.begin();
			it != clients.end(); it++)
		{
			User *client = *it;
			if (FD_ISSET(client->socket, &read_fds))
			{
				if (client->is_online == false)
					continue;
                try
                {
					if (client->status == USERNAME_REQUIRED)
					{
						char *username = new char[USERNAME_MAX_LEN+1];
						getMessage(username, client, USERNAME);
						if (strlen(username) == 0)
							client->is_online = false;
						else
						{
							client->username = username;
							client->status = AUTHORIZED;
						}
					}
					else if (client->status == AUTHORIZED)
					{
						char *message = new char[MAX_MESSAGE_LEN];
						char *text = new char[TEXT_MAX_LEN];
						getMessage(text, client, MESSAGE);
						if (strlen(text) == 0)
							client->is_online = false;
						else
						{
							makeMessage(message, client->username, text);
							delete[] text;
							sendToAllMessage(message);
						}
						delete[] message;
                	}
				}
                catch (const ErrorExept& exeption)
				{
					exeption.printError();			
				}
			}
		}
		std::set<User *>::iterator it = clients.begin();
		while(it != clients.end())
		{
			User *client = *it;
			if (client->is_online == true)
			{
				it++;
				continue;
			}
			close(client->socket);
			clients.erase(client);
			delete client;
			it = clients.begin();
		}
	}
}

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <vector>
#include <set>
#include <cstring>
#include <string>
#include <unistd.h>
#include "../constants.h"

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
	bool getMessage(std::string &message, User *user);
	void sendToAllMessage(std::string &message, User *user);
	void sendServerMessage(void);
	bool addNewClient();
	bool isNameValid(void); // checking for forbidden characters
	bool isNameUsed(void); // checking for equal usernames
	Server() {}
private:
	std::set<User *> clients;
	int listening_socket;
};


bool Server::startServer() 
{
	this->listening_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listening_socket < 0)
		return false;
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT); // PORT is a constant
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(this->listening_socket, (struct sockaddr *) &addr,
		 sizeof(addr)) != 0)
	{
		return false;
	}
	listen(this->listening_socket, 5);
	return true;
}


bool Server::process()
{
	fd_set read_fds;
	while (true)
	{
		// read_fds initialization
		FD_ZERO(&read_fds);
		int max_number = this->listening_socket;
		FD_SET(this->listening_socket, &read_fds);
		for (std::set<User *>::iterator it = this->clients.begin(); 
			it != this->clients.end(); it++)
		{
			FD_SET((*it)->socket, &read_fds);
			if ((*it)->socket > max_number)
				max_number = (*it)->socket;
		}
		// query handling
		int result = select(max_number + 1, &read_fds, NULL, NULL, NULL);
		if (result < 1)
			return false;
		// new client adding
		if (FD_ISSET(this->listening_socket, &read_fds))
		{
			if (!addNewClient())
				return false;
		}
		// message reading and deleting closed sockets
		std::vector<User *> clear_stack;
		for (std::set<User *>::iterator it = this->clients.begin();
			it != this->clients.end(); it++)
		{
			if (FD_ISSET((*it)->socket, &read_fds)) {
				std::string message;
				if (!getMessage(message, *it)) {
					close((*it)->socket);
					clear_stack.push_back(*it);
					continue;
				}
				sendToAllMessage(message, *it);
			}
		}
		// clients' set refreshment
		int length = (int) clear_stack.size();
		while (length > 0)
		{
			length--;
			this->clients.erase(clear_stack[length]);
			clear_stack.pop_back();
		}
	}
}

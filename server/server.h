#include <sys/socket.h>
#include <sys/types.h>
#include <vector>
#include <set>
#include <string>
#include <unistd.h>

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
	bool getMessage(string &message, User *user);
	void sendToAllMessage(string &message, User *user);
	void sendServerMessage(void);
	void addNewClient();
	bool isNameValid(void); // checking for forbidden characters
	bool isNameUsed(void); // checking for equal usernames
	Server() {}
private:
	set<User *> clients;
	int listening_socket;
};


bool Server::startServer() 
{
	this->listening_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listening_socket < 0)
		return false;
	sockaddr addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT); // PORT is a constant
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(this->listening_socket, &addr, sizeof(addr)) != 0)
		return false;
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
		for (set<User *>::iterator it = this->clients.begin(); 
			it != this->clients.end(); it++)
		{
			FD_SET(it->socket, &read_fds);
			if (it->socket > max_number)
				max_number = it->socket;
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
		vector<User *> clear_stack;
		for (set<User *>::iterator it = this->clients.begin();
			it != this->clients.end(); it++)
		{
			if (FD_ISSET(*it, &read_fds)) {
				string message;
				if (!getMessage(message, it)) {
					close(it->socket);
					clear_stack.push_back(it);
					continue;
				}
				sendToAllMessage(message, it);
			}
		}
		// clients' set refreshment
		int length = (int) clear_stack.size();
		while (length > 0)
		{
			User *element = pop_back(clear_stack);
			this->clients.erase(element);
			length--;
		}
	}
}

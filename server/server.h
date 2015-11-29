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
	bool startServer(void);
	bool stopServer(void);
	void sendToAllMessage(void);
	void sendServerMessage(void);
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


bool Server::()
{
	fd_set read_fds;
	while (true)
	{
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
		int result = select(max_number + 1, &read_fds, NULL, NULL, NULL);
		if (result < 1)
			return false;
		if (FD_ISSET(this->listening_socket, &read_fds))
		{
			int new_socket = accept(this->listening_socket, NULL, NULL);
			User *new_user = new User();
			new_user->socket = new_socket;
			this->clients.insert(new_user);
		}
	}
}

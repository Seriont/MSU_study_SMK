#include <sys/socket.h>
#include <sys/types.h>
#include <vector>
#include <string>

//we should describe the User class, whether it should
// be global or not, and what demands should it meet

// all arguments, that are marked as 'void' will be replaced
// by the person, who will code that function

class User {
public:
	string username;
};

class Server {
public:
	bool startServer(void);
	void sendToAllMessage(void);
	void sendServerMessage(void);
	bool isNameValid(void);
	bool isNameUsed(void);
private:
	vector<User *> clients;
};

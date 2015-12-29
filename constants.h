const int PORT = 10005;
const int MAX_REQUESTS = 5;
const int MAX_MESSAGE_LEN = 512;
const int USERNAME_MAX_LEN = 16;
const int TEXT_MAX_LEN = 496;
const char *IP_ADDRESS = "192.168.0.1";


enum Status
{
	AUTHORIZED,
	USERNAME_REQUIRED
};


enum DataType
{
	MESSAGE,
	USERNAME
};

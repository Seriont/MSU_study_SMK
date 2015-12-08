#ifndef ERROREXEPT
#define ERROREXEPT

#include <iostream>
#include <cstring>
#include <cerrno>


class ErrorExept
{
public:
	ErrorExept(const char error_message[]);
	ErrorExept(const ErrorExept& other); // copy constructor
	~ErrorExept();
	const char* getErrorMessage() const;
	int getErrorCode() const;
	void printError() const;
private:
	char *err_message;
	int err_code;
	static char *strdup(const char *string); // ~strcpy, but use "new"
};


void ErrorExept::printError() const
{
	std::cout << getErrorMessage() << std::endl;
    std::cout << strerror(getErrorCode()) << std::endl;
}


ErrorExept::ErrorExept(const char error_message[])
{
	err_message = strdup(error_message);
	err_code = errno;
}


ErrorExept::ErrorExept(const ErrorExept& other)
{
	err_message = strdup(other.err_message);
	err_code = other.err_code;
}


ErrorExept::~ErrorExept()
{
	delete[] err_message;
}


const char* ErrorExept::getErrorMessage() const
{
	return err_message;
}


int ErrorExept::getErrorCode() const
{
	return err_code;
}


char* ErrorExept::strdup(const char *string)
{
	char *new_string = new char [strlen(string)+1];
	strcpy(new_string, string);
	return new_string;
}


#endif

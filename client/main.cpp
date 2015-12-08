#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../errorexept.h"

int main(void)
{
    bool cmd = false;
    char command;
    std::cout << "What are you going to do?" << std::endl;
    while(!cmd)
    {
        std::cout << "C: connect to chat" << std::endl;
        std::cout << "Q: exit;" << std::endl;
        std::cin >> command;
        if (command == 'C' || command == 'c')
        {
            cmd = true;
            int pid;
            try
            {
                if ((pid = fork()) < 0)
                {
                    throw ErrorExept("error: couldn't create new process");
                }
            }
            catch (const ErrorExept& exeption)
            {
                exeption.printError();
            }
            if (pid == 0)
            {
                execlp("./client", "./client", NULL);
            }
            else
            {
                wait(&pid);
            }
        }
        else if (command == 'Q' || command == 'q')
        {
            cmd = true;
            std::cout << "Goodbye" << std::endl;
        }
        else
        {
            std::cout << "Broken command: try again" << std::endl;
        }
    }
    return 0;
}

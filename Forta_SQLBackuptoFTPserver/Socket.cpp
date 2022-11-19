#include "Socket.h"

#include <ws2tcpip.h>

#include <array>

#include <iostream>

Socket::Socket()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw SocketExecution("WSAStartup failed!");
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        WSACleanup();
        throw SocketExecution("Could not find a usable version of Winsock.dll!");
    }
}

Socket::~Socket()
{
    if (_socket != INVALID_SOCKET) {
        close();
    }
    WSACleanup();
}

bool Socket::connectToHost(const std::string& ip, uint16_t port) noexcept
{
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    int iResult;

    // Initialize Winsoc
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hints, &result);
    if (iResult == 0) {
        // Attempt to connect to an address until one succeeds
        for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
            // Create a SOCKET for connecting to server
            ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                ptr->ai_protocol);
            if (ConnectSocket == INVALID_SOCKET) {
                continue;
            }
            // Connect to server.
            iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
            if (iResult == SOCKET_ERROR) {
                closesocket(ConnectSocket);
                ConnectSocket = INVALID_SOCKET;
                continue;
            }
            break;
        }
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        return false;
    }
    _socket = ConnectSocket;
    return true;
}

int Socket::write(const std::string& data) noexcept
{
    return send(_socket, data.c_str(), (int)data.length(), 0);
}

int Socket::write(char* t, int n) noexcept
{
    return send(_socket, t, n, 0);
}

std::string Socket::read()
{
    std::array<char, 100> buff;
    std::string result;
    int recvSize = 0;
    do {
        buff.fill(0);
        recvSize = recv(_socket, buff.data(), (int)buff.size(), 0);
        result += std::string(std::begin(buff), std::end(buff));
        if (recvSize < 0){
            throw SocketExecution("Socket read error!");
        }
    } while (recvSize == buff.size());
    return result;
}

void Socket::close() noexcept
{
    closesocket(_socket);
    _socket = INVALID_SOCKET;
}

int Socket::error() const noexcept
{
    return WSAGetLastError();
}
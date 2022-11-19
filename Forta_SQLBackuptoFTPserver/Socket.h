#pragma once
#include "AbstractSocket.h"

#include <winsock2.h>
#include <execution>

class SocketExecution : public std::exception
{
public:
	SocketExecution(const char* msg) 
		: message(msg) 
	{}
	
	const char* what() const noexcept{
		return message;
	}
private:
	const char* message;
};

class Socket final: public AbstractSocket
{
public:
	Socket();
	~Socket();

	int error() const noexcept final;
	bool connectToHost(const std::string& ip, uint16_t port) noexcept final;
	int write(const std::string& data) noexcept final;
	int write(char* t, int n) noexcept;
	std::string read()final;
	void close() noexcept final;

	
private:
	SOCKET _socket = INVALID_SOCKET;
};


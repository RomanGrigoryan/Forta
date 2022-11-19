#include "FakeSocket.h"

#include <iostream>
#include <iomanip>

int FakeSocket::error() const noexcept
{
	return 0;
}

bool FakeSocket::connectToHost(const std::string& ip, uint16_t port) noexcept
{
	return true;
}

int FakeSocket::write(const std::string& data) noexcept
{
	_lastWrite = data;
	return (int)data.length();
}

int FakeSocket::write(char* t, int n) noexcept
{
	return write(std::string(t));
}

std::string FakeSocket::read()
{
	return _comunication[_lastWrite];
}

void FakeSocket::close() noexcept
{
}
#pragma once

#include <string>

class AbstractSocket
{
public:
	AbstractSocket() {}
	
	virtual ~AbstractSocket() {}
	AbstractSocket(const AbstractSocket&) = delete;
	AbstractSocket& operator=(const AbstractSocket&) = delete;
	AbstractSocket(AbstractSocket&&) = delete;
	AbstractSocket& operator=(AbstractSocket&&) = delete;

public:
	virtual int error() const noexcept = 0;
	virtual bool connectToHost(const std::string& ip, uint16_t port) noexcept = 0;
	virtual int write(const std::string& data) noexcept = 0;
	virtual int write(char* t, int n) noexcept = 0;
	virtual std::string read() = 0;
	virtual void close() noexcept = 0;

protected:
	

private:
};

#pragma once
#include "../Forta_SQLBackuptoFTPserver/AbstractSocket.h"

#include <unordered_map>

class FakeSocket final : public AbstractSocket
{
public:
	int error() const noexcept final;
	bool connectToHost(const std::string& ip, uint16_t port) noexcept final;
	int write(const std::string& data) noexcept final;
	int write(char* t, int n) noexcept final;
	std::string read()final;
	void close() noexcept final;

private:
	std::unordered_map<std::string, std::string> _comunication = {
				{ "", "220-FileZilla Server 1.5.1\r\n220 Please visit https://filezilla-project.org/\r\n"},
				{ "USER test\r\n", "331 Please, specify the password.\r\n"},
				{ "USER error\r\n", "503 Invalid user.\r\n"},
				{ "PASS root\r\n", "230 Login successful.\r\n"},
				{ "TYPE I\r\n", "200 Type set to I\r\n"},
				{ "PASV\r\n", "227 Entering Passive Mode (127,0,0,1,200,154)\r\n"},
	};
	std::string _lastWrite;
};


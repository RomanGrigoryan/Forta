#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include "AbstractSocket.h"

#include <exception>
#include <memory>

using FTPResponse = std::vector<std::pair<int, std::string>>;


class FTPException final : public std::exception
{
public:
	FTPException(const FTPResponse& response, const char* msg)
		: _response(response)
		, _msg(msg)
	{}
	const char* what() const noexcept final
	{
		return _msg;
	}
	const FTPResponse& getResponse() const
	{
		return _response;
	}
private:
	const char* _msg;
	FTPResponse _response;
};

template<class TS>
class FTPClient
{
	static_assert(std::is_base_of<AbstractSocket, TS>::value, "TS SHULD BE SUBCLASS");
	static const int Default_Buffer = 5000;

public:

	void connctToHost(const std::string ip, uint16_t port = 21)
	{
		if (_socket.connectToHost(ip, port)) {
			auto response = readCommandResponse();
			checkErrorCode(response, "Failed to connect to host!");
		}
	}

	void login(const std::string& user, const std::string& password)
	{
		std::string cmd = "USER " + user + "\015\012";
		auto response = sendCommand(cmd);
		checkErrorCode(response, "Failed to login: Login incorrect!");
		cmd = "PASS " + password + "\015\012";
		response = sendCommand(cmd);
		checkErrorCode(response, "Failed to login: Login incorrect!");
	}

	void put(const std::string& sourceFile, const std::string& destFile)
	{
		std::string cmd = "TYPE I" + std::string("\015\012");
		auto response = sendCommand(cmd);
		checkErrorCode(response, "Failed to transfer file!");
		std::ifstream file;
		file.open(sourceFile, std::ios::binary);
		if (file.is_open()) {
			cmd = "PASV" + std::string("\015\012");
			response = sendCommand(cmd);
			checkErrorCode(response, "Failed to transfer file!");
			if (!response.empty()) {
				long int a1, a2, a3, a4, p1, p2;
				sscanf_s(response[0].second.c_str(), " Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &a1, &a2, &a3, &a4, &p1, &p2);
				uint16_t dataPort = (uint16_t)((p1 * 256) + p2);
				std::string ip = std::to_string(a1) + '.' + std::to_string(a2) + '.' + std::to_string(a3) + '.' + std::to_string(a4);
				TS dataSocket;
				if (dataSocket.connectToHost(ip, dataPort)) {
					cmd = "STOR " + destFile + "\015\012";
					response = sendCommand(cmd);
					checkErrorCode(response, "Failed to transfer file!");
					{
						std::unique_ptr<char[]> fileBuff = std::make_unique<char[]>(Default_Buffer);
						if (fileBuff) {
							while (!file.eof()) {
								memset(fileBuff.get(), 0, Default_Buffer);
								file.read(fileBuff.get(), Default_Buffer - 1);

								int iResult = dataSocket.write(std::string(fileBuff.get()));
								if (iResult < 0) {
									break;
								}
							}
						}
					}
				}
			}
			response = readCommandResponse();
			checkErrorCode(response, "Failed to	transfer file!");
		}
		else {
			throw FTPException({}, "Failed to open input file!");
		}
	}

private:
	void checkErrorCode(const FTPResponse& response, const char* message)
	{
		if (response.empty() || (response.back().first /100) > 3) {
			throw FTPException(response, message);
		}
	}
	
	FTPResponse sendCommand(const std::string& cmd)
	{
		if (_socket.write(cmd) > 0) {
			return readCommandResponse();
		}
		return std::vector<std::pair<int, std::string>>();
	}

	FTPResponse readCommandResponse()
	{
		FTPResponse result;
		std::string data = _socket.read();
		if (data.size() > 3 && data[3] != '-') {
			result.push_back(std::make_pair(std::stoi(data), data.substr(3)));
			return result;
		}

		while (true) {
			std::stringstream sstream(data);
			std::string line;
			while (std::getline(sstream, line)) {
				std::size_t pos = 0;
				int icode = std::stoi(line, &pos);
				result.push_back(std::make_pair(icode, line.substr(pos)));
				if (pos == 3 && line[pos] == ' ')
					return result;
			}
			data = _socket.read();
		}
	}

private:
	TS _socket;
};
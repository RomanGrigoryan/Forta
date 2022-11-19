#pragma once
#include "FTPClient.h"

#include <iostream>
#include <functional>
#include <map>
#include "Socket.h"
#include "SQLiteDatabase.h" 

#include "CLIArguments.h"

class CLIApplication 
{
public:
	int exec(const CLIArguments& arg);
	std::string help() const;
private:
	void fillDbWithRandom();

private:
	FTPClient<Socket> _ftpClient;
	SQLiteDatabase _db;
};

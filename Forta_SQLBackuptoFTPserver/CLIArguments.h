#pragma once

#include <string>

struct CLIArguments
{
	static CLIArguments parseArgs(int argc, char* argv[]);

	//SQLite config
	std::string dbName;
	//FTP config
	std::string ftpHost;
	uint16_t ftpPort;
	std::string ftpUserName;
	std::string ftpPassword;
	std::string ftpPath;
	//internal use
	std::string error;
	bool help = false;

};
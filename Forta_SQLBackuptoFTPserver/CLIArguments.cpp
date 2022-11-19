#include "CLIArguments.h"
	
CLIArguments CLIArguments::parseArgs(int argc, char* argv[])
{
	CLIArguments result;

	for (int i = 2; i < argc; i += 2) {
		std::string key(argv[i - 1]);
		std::string value(argv[i]);
		if (key == "-db")
		{
			result.dbName = value;
		}
		else if (key == "-h") {
			result.ftpHost = value;
		}
		else if (key == "-p") {
			result.ftpPort = std::atoi(value.c_str());
		}
		else if (key == "-u") {
			result.ftpUserName = value;
		}
		else if (key == "-pw") {
			result.ftpPassword = value;
		}
		else if (key == "-fp") {
			result.ftpPath = value;
		}
		else if (key == "-?") {
			result.help = true;
			return result;
		}
	}

	if (result.dbName.empty()){
		result.error = "Database name is empty!";
	}
	if (result.ftpHost.empty()) {
		result.error = "FTP host is empty!";
	}
	if (result.ftpPort == 0) {
		result.error = "FTP port is empty!";
	}
	if (result.ftpPath.empty()) {
		result.error = "FTP path is empty!";
	}
	if (result.dbName.empty()) {
		result.error = "Database name is empty!";
	}
	return result;
}
#include "CLIApplication.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <iterator>
#include <algorithm>
#include <cstring>
#include <sstream>


int CLIApplication::exec(const CLIArguments& arg)
{
	try {

		static const std::string dumpFileName = "db.dump";
		std::cout << "Connecting to database" << std::endl;
		if (!_db.open(arg.dbName)) {
			throw std::runtime_error("Fails to open database!");
		}
		std::cout << "Filling database" << std::endl;
		fillDbWithRandom();

		if (!_db.crateDump(dumpFileName)) {
			throw std::runtime_error("Fails to crate database dump!");
		}

		std::cout << "Connecting to FTP server" << std::endl;
		_ftpClient.connctToHost(arg.ftpHost, arg.ftpPort);

		if (!arg.ftpUserName.empty()) {
			std::cout << "Logging into FTP server" << std::endl;
			_ftpClient.login(arg.ftpUserName, arg.ftpPassword);
		}
		std::cout << "Transferring file to FTP server" << std::endl;
		_ftpClient.put(dumpFileName, arg.ftpPath + "/" + dumpFileName);
	}
	catch (FTPException& exp) {
		std::cerr << "FTP exception: " << exp.what() << "\n";
		if (exp.getResponse().empty()) {
			std::cerr << "FTP server respons:\n";
			for (auto& r : exp.getResponse()) {
				std::cerr << r.first << ":" << r.second << "\n";
			}
		}
		return 2;
	}
	catch (std::exception& exp) {
		std::cerr << "An error ocured: " << exp.what() << "\n";
		return 3;
	}
	std::cout << "Complete" << std::endl;
	return 0;
}

void CLIApplication::fillDbWithRandom()
{
	std::string sql = "CREATE TABLE IF NOT EXISTS TEST("
		"id INTEGER PRIMARY KEY NOT NULL,"
		"x REAL NOT NULL,"
		"y REAL NOT NULL,"
		"z REAL NOT NULL"
		");";
	auto result = _db.executeQuery(sql);

	if (result) {
		std::string isql = "INSERT INTO TEST (x,y,z)"
			" SELECT RANDOM(), RANDOM(), RANDOM()"
			" FROM(SELECT * FROM("
				" (SELECT 0 UNION ALL SELECT 1) t2,"
				" (SELECT 0 UNION ALL SELECT 1) t4,"
				" (SELECT 0 UNION ALL SELECT 1) t8,"
				" (SELECT 0 UNION ALL SELECT 1) t16,"
				" (SELECT 0 UNION ALL SELECT 1) t32,"
				" (SELECT 0 UNION ALL SELECT 1) t64,"
				" (SELECT 0 UNION ALL SELECT 1) t128,"
				" (SELECT 0 UNION ALL SELECT 1) t256,"
				" (SELECT 0 UNION ALL SELECT 1) t512,"
				" (SELECT 0 UNION ALL SELECT 1) t1024,"
				" (SELECT 0 UNION ALL SELECT 1) t2048"
			")"
			") LIMIT 246;\n";
		if (!_db.executeQuery(isql))
			throw std::runtime_error("Random data denereaton fauls!");
	}
}

std::string CLIApplication::help() const
{
	return
		"Arguments: -db <dbName> -h <host> -p <port> -fp <path> [-u <username>] [-pw <password>]\r\n"
		"\t-db: Database name\r\n"
		"\t-h: FTP host name\r\n"
		"\t-p: FTP port\r\n"
		"\t-fp: Directory path on FTP server\r\n"
		"\t-u: FTP user name (optional)\r\n"
		"\t-pw: FTP password (optional)\r\n"
		"\t-?: Help\r\n";
}
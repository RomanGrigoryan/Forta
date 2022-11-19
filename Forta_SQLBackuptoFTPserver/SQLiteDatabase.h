#pragma once

#include <string>
#include <vector>
#include <variant>
#include <array>
struct sqlite3;

class SQLiteDatabase
{
public:

	enum ColumnIndex
	{
		NULL_VALUE,
		INT_VALUE,
		DOUBLE_VALUE,
		STRING
	};


	using Column = std::variant<nullptr_t, int, double, std::string>;
	using Columns = std::vector<Column>;
	using Rows = std::vector<Columns>;

	class QueryResult
	{
	public:
		operator bool() const
		{
			return result;
		}
		bool result = false;
		Rows rows;
	};


	bool open(const std::string& fileName);
	bool crateTable();
	bool crateDump(const std::string& dumpFileName);
	QueryResult executeQuery(const std::string& query);
private:

	sqlite3* _db;
};


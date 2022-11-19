#include "SQLiteDatabase.h"

#include "sqlite-amalgamation-3400000/sqlite3.h"

#include <fstream>
#include <iomanip>


bool SQLiteDatabase::open(const std::string& fileName)
{
   return SQLITE_OK == sqlite3_open(fileName.c_str(), &_db) ;
}

bool SQLiteDatabase::crateTable()
{

    return true;
}

SQLiteDatabase::QueryResult SQLiteDatabase::executeQuery(const std::string& query)
{
    QueryResult result; 
    sqlite3_stmt* res;
    int rc = sqlite3_prepare_v2(_db, query.c_str(), -1, &res, 0);
    if (rc != SQLITE_OK) {
        return result;
    }
    const int columnCount = sqlite3_column_count(res);
    while (rc = sqlite3_step(res) == SQLITE_ROW) {
        Columns cls;
        for (int i = 0; i < columnCount; ++i) {
            Column column;
            switch (sqlite3_column_type(res, i))
            {
            case SQLITE_NULL:
                break;
            case SQLITE_INTEGER:
                column = sqlite3_column_int(res, i);
                break;
            case SQLITE_FLOAT:
                column = sqlite3_column_double(res, i);
                break;
            case SQLITE_BLOB:
                break;
            case SQLITE_TEXT:
                column = std::string((char*)sqlite3_column_text(res, i));
                break;
            default:
                break;
            }
            cls.push_back(column);
        }
        result.rows.push_back(cls);
    }
    sqlite3_finalize(res);
    result.result = true;
    return result;
}


bool SQLiteDatabase::crateDump(const std::string& dumpFile)
{
    // Open text stream to the file
    std::ofstream fileStream;
    fileStream.open(dumpFile, std::ofstream::out | std::ofstream::trunc);
    if (!fileStream.is_open())
        return false;
    const auto default_precision{ fileStream.precision() };
    fileStream << "BEGIN TRANSACTION;\n";

    auto tables = executeQuery("SELECT name, sql FROM sqlite_master where type = 'table';");
    if (tables) {
        for (const auto& row : tables.rows) {
            std::string table_query = std::get<ColumnIndex::STRING>(row[1]);
            const int createTableLength = 12;
            table_query.replace(0, createTableLength, "CREATE TABLE IF NOT EXISTS");
            fileStream << table_query << ";\n";
        }

        for (const auto& tRow : tables.rows) {
            std::string tableName = std::get<ColumnIndex::STRING>(tRow[0]);
            std::string tableInfoQuery = "PRAGMA table_info ('" + tableName + "');";
            auto table_columnInof = executeQuery(tableInfoQuery);
            if (table_columnInof) {
                std::string valueNames;
                for (const auto& tciRow : table_columnInof.rows) {
                    valueNames += std::get<ColumnIndex::STRING>(tciRow[1]) + ',';
                }
                valueNames.pop_back();

                std::string dataQuery = "SELECT * FROM `" + tableName + "`;";
                auto tableData = executeQuery(dataQuery);
                if (tableData && !tableData.rows.empty()) {
                    fileStream << "INSERT INTO `" << tableName << "` (" << valueNames << ") VALUES \n";
                    for (size_t row = 0; row < tableData.rows.size(); ++row) {
                        fileStream << " (";
                        auto& dRow = tableData.rows[row];
                        for (size_t col = 0; col < dRow.size(); ++col) {
                            auto& dCol = dRow[col];
                            switch (dCol.index())
                            {
                            case ColumnIndex::NULL_VALUE: {
                                fileStream << "NULL";
                            }
                                                        break;
                            case ColumnIndex::INT_VALUE: {
                                int intValue = std::get<ColumnIndex::INT_VALUE>(dCol);
                                fileStream << intValue;
                            }
                                                       break;
                            case ColumnIndex::DOUBLE_VALUE: {
                                double doubleValue = std::get<ColumnIndex::DOUBLE_VALUE>(dCol);
                                fileStream << std::setprecision(6) << doubleValue << std::setprecision(default_precision);
                            }
                                                          break;
                            case ColumnIndex::STRING: {
                                std::string stringValue = std::get<ColumnIndex::STRING>(dCol);
                                fileStream << "'" << stringValue << "'";
                            }
                                                    break;
                            default:
                                break;
                            }
                            if (col != dRow.size() - 1) {
                                fileStream << ",";
                            }
                        }
                        fileStream << ") ";
                        if (row != tableData.rows.size() - 1) {
                            fileStream << ",";
                        }
                        fileStream << "\n";
                    }
                }
            }
        }
    }
    fileStream << "COMMIT;\n";
    fileStream.close();
    return true;
}
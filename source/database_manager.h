/// 
/// ============ DATABASE_MANAGER ============
/// 
/// �������� ����� ��� ������ � PostgreSQL ������ ������
/// ��������� ������������ ����� libpq
/// ��������� SQL - ������� � �������� ����������
/// ������������� ������ ��� ��������� ���������� ������

#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H
#pragma comment(lib, "C:/PostgreSQL/17/lib/libpq.lib")
#include <string>
#include <vector>
#include <C:\PostgreSQL\17\include\libpq-fe.h>
#include "logger.h"

class DatabaseManager {
private:
    PGconn* connection;
    std::string connectionString;

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

public:
    DatabaseManager(const std::string& connStr);
    ~DatabaseManager();

    bool connect();
    void disconnect();

    PGconn* getConnection() const { return connection; }

    std::vector<std::string> getTablesList();
    std::string getTableSchema(const std::string& tableName);
    bool executeQuery(const std::string& query);
};

#endif
/// 
/// ============ DATABASE_MANAGER ============
/// 
/// Основной класс для работы с PostgreSQL базами данных
/// Управляет подключением через libpq
/// Выполняет SQL - запросы и получает результаты
/// Предоставляет методы для получения метаданных таблиц

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
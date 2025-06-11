/// 
/// =============== TABLE_MANNAGER =================
/// 
/// Координирует процесс копирования между источником и целевым БД
/// Генерирует CREATE TABLE запросы на основе схемы
/// Обеспечивает передачу данных

#ifndef TABLE_COPIER_H
#define TABLE_COPIER_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "external/json.hpp"
#include "database_manager.h"
#include "logger.h"

class TableManager {
private:
    DatabaseManager& sourceDB;
    DatabaseManager& targetDB;
    bool isConnected;

    // Выполнение SQL-запроса для обеих баз данных
    bool executeSQL(const std::string& query);
    // Загрузки и выполнение SQL-запросов из файла
    bool loadAndExecuteSQLFile(const std::string& filename);
    // Получение списка таблиц из sourceDB
    std::vector<std::string> getTablesList();
    // Получение схемы таблицы в sourceDB
    std::string getTableSchema(const std::string& tableName);

public:
    TableManager(DatabaseManager& src, DatabaseManager& tgt);
    ~TableManager();

    // Обработка команд пользователя
    bool connect(const std::string& connStr);
    void disconnect();
    bool tables();
    bool schema(const std::string& tableName);
    bool exec(const std::string& query);
    bool load(const std::string& filename);
    void exit();
};

#endif
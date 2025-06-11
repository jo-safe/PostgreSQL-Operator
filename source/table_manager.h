/// 
/// =============== TABLE_MANNAGER =================
/// 
/// ������������ ������� ����������� ����� ���������� � ������� ��
/// ���������� CREATE TABLE ������� �� ������ �����
/// ������������ �������� ������

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

    // ���������� SQL-������� ��� ����� ��� ������
    bool executeSQL(const std::string& query);
    // �������� � ���������� SQL-�������� �� �����
    bool loadAndExecuteSQLFile(const std::string& filename);
    // ��������� ������ ������ �� sourceDB
    std::vector<std::string> getTablesList();
    // ��������� ����� ������� � sourceDB
    std::string getTableSchema(const std::string& tableName);

public:
    TableManager(DatabaseManager& src, DatabaseManager& tgt);
    ~TableManager();

    // ��������� ������ ������������
    bool connect(const std::string& connStr);
    void disconnect();
    bool tables();
    bool schema(const std::string& tableName);
    bool exec(const std::string& query);
    bool load(const std::string& filename);
    void exit();
};

#endif
#include "table_manager.h"

TableManager::TableManager(DatabaseManager& src, DatabaseManager& tgt)
    : sourceDB(src), targetDB(tgt), isConnected(false) {
    Logger::log("������ ����������� ���������� ������");
}

TableManager::~TableManager() {
    if (isConnected) {
        disconnect();
    }
}

bool TableManager::executeSQL(const std::string& query) {
    if (query.empty()) {
        Logger::logError("������ SQL-������");
        return false;
    }

    // ���������� ������� � ����� ����� ������
    bool sourceResult = sourceDB.executeQuery(query);
    bool targetResult = targetDB.executeQuery(query);

    return sourceResult && targetResult;
}

bool TableManager::loadAndExecuteSQLFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::logError("�� ������� ������� ����: " + filename);
        return false;
    }

    std::string query;
    bool allSuccess = true;

    while (std::getline(file, query)) {
        // �������� ������ ��������
        query.erase(0, query.find_first_not_of(" \t\r\n"));
        query.erase(query.find_last_not_of(" \t\r\n") + 1);

        if (!query.empty() && query[0] != '-') {
            if (!executeSQL(query)) {
                allSuccess = false;
            }
        }
    }

    return allSuccess;
}

std::vector<std::string> TableManager::getTablesList() {
    std::vector<std::string> tables;

    PGresult* res = PQexec(sourceDB.getConnection(),
        "SELECT tablename FROM pg_tables WHERE schemaname = 'public'");

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        int rows = PQntuples(res);
        for (int i = 0; i < rows; ++i) {
            tables.push_back(std::string(PQgetvalue(res, i, 0)));
        }
    }

    PQclear(res);
    Logger::log("������� ������ ������ �� ���� ������");
    return tables;
}

std::string TableManager::getTableSchema(const std::string& tableName) {
    std::string schema;

    std::string query = "SELECT column_name, data_type "
        "FROM information_schema.columns "
        "WHERE table_name = '" + tableName + "'";

    PGresult* res = PQexec(sourceDB.getConnection(), query.c_str());
    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        int rows = PQntuples(res);
        for (int i = 0; i < rows; ++i) {
            schema += std::string(PQgetvalue(res, i, 0)) + " " +
                std::string(PQgetvalue(res, i, 1)) + ", ";
        }
        if (!schema.empty()) {
            schema.pop_back();
            schema.pop_back(); // �������� ��������� ", "
        }
    }

    PQclear(res);
    Logger::log("�������� ����� �������: " + tableName);
    return schema;
}

bool TableManager::connect(const std::string& connStr) {
    if (isConnected) {
        Logger::logError("��� ���������� � ���� ������");
        return false;
    }

    if (!sourceDB.connect() || !targetDB.connect()) {
        Logger::logError("������ ��� ����������� � ����� ������");
        return false;
    }

    isConnected = true;
    Logger::log("�������� ����������� � ����� ������");
    return true;
}

void TableManager::disconnect() {
    if (!isConnected) {
        Logger::log("�� ���������� � ���� ������");
        return;
    }

    sourceDB.disconnect();
    targetDB.disconnect();
    isConnected = false;
    Logger::log("���������� �� ��� ������ ���������");
}

bool TableManager::tables() {
    if (!isConnected) {
        Logger::logError("��� ����������� � ���� ������");
        return false;
    }

    std::vector<std::string> tableList = getTablesList();
    if (tableList.empty()) {
        Logger::logError("�� ������� �������� ������ ������");
        return false;
    }

    Logger::log("������� ������ ������:");
    for (const auto& table : tableList) {
        std::cout << table << std::endl;
    }
    return true;
}

bool TableManager::schema(const std::string& tableName) {
    if (!isConnected) {
        Logger::logError("��� ����������� � ���� ������");
        return false;
    }

    std::string schema = getTableSchema(tableName);
    if (schema.empty()) {
        Logger::logError("�� ������� �������� ����� �������: " + tableName);
        return false;
    }

    std::cout << "����� ������� " << tableName << ":" << std::endl;
    std::cout << schema << std::endl;
    Logger::log("�������� � �������� ����� �������: " + tableName);
    return true;
}

bool TableManager::exec(const std::string& query) {
    if (!isConnected) {
        Logger::logError("��� ����������� � ���� ������");
        return false;
    }

    if (query.empty()) {
        Logger::logError("������ SQL-������");
        return false;
    }

    bool result = executeSQL(query);
    if (result) {
        Logger::log("SQL-������ �������� �������");
    }
    else {
        Logger::logError("������ ��� ���������� SQL-�������");
    }
    return result;
}

bool TableManager::load(const std::string& filename) {
    if (!isConnected) {
        Logger::logError("��� ����������� � ���� ������");
        return false;
    }

    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            Logger::logError("�� ������� ������� ����: " + filename);
            return false;
        }

        nlohmann::json json;
        file >> json;

        if (!json.is_array()) {
            Logger::logError("�������� ������ JSON-�����");
            return false;
        }

        bool allSuccess = true;
        for (const auto& query : json) {
            if (!query.is_string()) {
                Logger::logError("�������� ������ ������� � JSON");
                allSuccess = false;
                continue;
            }
            if (!executeSQL(query.get<std::string>())) {
                allSuccess = false;
            }
        }

        Logger::log("�������� � �������� ���� SQL-������: " + filename);
        return allSuccess;
    }
    catch (const std::exception& e) {
        Logger::logError("������ ��� ��������� JSON-�����: " + std::string(e.what()));
        return false;
    }
}

void TableManager::exit() {
    Logger::log("��������� ���������� ������ ���������");
    disconnect();
}
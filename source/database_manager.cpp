#include "database_manager.h"

DatabaseManager::DatabaseManager(const std::string& connStr) : connectionString(connStr), connection(nullptr) {
    Logger::log("Создан менеджер базы данных");
}

DatabaseManager::~DatabaseManager() {
    if (connection) {
        PQfinish(connection);
        Logger::log("Закрыто соединение с базой данных");
    }
}

bool DatabaseManager::connect() {
    connection = PQconnectdb(connectionString.c_str());
    if (PQstatus(connection) != CONNECTION_OK) {
        Logger::logError("Ошибка подключения к базе данных: " + std::string(PQerrorMessage(connection)));
        return false;
    }
    Logger::log("Успешное подключение к базе данных");
    return true;
}

void DatabaseManager::disconnect() {
    if (connection) {
        PQfinish(connection);
        connection = nullptr;
        Logger::log("Соединение с базой данных закрыто");
    }
}

std::vector<std::string> DatabaseManager::getTablesList() {
    std::vector<std::string> tables;
    PGresult* res = PQexec(connection,
        "SELECT tablename FROM pg_tables WHERE schemaname = 'public'");

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        int rows = PQntuples(res);
        for (int i = 0; i < rows; ++i) {
            tables.push_back(std::string(PQgetvalue(res, i, 0)));
        }
    }

    PQclear(res);
    Logger::log("Получен список таблиц из базы данных");
    return tables;
}

std::string DatabaseManager::getTableSchema(const std::string& tableName) {
    std::string schema;
    std::string query = "SELECT column_name, data_type "
        "FROM information_schema.columns "
        "WHERE table_name = '" + tableName + "'";

    PGresult* res = PQexec(connection, query.c_str());
    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        int rows = PQntuples(res);
        for (int i = 0; i < rows; ++i) {
            schema += std::string(PQgetvalue(res, i, 0)) + " " +
                std::string(PQgetvalue(res, i, 1)) + ", ";
        }
        if (!schema.empty()) {
            schema.pop_back();
            schema.pop_back();
        }
    }

    PQclear(res);
    Logger::log("Получена схема таблицы: " + tableName);
    return schema;
}

bool DatabaseManager::executeQuery(const std::string& query) {
    PGresult* res = PQexec(connection, query.c_str());
    bool success = (PQresultStatus(res) == PGRES_COMMAND_OK);

    if (!success) {
        Logger::logError("Ошибка выполнения запроса: " +
            std::string(PQerrorMessage(connection)));
    }
    else {
        Logger::log("Запрос '" + query + "' выполнен успешно");
    }

    PQclear(res);
    return success;
}
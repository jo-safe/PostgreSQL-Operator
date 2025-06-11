#include "table_manager.h"

TableManager::TableManager(DatabaseManager& src, DatabaseManager& tgt)
    : sourceDB(src), targetDB(tgt), isConnected(false) {
    Logger::log("Создан обновленный копировщик таблиц");
}

TableManager::~TableManager() {
    if (isConnected) {
        disconnect();
    }
}

bool TableManager::executeSQL(const std::string& query) {
    if (query.empty()) {
        Logger::logError("Пустой SQL-запрос");
        return false;
    }

    // Выполнение запроса в обеих базах данных
    bool sourceResult = sourceDB.executeQuery(query);
    bool targetResult = targetDB.executeQuery(query);

    return sourceResult && targetResult;
}

bool TableManager::loadAndExecuteSQLFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::logError("Не удалось открыть файл: " + filename);
        return false;
    }

    std::string query;
    bool allSuccess = true;

    while (std::getline(file, query)) {
        // Удаление лишних символов
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
    Logger::log("Получен список таблиц из базы данных");
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
            schema.pop_back(); // Удаление последних ", "
        }
    }

    PQclear(res);
    Logger::log("Получена схема таблицы: " + tableName);
    return schema;
}

bool TableManager::connect(const std::string& connStr) {
    if (isConnected) {
        Logger::logError("Уже подключено к базе данных");
        return false;
    }

    if (!sourceDB.connect() || !targetDB.connect()) {
        Logger::logError("Ошибка при подключении к базам данных");
        return false;
    }

    isConnected = true;
    Logger::log("Успешное подключение к базам данных");
    return true;
}

void TableManager::disconnect() {
    if (!isConnected) {
        Logger::log("Не подключено к базе данных");
        return;
    }

    sourceDB.disconnect();
    targetDB.disconnect();
    isConnected = false;
    Logger::log("Отключение от баз данных выполнено");
}

bool TableManager::tables() {
    if (!isConnected) {
        Logger::logError("Нет подключения к базе данных");
        return false;
    }

    std::vector<std::string> tableList = getTablesList();
    if (tableList.empty()) {
        Logger::logError("Не удалось получить список таблиц");
        return false;
    }

    Logger::log("Получен список таблиц:");
    for (const auto& table : tableList) {
        std::cout << table << std::endl;
    }
    return true;
}

bool TableManager::schema(const std::string& tableName) {
    if (!isConnected) {
        Logger::logError("Нет подключения к базе данных");
        return false;
    }

    std::string schema = getTableSchema(tableName);
    if (schema.empty()) {
        Logger::logError("Не удалось получить схему таблицы: " + tableName);
        return false;
    }

    std::cout << "Схема таблицы " << tableName << ":" << std::endl;
    std::cout << schema << std::endl;
    Logger::log("Получена и выведена схема таблицы: " + tableName);
    return true;
}

bool TableManager::exec(const std::string& query) {
    if (!isConnected) {
        Logger::logError("Нет подключения к базе данных");
        return false;
    }

    if (query.empty()) {
        Logger::logError("Пустой SQL-запрос");
        return false;
    }

    bool result = executeSQL(query);
    if (result) {
        Logger::log("SQL-запрос выполнен успешно");
    }
    else {
        Logger::logError("Ошибка при выполнении SQL-запроса");
    }
    return result;
}

bool TableManager::load(const std::string& filename) {
    if (!isConnected) {
        Logger::logError("Нет подключения к базе данных");
        return false;
    }

    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            Logger::logError("Не удалось открыть файл: " + filename);
            return false;
        }

        nlohmann::json json;
        file >> json;

        if (!json.is_array()) {
            Logger::logError("Неверный формат JSON-файла");
            return false;
        }

        bool allSuccess = true;
        for (const auto& query : json) {
            if (!query.is_string()) {
                Logger::logError("Неверный формат запроса в JSON");
                allSuccess = false;
                continue;
            }
            if (!executeSQL(query.get<std::string>())) {
                allSuccess = false;
            }
        }

        Logger::log("Загружен и выполнен файл SQL-команд: " + filename);
        return allSuccess;
    }
    catch (const std::exception& e) {
        Logger::logError("Ошибка при обработке JSON-файла: " + std::string(e.what()));
        return false;
    }
}

void TableManager::exit() {
    Logger::log("Запрошено завершение работы программы");
    disconnect();
}
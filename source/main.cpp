/// 
/// ============================ MAIN ==============================
/// 
/// Главный файл кода, отвечающий за создание соединения с БД и обработку команд пользоватля.
/// Доступные команды:
///     - connect <connection_string> - подключение к БД
///     - disconnect - отключени от БД
///     - tables - получения списка таблиц исходной БД
///     - schema <table_name> - получение схемы таблицы из исходной БД по ее имени
///     - exec <SQL_query> - выполнение SQL-запроса для обеих таблиц
///     - load <filename> - загрузка SQL-запросов из файла и их выполнение
///     - exit - завершение работы программы
///     - help - справка
///     Команды вводятся пользователем с клавиатуры

#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>
#include "table_manager.h"

void printHelp() {
    std::cout << "Доступные команды:" << std::endl;
    std::cout << "  connect <connection_string> - Подключение к базе данных" << std::endl;
    std::cout << "  формат <connection_string> : 'dbname=имя_базы host=хост port=порт user=пользователь password=пароль'" << std::endl;
    std::cout << "  disconnect - Отключение от базы данных" << std::endl;
    std::cout << "  tables - Показать список всех таблиц" << std::endl;
    std::cout << "  schema <table_name> - Показать схему таблицы" << std::endl;
    std::cout << "  exec <SQL_query> - Выполнить SQL-запрос" << std::endl;
    std::cout << "  load <filename> - Загрузить и выполнить SQL-команды из файла" << std::endl;
    std::cout << "  exit - Завершить работу программы" << std::endl;
    std::cout << "  help - Показать эту справку" << std::endl;
}

int main() {
    // Поддержка кириллицы
    SetConsoleOutputCP(1251);
    std::locale::global(std::locale("Russian_Russia.1251"));
    std::cout.imbue(std::locale());

    // Инициализация логгера
    if (!Logger::initialize()) {
        std::cerr << "Ошибка при инициализации логгера" << std::endl;
        return 1;
    }

    DatabaseManager sourceDB("dbname=source_db");
    DatabaseManager targetDB("dbname=target_db");
    TableManager tableManager(sourceDB, targetDB);

    // Основной цикл обработки команд пользователя
    std::string command;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);

        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;

        if (cmd == "exit") {
            tableManager.exit();
            break;
        }
        else if (cmd == "help") {
            printHelp();
        }
        else if (cmd == "connect") {
            std::string connStr;
            std::getline(iss, connStr);
            if (!connStr.empty()) {
                tableManager.connect(connStr);
            }
            else {
                Logger::logError("Не указана строка подключения");
            }
        }
        else if (cmd == "disconnect") {
            tableManager.disconnect();
        }
        else if (cmd == "tables") {
            tableManager.tables();
        }
        else if (cmd == "schema") {
            std::string tableName;
            iss >> tableName;
            if (!tableName.empty()) {
                tableManager.schema(tableName);
            }
            else {
                Logger::logError("Не указано имя таблицы");
            }
        }
        else if (cmd == "exec") {
            std::string query;
            std::getline(iss, query);
            if (!query.empty()) {
                tableManager.exec(query);
            }
            else {
                Logger::logError("Пустой SQL-запрос");
            }
        }
        else if (cmd == "load") {
            std::string filename;
            iss >> filename;
            if (!filename.empty()) {
                tableManager.load(filename);
            }
            else {
                Logger::logError("Не указано имя файла");
            }
        }
        else {
            Logger::logError("Неизвестная команда: " + cmd);
            std::cout << "Введите 'help' для просмотра доступных команд" << std::endl;
        }
    }

    return 0;
}
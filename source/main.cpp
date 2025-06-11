/// 
/// ============================ MAIN ==============================
/// 
/// ������� ���� ����, ���������� �� �������� ���������� � �� � ��������� ������ �����������.
/// ��������� �������:
///     - connect <connection_string> - ����������� � ��
///     - disconnect - ��������� �� ��
///     - tables - ��������� ������ ������ �������� ��
///     - schema <table_name> - ��������� ����� ������� �� �������� �� �� �� �����
///     - exec <SQL_query> - ���������� SQL-������� ��� ����� ������
///     - load <filename> - �������� SQL-�������� �� ����� � �� ����������
///     - exit - ���������� ������ ���������
///     - help - �������
///     ������� �������� ������������� � ����������

#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>
#include "table_manager.h"

void printHelp() {
    std::cout << "��������� �������:" << std::endl;
    std::cout << "  connect <connection_string> - ����������� � ���� ������" << std::endl;
    std::cout << "  ������ <connection_string> : 'dbname=���_���� host=���� port=���� user=������������ password=������'" << std::endl;
    std::cout << "  disconnect - ���������� �� ���� ������" << std::endl;
    std::cout << "  tables - �������� ������ ���� ������" << std::endl;
    std::cout << "  schema <table_name> - �������� ����� �������" << std::endl;
    std::cout << "  exec <SQL_query> - ��������� SQL-������" << std::endl;
    std::cout << "  load <filename> - ��������� � ��������� SQL-������� �� �����" << std::endl;
    std::cout << "  exit - ��������� ������ ���������" << std::endl;
    std::cout << "  help - �������� ��� �������" << std::endl;
}

int main() {
    // ��������� ���������
    SetConsoleOutputCP(1251);
    std::locale::global(std::locale("Russian_Russia.1251"));
    std::cout.imbue(std::locale());

    // ������������� �������
    if (!Logger::initialize()) {
        std::cerr << "������ ��� ������������� �������" << std::endl;
        return 1;
    }

    DatabaseManager sourceDB("dbname=source_db");
    DatabaseManager targetDB("dbname=target_db");
    TableManager tableManager(sourceDB, targetDB);

    // �������� ���� ��������� ������ ������������
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
                Logger::logError("�� ������� ������ �����������");
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
                Logger::logError("�� ������� ��� �������");
            }
        }
        else if (cmd == "exec") {
            std::string query;
            std::getline(iss, query);
            if (!query.empty()) {
                tableManager.exec(query);
            }
            else {
                Logger::logError("������ SQL-������");
            }
        }
        else if (cmd == "load") {
            std::string filename;
            iss >> filename;
            if (!filename.empty()) {
                tableManager.load(filename);
            }
            else {
                Logger::logError("�� ������� ��� �����");
            }
        }
        else {
            Logger::logError("����������� �������: " + cmd);
            std::cout << "������� 'help' ��� ��������� ��������� ������" << std::endl;
        }
    }

    return 0;
}
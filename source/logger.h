/// 
/// ================ LOGGER ==================
/// 
/// Отвечает за ведение лога операций
/// Использует fprintf() для записи в файл
/// Логирует ошибки и ключевые события процесса копирования

#ifndef LOGGER_H
#define LOGGER_H

#define _CRT_SECURE_NO_WARNINGS

#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <chrono>

class Logger {
private:
    static const std::string LOG_FILE;
    static std::ofstream logStream;

    static std::tm get_localtime(std::time_t t);
public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static void log(const std::string& message);
    static void logError(const std::string& message);

    static bool initialize();
};

#endif
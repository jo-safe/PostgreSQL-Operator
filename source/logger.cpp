#include "logger.h"

const std::string Logger::LOG_FILE = "program.log";
std::ofstream Logger::logStream;

bool Logger::initialize() {
    // mode = rewrite
    logStream.open(LOG_FILE);
    if (!logStream.is_open()) {
        std::cerr << "Ошибка при открытии файла лога: " << LOG_FILE << std::endl;
        return false;
    }

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    log(std::string("Программа запущена: ") + std::ctime(&time));

    return true;
}

void Logger::log(const std::string& message) {
    if (!logStream.is_open()) return;

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&time));

    logStream << "[" << timestamp << "] " << message << std::endl;
}

void Logger::logError(const std::string& message) {
    log("ERROR: " + message);
}
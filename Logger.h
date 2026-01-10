#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include <mutex>

class Logger {
public:
    static Logger& getInstance();

    void logMove(const std::string& moveText);
    void logMessage(const std::string& message);
    void logError(const std::string& error);

    // Non-copyable
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger();
    ~Logger();

    void writeWithTimestamp(const std::string& level, const std::string& text);

    std::ofstream m_file;
    std::mutex m_mutex;
};

#endif // LOGGER_H

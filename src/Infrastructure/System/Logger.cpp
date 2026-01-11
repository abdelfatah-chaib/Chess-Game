#include "Services/Logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>

static std::string currentTimestamp() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto time_t_now = system_clock::to_time_t(now);
    std::tm tm_buf{};
#ifdef _WIN32
    localtime_s(&tm_buf, &time_t_now);
#else
    tm_buf = *std::localtime(&time_t_now);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    m_file.open("chess_log.txt", std::ios::out | std::ios::app);
}

Logger::~Logger() {
    if (m_file.is_open()) {
        m_file.flush();
        m_file.close();
    }
}

void Logger::writeWithTimestamp(const std::string& level, const std::string& text) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_file.is_open()) return;
    m_file << "[" << currentTimestamp() << "] " << level << ": " << text << std::endl;
}

void Logger::logMove(const std::string& moveText) { writeWithTimestamp("MOVE", moveText); }
void Logger::logMessage(const std::string& message) { writeWithTimestamp("INFO", message); }
void Logger::logError(const std::string& error) { writeWithTimestamp("ERROR", error); }

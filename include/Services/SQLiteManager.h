#pragma once
#include <string>
#include <vector>
#include <sqlite3.h>

struct GameHistoryEntry {
    int id;
    std::string result;
    std::string playerColor;
    int difficulty;
    int movesCount;
    std::string playedAt;
};

class SQLiteManager {
private:
    sqlite3* db;

public:
    SQLiteManager();
    ~SQLiteManager();

    bool openDB(const std::string& path = "chessgame.db");
    bool createTables();
    bool insertUser(const std::string& username, const std::string& email, const std::string& password);
    bool validateUser(const std::string& email, const std::string& password);
    bool tableExists(const std::string& tableName = "users");

    // Méthodes publiques pour EmailPasswordLoginScreen
    bool prepareStatement(const std::string& sql, sqlite3_stmt** stmt);
    void finalizeStatement(sqlite3_stmt* stmt);
    bool updateStayLoggedIn(int userId, bool state);
    bool getStayLoggedIn(const std::string& email);
    int getUserId(const std::string& email);

    // Game history and statistics
    bool saveGameResult(int userId, const std::string& result, const std::string& playerColor, int difficulty, int movesCount);
    bool updatePlayerStats(int userId, const std::string& result);
    bool getPlayerStats(int userId, int& totalGames, int& wins, int& losses, int& draws);
    std::vector<GameHistoryEntry> getGameHistory(int userId, int limit = 10);
    
    // Testing and debugging
    bool testConnection();
    int getUserCount();
    int getLastStayLoggedInUserId();
    std::string getUsernameById(int userId);
    
    // Password recovery
    bool emailExists(const std::string& email);
    bool resetPassword(const std::string& email, const std::string& newPassword);
};

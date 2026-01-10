#include "SQLiteManager.h"
#include <iostream>

SQLiteManager::SQLiteManager() : db(nullptr) {}
SQLiteManager::~SQLiteManager() { if (db) sqlite3_close(db); }

bool SQLiteManager::openDB(const std::string& path) {
    if (sqlite3_open(path.c_str(), &db) == SQLITE_OK) return true;
    std::cerr << "[SQLiteManager] Failed to open DB: " << sqlite3_errmsg(db) << std::endl;
    return false;
}

bool SQLiteManager::createTables() {
    const char* usersTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL UNIQUE,
            email TEXT NOT NULL UNIQUE,
            password TEXT NOT NULL,
            stayLoggedIn INTEGER DEFAULT 0,
            level INTEGER DEFAULT 0,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );
    )";

    const char* gameHistoryTable = R"(
        CREATE TABLE IF NOT EXISTS game_history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            result TEXT NOT NULL,
            player_color TEXT NOT NULL,
            difficulty INTEGER NOT NULL,
            moves_count INTEGER DEFAULT 0,
            played_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (user_id) REFERENCES users(id)
        );
    )";

    const char* playerStatsTable = R"(
        CREATE TABLE IF NOT EXISTS player_stats (
            user_id INTEGER PRIMARY KEY,
            total_games INTEGER DEFAULT 0,
            wins INTEGER DEFAULT 0,
            losses INTEGER DEFAULT 0,
            draws INTEGER DEFAULT 0,
            FOREIGN KEY (user_id) REFERENCES users(id)
        );
    )";

    char* errMsg = nullptr;

    if (sqlite3_exec(db, usersTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "[SQLiteManager] Failed to create users table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    if (sqlite3_exec(db, gameHistoryTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "[SQLiteManager] Failed to create game_history table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    if (sqlite3_exec(db, playerStatsTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "[SQLiteManager] Failed to create player_stats table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    std::cout << "[SQLiteManager] All tables created successfully.\n";
    return true;
}

bool SQLiteManager::tableExists(const std::string& tableName) {
    std::string sql = "SELECT name FROM sqlite_master WHERE type='table' AND name=?;";
    sqlite3_stmt* stmt;
    if (!prepareStatement(sql, &stmt)) return false;

    sqlite3_bind_text(stmt, 1, tableName.c_str(), -1, SQLITE_TRANSIENT);
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    finalizeStatement(stmt);
    return exists;
}

bool SQLiteManager::insertUser(const std::string& username, const std::string& email, const std::string& password) {
    std::cout << "[SQLiteManager] Attempting to insert user: " << username << " (" << email << ")" << std::endl;
    
    const char* sql = "INSERT INTO users (username, email, password, stayLoggedIn) VALUES (?, ?, ?, 0);";
    sqlite3_stmt* stmt;
    
    if (!prepareStatement(sql, &stmt)) {
        std::cerr << "[SQLiteManager] Failed to prepare INSERT statement" << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, password.c_str(), -1, SQLITE_TRANSIENT);

    int result = sqlite3_step(stmt);
    bool success = (result == SQLITE_DONE);
    
    if (success) {
        std::cout << "[SQLiteManager] ✓ User inserted successfully: " << username << std::endl;
    } else {
        std::cerr << "[SQLiteManager] ✗ Failed to insert user: " << sqlite3_errmsg(db) << std::endl;
        std::cerr << "[SQLiteManager] Error code: " << result << std::endl;
    }
    
    finalizeStatement(stmt);
    return success;
}

bool SQLiteManager::validateUser(const std::string& email, const std::string& password) {
    const char* sql = "SELECT password FROM users WHERE email=?;";
    sqlite3_stmt* stmt;
    if (!prepareStatement(sql, &stmt)) return false;

    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);

    bool valid = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* dbPass = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (dbPass) valid = (password == std::string(dbPass));
    }

    finalizeStatement(stmt);
    return valid;
}

bool SQLiteManager::prepareStatement(const std::string& sql, sqlite3_stmt** stmt) {
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[SQLiteManager] Failed to prepare: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    return true;
}

void SQLiteManager::finalizeStatement(sqlite3_stmt* stmt) {
    if (stmt) sqlite3_finalize(stmt);
}

bool SQLiteManager::updateStayLoggedIn(int userId, bool state) {
    const char* sql = "UPDATE users SET stayLoggedIn=? WHERE id=?";
    sqlite3_stmt* stmt;

    if (!prepareStatement(sql, &stmt)) return false;

    sqlite3_bind_int(stmt, 1, state ? 1 : 0);
    sqlite3_bind_int(stmt, 2, userId);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    finalizeStatement(stmt);
    return success;
}

bool SQLiteManager::getStayLoggedIn(const std::string& email) {
    const char* sql = "SELECT stayLoggedIn FROM users WHERE email=?";
    sqlite3_stmt* stmt;

    if (!prepareStatement(sql, &stmt)) return false;
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);

    bool stay = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        stay = sqlite3_column_int(stmt, 0) == 1;
    }
    finalizeStatement(stmt);
    return stay;
}

int SQLiteManager::getUserId(const std::string& email) {
    const char* sql = "SELECT id FROM users WHERE email=?";
    sqlite3_stmt* stmt;

    if (!prepareStatement(sql, &stmt)) return -1;
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);

    int userId = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        userId = sqlite3_column_int(stmt, 0);
    }
    finalizeStatement(stmt);
    return userId;
}

bool SQLiteManager::saveGameResult(int userId, const std::string& result, const std::string& playerColor, int difficulty, int movesCount) {
    const char* sql = "INSERT INTO game_history (user_id, result, player_color, difficulty, moves_count) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    
    if (!prepareStatement(sql, &stmt)) return false;

    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_text(stmt, 2, result.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, playerColor.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, difficulty);
    sqlite3_bind_int(stmt, 5, movesCount);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    finalizeStatement(stmt);

    if (success) {
        std::cout << "[SQLiteManager] Game result saved for user " << userId << std::endl;
        updatePlayerStats(userId, result);
    }

    return success;
}

bool SQLiteManager::updatePlayerStats(int userId, const std::string& result) {
    // First, check if stats exist for this user
    const char* checkSql = "SELECT total_games FROM player_stats WHERE user_id=?";
    sqlite3_stmt* stmt;

    if (!prepareStatement(checkSql, &stmt)) return false;
    sqlite3_bind_int(stmt, 1, userId);

    bool statsExist = (sqlite3_step(stmt) == SQLITE_ROW);
    finalizeStatement(stmt);

    std::string sql;
    if (!statsExist) {
        // Create initial stats entry
        sql = "INSERT INTO player_stats (user_id, total_games, wins, losses, draws) VALUES (?, 1, ?, ?, ?);";
    } else {
        // Update existing stats
        if (result == "win") {
            sql = "UPDATE player_stats SET total_games = total_games + 1, wins = wins + 1 WHERE user_id = ?;";
        } else if (result == "loss") {
            sql = "UPDATE player_stats SET total_games = total_games + 1, losses = losses + 1 WHERE user_id = ?;";
        } else if (result == "draw") {
            sql = "UPDATE player_stats SET total_games = total_games + 1, draws = draws + 1 WHERE user_id = ?;";
        }
    }

    if (!prepareStatement(sql, &stmt)) return false;

    sqlite3_bind_int(stmt, 1, userId);
    
    if (!statsExist) {
        // For new entry, bind the initial values
        sqlite3_bind_int(stmt, 2, result == "win" ? 1 : 0);
        sqlite3_bind_int(stmt, 3, result == "loss" ? 1 : 0);
        sqlite3_bind_int(stmt, 4, result == "draw" ? 1 : 0);
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    finalizeStatement(stmt);

    return success;
}

bool SQLiteManager::getPlayerStats(int userId, int& totalGames, int& wins, int& losses, int& draws) {
    const char* sql = "SELECT total_games, wins, losses, draws FROM player_stats WHERE user_id=?";
    sqlite3_stmt* stmt;

    if (!prepareStatement(sql, &stmt)) return false;
    sqlite3_bind_int(stmt, 1, userId);

    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        totalGames = sqlite3_column_int(stmt, 0);
        wins = sqlite3_column_int(stmt, 1);
        losses = sqlite3_column_int(stmt, 2);
        draws = sqlite3_column_int(stmt, 3);
        found = true;
    } else {
        // No stats yet - return zeros
        totalGames = wins = losses = draws = 0;
    }

    finalizeStatement(stmt);
    return found;
}

bool SQLiteManager::testConnection() {
    if (!db) {
        std::cerr << "[SQLiteManager] No database connection!" << std::endl;
        return false;
    }
    
    const char* testSql = "SELECT sqlite_version();";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, testSql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[SQLiteManager] Failed to prepare test query" << std::endl;
        return false;
    }
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* version = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::cout << "[SQLiteManager] ✓ SQLite version: " << version << std::endl;
        sqlite3_finalize(stmt);
        return true;
    }
    
    sqlite3_finalize(stmt);
    return false;
}

int SQLiteManager::getUserCount() {
    const char* sql = "SELECT COUNT(*) FROM users;";
    sqlite3_stmt* stmt;
    
    if (!prepareStatement(sql, &stmt)) return -1;
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    finalizeStatement(stmt);
    std::cout << "[SQLiteManager] Total users in database: " << count << std::endl;
    return count;
}

std::vector<GameHistoryEntry> SQLiteManager::getGameHistory(int userId, int limit) {
    std::vector<GameHistoryEntry> history;
    
    const char* sql = "SELECT id, result, player_color, difficulty, moves_count, "
                     "datetime(played_at, 'localtime') as played_at "
                     "FROM game_history WHERE user_id=? "
                     "ORDER BY played_at DESC LIMIT ?;";
    
    sqlite3_stmt* stmt;
    if (!prepareStatement(sql, &stmt)) return history;
    
    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, limit);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        GameHistoryEntry entry;
        entry.id = sqlite3_column_int(stmt, 0);
        entry.result = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        entry.playerColor = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        entry.difficulty = sqlite3_column_int(stmt, 3);
        entry.movesCount = sqlite3_column_int(stmt, 4);
        
        const char* dateStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        entry.playedAt = dateStr ? dateStr : "";
        
        history.push_back(entry);
    }
    
    finalizeStatement(stmt);
    std::cout << "[SQLiteManager] Retrieved " << history.size() << " game history entries" << std::endl;
    return history;
}

int SQLiteManager::getLastStayLoggedInUserId() {
    const char* sql = "SELECT id FROM users WHERE stayLoggedIn=1 ORDER BY id DESC LIMIT 1;";
    sqlite3_stmt* stmt;
    
    if (!prepareStatement(sql, &stmt)) return -1;
    
    int userId = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        userId = sqlite3_column_int(stmt, 0);
    }
    
    finalizeStatement(stmt);
    return userId;
}

std::string SQLiteManager::getUsernameById(int userId) {
    const char* sql = "SELECT username FROM users WHERE id=?;";
    sqlite3_stmt* stmt;
    
    if (!prepareStatement(sql, &stmt)) return "";
    sqlite3_bind_int(stmt, 1, userId);
    
    std::string username;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (name) username = name;
    }
    
    finalizeStatement(stmt);
    return username;
}

bool SQLiteManager::emailExists(const std::string& email) {
    const char* sql = "SELECT COUNT(*) FROM users WHERE email=?;";
    sqlite3_stmt* stmt;
    
    if (!prepareStatement(sql, &stmt)) return false;
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);
    
    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(stmt, 0) > 0;
    }
    
    finalizeStatement(stmt);
    return exists;
}

bool SQLiteManager::resetPassword(const std::string& email, const std::string& newPassword) {
    std::cout << "[SQLiteManager] Resetting password for: " << email << std::endl;
    
    const char* sql = "UPDATE users SET password=? WHERE email=?;";
    sqlite3_stmt* stmt;
    
    if (!prepareStatement(sql, &stmt)) return false;
    
    sqlite3_bind_text(stmt, 1, newPassword.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_TRANSIENT);
    
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    
    if (success) {
        std::cout << "[SQLiteManager] ✓ Password reset successful" << std::endl;
    } else {
        std::cerr << "[SQLiteManager] ✗ Failed to reset password: " << sqlite3_errmsg(db) << std::endl;
    }
    
    finalizeStatement(stmt);
    return success;
}

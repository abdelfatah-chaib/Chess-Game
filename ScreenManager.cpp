#include "ScreenManager.h"
#include <iostream>

ScreenManager::ScreenManager(TextureManager* texMgr, FontManager* fontMgr)
    : currentState(STATE_TITLE), textureManager(texMgr), fontManager(fontMgr) {
    std::cout << "[ScreenManager] Initialized with STATE_TITLE" << std::endl;
}

void ScreenManager::initDatabase(const std::string& dbPath) {
    std::cout << "[ScreenManager] ===========================================" << std::endl;
    std::cout << "[ScreenManager] DATABASE INITIALIZATION" << std::endl;
    std::cout << "[ScreenManager] ===========================================" << std::endl;
    std::cout << "[ScreenManager] Attempting to open database at: " << dbPath << std::endl;
    
    if (databaseManager.openDB(dbPath)) {
        std::cout << "[ScreenManager] ✓ SQLite DB opened successfully at: " << dbPath << std::endl;

        // Test connection
        std::cout << "[ScreenManager] Testing database connection..." << std::endl;
        if (databaseManager.testConnection()) {
            std::cout << "[ScreenManager] ✓ Database connection test passed" << std::endl;
        } else {
            std::cerr << "[ScreenManager] ✗ Database connection test failed" << std::endl;
        }

        // Create tables if they don't exist
        std::cout << "[ScreenManager] Creating database tables..." << std::endl;
        if (databaseManager.createTables()) {
            std::cout << "[ScreenManager] ✓ Database tables initialized successfully.\n";
        } else {
            std::cerr << "[ScreenManager] ✗ ERROR: Failed to create database tables!\n";
        }

        // Verify tables exist
        std::cout << "[ScreenManager] Verifying tables..." << std::endl;
        if (databaseManager.tableExists("users")) {
            std::cout << "[ScreenManager] ✓ Table 'users' exists.\n";
        } else {
            std::cout << "[ScreenManager] ✗ Table 'users' doesn't exist!\n";
        }
        
        if (databaseManager.tableExists("game_history")) {
            std::cout << "[ScreenManager] ✓ Table 'game_history' exists.\n";
        } else {
            std::cout << "[ScreenManager] ✗ Table 'game_history' doesn't exist!\n";
        }
        
        if (databaseManager.tableExists("player_stats")) {
            std::cout << "[ScreenManager] ✓ Table 'player_stats' exists.\n";
        } else {
            std::cout << "[ScreenManager] ✗ Table 'player_stats' doesn't exist!\n";
        }
        
        // Get user count
        databaseManager.getUserCount();
        
        std::cout << "[ScreenManager] ===========================================" << std::endl;
        std::cout << "[ScreenManager] DATABASE READY" << std::endl;
        std::cout << "[ScreenManager] ===========================================" << std::endl;

    }
    else {
        std::cerr << "[ScreenManager] ===========================================" << std::endl;
        std::cerr << "[ScreenManager] ✗ ERROR: Failed to open SQLite DB at: " << dbPath << std::endl;
        std::cerr << "[ScreenManager] Make sure SQLite3 is properly installed and linked.\n";
        std::cerr << "[ScreenManager] ===========================================" << std::endl;
    }
}

void ScreenManager::registerScreen(AppState state, std::unique_ptr<Screen> screen) {
    screens[state] = std::move(screen);
    std::cout << "[ScreenManager] Screen registered for state: " << state << std::endl;
}

void ScreenManager::changeState(AppState newState) {
    if (screens.find(newState) != screens.end()) {
        currentState = newState;
        std::cout << "Changed to state: " << newState << std::endl;
    } else {
        std::cout << "Warning: Screen not registered for state " << newState << std::endl;
    }
}

void ScreenManager::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) {
    auto it = screens.find(currentState);
    if (it != screens.end() && it->second) {
        it->second->handleEvent(event, mousePos);
    } else {
        std::cout << "[ScreenManager] WARNING: No screen found for currentState: " << currentState << std::endl;
    }
}

void ScreenManager::update(float deltaTime) {
    auto it = screens.find(currentState);
    if (it != screens.end() && it->second) {
        it->second->update(deltaTime);
    }
}

void ScreenManager::draw(sf::RenderWindow& window) {
    auto it = screens.find(currentState);
    if (it != screens.end() && it->second) {
        it->second->draw(window);
    } else {
        std::cout << "[ScreenManager] WARNING: Cannot draw, no screen for state: " << currentState << std::endl;
    }
}

void ScreenManager::resetUserData() {
    std::cout << "[ScreenManager] Resetting UserData (logout)\n";
    userData = UserData();
}

void ScreenManager::quitApplication() {
    std::cout << "[ScreenManager] Quit requested\n";
    quitRequested = true;
}

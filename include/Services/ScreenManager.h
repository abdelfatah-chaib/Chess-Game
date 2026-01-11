#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include "AppState.h"
#include "Screen.h"
#include "TextureManager.h"
#include "FontManager.h"
#include "UserData.h"
#include "SQLiteManager.h"
#include "ChessBoard.h"

class ScreenManager {
private:
    std::map<AppState, std::unique_ptr<Screen>> screens;
    AppState currentState;
    TextureManager* textureManager;
    FontManager* fontManager;
    UserData userData;
    SQLiteManager databaseManager;
    bool quitRequested = false;
    ChessBoard chessBoard;

public:
    ScreenManager(TextureManager* texMgr, FontManager* fontMgr);
    ~ScreenManager() = default;

    void registerScreen(AppState state, std::unique_ptr<Screen> screen);
    void changeState(AppState newState);
    
    void handleEvent(const sf::Event& event, const sf::Vector2i& mousePos);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    // Database
    void initDatabase(const std::string& dbPath);

    // User management
    void resetUserData();
    UserData& getUserData() { return userData; }

    // Quit app
    void quitApplication();
    bool shouldQuit() const { return quitRequested; }

    AppState getCurrentState() const { return currentState; }
    TextureManager* getTextureManager() { return textureManager; }
    FontManager* getFontManager() { return fontManager; }
    SQLiteManager& getDatabaseManager() { return databaseManager; }
    ChessBoard* getChessBoard() { return &chessBoard; }
    Screen* getScreen(AppState state) {
        auto it = screens.find(state);
        if (it != screens.end())
            return it->second.get();
        return nullptr;
    }
};

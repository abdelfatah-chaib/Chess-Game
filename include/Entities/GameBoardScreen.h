#pragma once
#include "Screen.h"
#include "Button.h"
#include "GameController.h"
#include "BoardTheme.h"
#include <SFML/Graphics.hpp>

class GameBoardScreen : public Screen {
private:
    const float WINDOW_W = 900.f;
    const float WINDOW_H = 520.f;

    GameController* gameController;
    ThemeColors currentTheme;
    
    sf::Sprite background;
    
    // Player panels
    sf::RectangleShape whitePlayerPanel;
    sf::RectangleShape blackPlayerPanel;
    sf::Sprite whitePawnIcon;
    sf::Sprite blackPawnIcon;
    sf::Text whitePointsText;
    sf::Text blackPointsText;
    sf::Text whitePlayerTitle;
    sf::Text blackPlayerTitle;
    
    // Control buttons
    Button btnRestart;
    Button btnUndo;
    Button btnBack;
    
    // Turn display
    sf::Text turnText;
    
    // AI status display
    sf::Text aiStatusText;
    
    // Game end display
    sf::Text gameEndText;
    sf::RectangleShape gameEndOverlay;
    
    // Board initialized flag
    bool boardInitialized;

public:
    GameBoardScreen(ScreenManager* manager, GameController* gc);
    
    void handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    AppState getStateType() const override { return STATE_GAME_BOARD; }
    
private:
    void initializeBoard();
    void configureAI();
    void handleBoardClick(int row, int col);
    void drawBoard(sf::RenderWindow& window);
    void updateTurnIndicator();
    void updateScoreDisplay();
    void updateAIStatus();
    void updateBoardScale(sf::RenderWindow& window);
    void drawGameEndOverlay(sf::RenderWindow& window);
    void saveGameToDatabase();
    void resetScreen();  // Reset screen state when returning from menu
};

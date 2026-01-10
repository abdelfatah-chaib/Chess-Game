#pragma once
#include "Screen.h"
#include "Button.h"
#include "SQLiteManager.h"
#include <SFML/Graphics.hpp>
#include <vector>

class StatisticsScreen : public Screen {
private:
    sf::Sprite background;
    sf::Text title;
    sf::Text statsText;
    sf::Text historyTitle;
    
    Button btnBack;
    
    // Stats display
    int totalGames;
    int wins;
    int losses;
    int draws;
    float winRate;
    
    // History entries
    std::vector<GameHistoryEntry> gameHistory;
    std::vector<sf::Text> historyTexts;
    
    // Scroll position for history
    int scrollOffset;
    
public:
    StatisticsScreen(ScreenManager* manager);
    
    void handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    AppState getStateType() const override { return STATE_STATISTICS; }
    
private:
    void loadPlayerStats();
    void loadGameHistory();
    void updateStatsDisplay();
    void updateHistoryDisplay();
};

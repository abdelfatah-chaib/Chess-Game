#pragma once
#include "Screen.h"
#include "Button.h"
#include "Sidebar.h"
#include "Card.h"
#include "BoardTheme.h"
#include <SFML/Graphics.hpp>
#include <vector>

class HomeScreen : public Screen {
private:
    // Layout components
    Sidebar sidebar;
    
    // Scrolling
    float scrollOffset;
    float maxScrollOffset;
    
    // Header
    sf::Text welcomeText;
    sf::Text readyText;
    sf::Text onlinePlayersText;
    Button btnUpgradePro;
    sf::RectangleShape userAvatar;
    sf::Text userName;
    sf::Text userRating;
    
    // Quick Pair section
    sf::RectangleShape quickPairCard;
    sf::Sprite quickPairImage;
    sf::Text quickPairTitle;
    sf::Text quickPairSubtitle;
    sf::Text quickPairDesc;
    Button btnPlayNow;
    sf::RectangleShape timeControlDropdown;
    sf::Text timeControlText;
    
    // Game Modes section
    sf::Text gameModesTitle;
    std::vector<Card> gameModeCards;
    
    // Theme selector section
    sf::Text themeSelectorTitle;
    std::vector<Card> themeCards;
    BoardTheme selectedTheme;
    
    // Your Stats section
    sf::RectangleShape statsCard;
    sf::Text statsTitle;
    sf::Text ratingText;
    sf::Text ratingLabel;
    sf::RectangleShape progressBar;
    sf::RectangleShape progressFill;
    
    // Recent Matches section
    sf::RectangleShape matchesCard;
    sf::Text matchesTitle;
    Button btnViewAll;
    
    struct MatchItem {
        sf::RectangleShape background;
        sf::Text resultBadge;
        sf::Text opponentName;
        sf::Text matchInfo;
        sf::Text date;
    };
    std::vector<MatchItem> recentMatches;

public:
    HomeScreen(ScreenManager* manager);
    
    void handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    AppState getStateType() const override { return STATE_HOME; }
    
private:
    void createGameModeCards(sf::Font& font);
    void createRecentMatches(sf::Font& font);
    void createThemeCards(sf::Font& font);
    void updateMaxScroll(float windowHeight);
};

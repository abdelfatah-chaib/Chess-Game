#pragma once
#include "Screen.h"
#include "Button.h"
#include "BoardTheme.h"
#include "PieceSetType.h"
#include <SFML/Graphics.hpp>

class MainMenuScreen : public Screen {
private:
    const float WINDOW_W = 900.f;
    const float WINDOW_H = 520.f;
    sf::Vector2f currentWindowSize{WINDOW_W, WINDOW_H};

    sf::Sprite background;
    sf::Text welcomeText;
    sf::Text chessGameSetupText;
    sf::Text levelText;

    // Main container (Play as + Difficulty) - with rounded corners effect
    sf::RectangleShape mainContainer;
    sf::RectangleShape containerShadow;
    sf::RectangleShape containerGlow;

    // Play as section
    sf::RectangleShape playAsSection;
    sf::Text playAsTitle;
    
    // Icon buttons for White/Black king selection
    Button btnWhiteKing;
    Button btnBlackKing;
    sf::Sprite whiteKingIcon;
    sf::Sprite blackKingIcon;
    sf::RectangleShape whiteKingFrame;
    sf::RectangleShape blackKingFrame;

    // Difficulty section
    sf::RectangleShape difficultySection;
    sf::Text difficultyTitle;
    
    // Modern difficulty buttons (Easy/Medium/Hard)
    Button btnEasy;
    Button btnMedium;
    Button btnHard;
    
    // Piece Set Selection section
    sf::RectangleShape pieceSetContainer;
    sf::RectangleShape pieceSetContainerShadow;
    sf::Text pieceSetTitle;
    sf::Text pieceSetSubtitle;
    
    struct PieceSetCard {
        sf::RectangleShape frame;
        sf::RectangleShape border;
        sf::Text label;
        sf::Sprite previewSprites[3]; // King, Queen, Pawn previews
        sf::RectangleShape previewSlots[3];
        sf::Text previewGlyphs[3];
        sf::FloatRect clickArea;
        sf::RectangleShape selectionGlow;
    };
    PieceSetCard pieceSetCards[2]; // Classic and Themed
    PieceSetType selectedPieceSet;
    
    // Board Theme section (SEPARATE container below main)
    sf::RectangleShape themeContainer;
    sf::RectangleShape themeContainerShadow;
    sf::Text themeTitle;
    sf::Text themeSubtitle;
    
    // Mini board previews for each theme
    struct MiniBoard {
        sf::RectangleShape frame;
        sf::RectangleShape border;
        sf::RectangleShape squares[8][8];
        sf::Text label;
        sf::FloatRect clickArea;
        sf::RectangleShape selectionGlow;
        sf::Sprite pieceSprites[8]; // Sprites pour afficher les pièces sur le board
    };
    MiniBoard themePreviews[3];
    BoardTheme selectedTheme;

    // Play button
    Button btnPlayNow;
    Button btnStatistics;
    Button btnLogout;

    // Animation variables
    float playButtonPulseTime;
    float playButtonPulseDirection;

    // State
    bool whiteSelected;
    bool blackSelected;
    int selectedDifficulty; // 1=Easy, 2=Medium, 3=Hard

public:
    MainMenuScreen(ScreenManager* manager);

    void handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    AppState getStateType() const override { return STATE_MAIN_MENU; }

private:
    void applyResponsiveLayout(const sf::Vector2u& windowSize);
    void updateVisualFeedback();
    bool checkThemeClicks(const sf::Vector2i& mousePos);
    bool checkPieceSetClicks(const sf::Vector2i& mousePos);
    void createMiniBoards(sf::Font& font, const sf::Vector2f& windowSize);
    void createPieceSetCards(sf::Font& font, const sf::Vector2f& windowSize);
    void loadPieceSetPreviews();
};

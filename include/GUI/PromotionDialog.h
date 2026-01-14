#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>

/**
 * @brief UI Dialog for selecting which piece to promote a pawn to
 * 
 * This dialog appears when a pawn reaches the promotion row and allows
 * the player to choose between Queen, Rook, Bishop, or Knight.
 */
class PromotionDialog {
private:
    // Dialog state
    bool visible;
    std::string pawnColor;
    int promotionRow;
    int promotionCol;
    
    // Visual components
    sf::RectangleShape overlay;
    sf::RectangleShape dialogBackground;
    sf::Text title;
    sf::Text instruction;
    
    // Piece selection buttons
    struct PieceOption {
        std::string pieceType;
        sf::RectangleShape background;
        sf::Sprite sprite;
        sf::Text label;
        bool hovered;
        
        PieceOption() : hovered(false) {}
    };
    
    std::vector<PieceOption> pieceOptions;
    
    // Callback when piece is selected
    std::function<void(const std::string&)> onPieceSelected;
    
    // Resources
    const sf::Font* font;
    sf::Texture queenTexture;
    sf::Texture rookTexture;
    sf::Texture bishopTexture;
    sf::Texture knightTexture;

public:
    PromotionDialog();
    
    /**
     * @brief Show the promotion dialog for a specific pawn
     * @param color Color of the pawn being promoted ("white" or "black")
     * @param row Row where promotion occurred
     * @param col Column where promotion occurred
     * @param callback Function to call when piece is selected
     * @param font Font to use for text
     */
    void show(const std::string& color, int row, int col, 
              std::function<void(const std::string&)> callback,
              const sf::Font* font);
    
    /**
     * @brief Hide the dialog
     */
    void hide();
    
    /**
     * @brief Check if dialog is currently visible
     */
    bool isVisible() const { return visible; }
    
    /**
     * @brief Handle mouse events
     * @param event SFML event
     * @param mousePos Current mouse position
     */
    void handleEvent(const sf::Event& event, const sf::Vector2i& mousePos);
    
    /**
     * @brief Update hover states
     * @param mousePos Current mouse position
     */
    void updateHover(const sf::Vector2i& mousePos);
    
    /**
     * @brief Draw the dialog
     * @param window Window to draw on
     */
    void draw(sf::RenderWindow& window);
    
private:
    /**
     * @brief Load piece textures based on color
     * @param color "white" or "black"
     */
    bool loadPieceTextures(const std::string& color);
    
    /**
     * @brief Initialize piece option buttons
     */
    void initializePieceOptions(const sf::Vector2f& windowSize);
};

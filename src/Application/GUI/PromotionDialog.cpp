#include "GUI/PromotionDialog.h"
#include "UIStyles.h"
#include "UIHelpers.h"
#include <iostream>

PromotionDialog::PromotionDialog() 
    : visible(false), pawnColor("white"), promotionRow(-1), promotionCol(-1), font(nullptr) {
    
    // Initialize overlay (semi-transparent dark background)
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    
    // Initialize dialog background
    dialogBackground.setFillColor(sf::Color::White);
    dialogBackground.setOutlineThickness(3.f);
    dialogBackground.setOutlineColor(UIStyles::Colors::Primary);
}

void PromotionDialog::show(const std::string& color, int row, int col,
                            std::function<void(const std::string&)> callback,
                            const sf::Font* dialogFont) {
    std::cout << "[PromotionDialog] Showing dialog for " << color << " pawn at (" 
              << row << "," << col << ")" << std::endl;
    
    visible = true;
    pawnColor = color;
    promotionRow = row;
    promotionCol = col;
    onPieceSelected = callback;
    font = dialogFont;
    
    // Load textures for the pieces
    if (!loadPieceTextures(color)) {
        std::cout << "[PromotionDialog] Warning: Could not load all piece textures" << std::endl;
    }
    
    // Initialize title and instruction text
    if (font) {
        title.setFont(*font);
        title.setString("Pawn Promotion");
        title.setCharacterSize(UIStyles::Typography::HeadingSize);
        title.setFillColor(UIStyles::Colors::TextPrimary);
        title.setStyle(sf::Text::Bold);
        
        instruction.setFont(*font);
        instruction.setString("Choose a piece to promote your pawn:");
        instruction.setCharacterSize(UIStyles::Typography::BodySize);
        instruction.setFillColor(UIStyles::Colors::TextSecondary);
    }
}

void PromotionDialog::hide() {
    std::cout << "[PromotionDialog] Hiding dialog" << std::endl;
    visible = false;
    pieceOptions.clear();
}

bool PromotionDialog::loadPieceTextures(const std::string& color) {
    std::string basePath = "C:/Users/abdel/OneDrive - uit.ac.ma/Bureau/M1-IAOC/Conception and Programing CPP/ChessMasterUIT-Project/ChessMasterUIT/assets/";
    std::string prefix = (color == "white") ? "w_" : "b_";
    
    bool allLoaded = true;
    
    if (!queenTexture.loadFromFile(basePath + prefix + "queen.png")) {
        std::cout << "[PromotionDialog] Failed to load queen texture" << std::endl;
        allLoaded = false;
    }
    
    if (!rookTexture.loadFromFile(basePath + prefix + "rook.png")) {
        std::cout << "[PromotionDialog] Failed to load rook texture" << std::endl;
        allLoaded = false;
    }
    
    if (!bishopTexture.loadFromFile(basePath + prefix + "bishop.png")) {
        std::cout << "[PromotionDialog] Failed to load bishop texture" << std::endl;
        allLoaded = false;
    }
    
    if (!knightTexture.loadFromFile(basePath + prefix + "knight.png")) {
        std::cout << "[PromotionDialog] Failed to load knight texture" << std::endl;
        allLoaded = false;
    }
    
    return allLoaded;
}

void PromotionDialog::initializePieceOptions(const sf::Vector2f& windowSize) {
    pieceOptions.clear();
    
    // Calculate dialog dimensions and position
    float dialogWidth = 600.f;
    float dialogHeight = 300.f;
    float dialogX = (windowSize.x - dialogWidth) / 2.f;
    float dialogY = (windowSize.y - dialogHeight) / 2.f;
    
    dialogBackground.setSize(sf::Vector2f(dialogWidth, dialogHeight));
    dialogBackground.setPosition(dialogX, dialogY);
    
    // Position title and instruction
    if (font) {
        UIHelpers::centerText(title, windowSize.x / 2.f, dialogY + 30.f);
        UIHelpers::centerText(instruction, windowSize.x / 2.f, dialogY + 70.f);
    }
    
    // Create piece options (4 buttons: Queen, Rook, Bishop, Knight)
    std::vector<std::string> pieces = {"queen", "rook", "bishop", "knight"};
    std::vector<sf::Texture*> textures = {&queenTexture, &rookTexture, &bishopTexture, &knightTexture};
    
    float buttonWidth = 120.f;
    float buttonHeight = 120.f;
    float buttonSpacing = 20.f;
    float totalWidth = (buttonWidth * 4) + (buttonSpacing * 3);
    float startX = dialogX + (dialogWidth - totalWidth) / 2.f;
    float buttonY = dialogY + 120.f;
    
    for (size_t i = 0; i < pieces.size(); ++i) {
        PieceOption option;
        option.pieceType = pieces[i];
        
        // Background
        option.background.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        option.background.setPosition(startX + i * (buttonWidth + buttonSpacing), buttonY);
        option.background.setFillColor(UIStyles::Colors::Background);
        option.background.setOutlineThickness(2.f);
        option.background.setOutlineColor(UIStyles::Colors::BorderDefault);
        
        // Sprite
        option.sprite.setTexture(*textures[i]);
        sf::Vector2u texSize = textures[i]->getSize();
        float scale = (buttonWidth * 0.7f) / std::max(texSize.x, texSize.y);
        option.sprite.setScale(scale, scale);
        
        sf::FloatRect bounds = option.sprite.getLocalBounds();
        option.sprite.setOrigin(bounds.width / 2, bounds.height / 2);
        option.sprite.setPosition(
            startX + i * (buttonWidth + buttonSpacing) + buttonWidth / 2,
            buttonY + buttonHeight * 0.5f
        );
        
        // Label
        if (font) {
            option.label.setFont(*font);
            option.label.setString(pieces[i]);
            option.label.setCharacterSize(UIStyles::Typography::CaptionSize);
            option.label.setFillColor(UIStyles::Colors::TextPrimary);
            UIHelpers::centerText(option.label, 
                                  startX + i * (buttonWidth + buttonSpacing) + buttonWidth / 2,
                                  buttonY + buttonHeight + 15.f);
        }
        
        pieceOptions.push_back(option);
    }
}

void PromotionDialog::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) {
    if (!visible) return;
    
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        
        // Check if any piece option was clicked
        for (const auto& option : pieceOptions) {
            if (option.background.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                std::cout << "[PromotionDialog] Selected: " << option.pieceType << std::endl;
                
                // Call callback with selected piece
                if (onPieceSelected) {
                    onPieceSelected(option.pieceType);
                }
                
                hide();
                return;
            }
        }
    }
}

void PromotionDialog::updateHover(const sf::Vector2i& mousePos) {
    if (!visible) return;
    
    for (auto& option : pieceOptions) {
        bool wasHovered = option.hovered;
        option.hovered = option.background.getGlobalBounds().contains(mousePos.x, mousePos.y);
        
        // Update visual feedback
        if (option.hovered) {
            option.background.setFillColor(UIStyles::Colors::PrimaryHover);
            option.background.setOutlineColor(UIStyles::Colors::Primary);
            option.background.setOutlineThickness(3.f);
        } else {
            option.background.setFillColor(UIStyles::Colors::Background);
            option.background.setOutlineColor(UIStyles::Colors::BorderDefault);
            option.background.setOutlineThickness(2.f);
        }
    }
}

void PromotionDialog::draw(sf::RenderWindow& window) {
    if (!visible) return;
    
    sf::Vector2u winSize = window.getSize();
    sf::Vector2f winSizeF(static_cast<float>(winSize.x), static_cast<float>(winSize.y));
    
    // Initialize piece options if not done yet
    if (pieceOptions.empty()) {
        initializePieceOptions(winSizeF);
    }
    
    // Draw overlay
    overlay.setSize(winSizeF);
    window.draw(overlay);
    
    // Draw dialog background
    window.draw(dialogBackground);
    
    // Draw title and instruction
    if (font) {
        window.draw(title);
        window.draw(instruction);
    }
    
    // Draw piece options
    for (const auto& option : pieceOptions) {
        window.draw(option.background);
        window.draw(option.sprite);
        window.draw(option.label);
    }
}

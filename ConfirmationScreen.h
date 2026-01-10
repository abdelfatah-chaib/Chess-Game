#pragma once
#include "Screen.h"
#include "Button.h"
#include <SFML/Graphics.hpp>

class ConfirmationScreen : public Screen {
private:
    // Overlay and modal
    sf::RectangleShape overlay;
    sf::RectangleShape modalBackground;
    
    // Success icon (animated circles + checkmark)
    sf::CircleShape iconOuterCircle;
    sf::CircleShape iconMiddleCircle;
    sf::CircleShape iconInnerCircle;
    sf::Text checkmark;
    
    // Content
    sf::Text title;
    sf::Text message;
    Button btnEnterLobby;
    Button btnViewProfile;
    
    // Footer links
    sf::Text footerLinks;
    sf::Text copyright;
    
    // Animation
    float animationTime;
    bool animationComplete;

public:
    ConfirmationScreen(ScreenManager* manager);

    void handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    AppState getStateType() const override { return STATE_CONFIRMATION; }
};

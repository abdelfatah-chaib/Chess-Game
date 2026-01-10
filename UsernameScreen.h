#pragma once
#include "Screen.h"
#include "Button.h"
#include "InputField.h"
#include <SFML/Graphics.hpp>

class UsernameScreen : public Screen {
private:
    // Split-screen layout
    sf::Sprite leftSideImage;
    sf::RectangleShape leftBackground;
    sf::RectangleShape rightBackground;
    
    // Left side content
    sf::Text leftTitle;
    sf::Text leftDescription;
    sf::RectangleShape badge;
    sf::Text badgeText;
    
    // Right side content
    sf::RectangleShape progressIndicator1;
    sf::RectangleShape progressIndicator2;
    sf::RectangleShape progressIndicator3;
    sf::Text title;
    sf::Text subtitle;
    sf::Text infoText;
    sf::Text infoDescription;
    
    InputField usernameField;
    Button btnContinue;
    Button btnContinueAsGuest;
    
    InputField* activeInputField;

public:
    UsernameScreen(ScreenManager* manager);
    
    void handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    AppState getStateType() const override { return STATE_USERNAME; }
};

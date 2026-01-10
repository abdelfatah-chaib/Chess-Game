#pragma once
#include "Screen.h"
#include "Button.h"
#include "InputField.h"
#include <SFML/Graphics.hpp>

class EmailPasswordScreen : public Screen {
private:
    sf::Sprite background;
    sf::Text titleText;
    sf::Text subtitleText;
    InputField emailField;
    InputField passwordField;
    Button btnContinue;
    
    InputField* activeInputField;

public:
    EmailPasswordScreen(ScreenManager* manager);
    
    void handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    AppState getStateType() const override { return STATE_EMAIL_PASSWORD; }
};

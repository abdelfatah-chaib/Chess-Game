#pragma once
#include "Screen.h"
#include "Button.h"
#include "InputField.h"
#include <SFML/Graphics.hpp>

class CreateAccountScreen : public Screen {
private:
    sf::Sprite backgroundSprite;
    sf::RectangleShape cardBackground;
    sf::Sprite headerImage;
    sf::Text title;
    sf::Text subtitle;
    sf::Text dividerText;
    sf::Text loginText;
    
    InputField usernameField;
    InputField emailField;
    InputField passwordField;
    InputField confirmPasswordField;
    
    Button btnCreateAccount;
    Button btnGoogle;
    Button btnApple;
    Button btnLogin;

public:
    CreateAccountScreen(ScreenManager* manager);
    
    void handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    AppState getStateType() const override { return STATE_CREATE_ACCOUNT; }
    
private:
    void attemptCreateAccount();
};

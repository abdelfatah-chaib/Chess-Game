#pragma once
#include "Screen.h"
#include "Button.h"
#include "InputField.h"
#include "Checkbox.h"
#include <SFML/Graphics.hpp>

class LoginScreen : public Screen {
private:
    sf::Sprite backgroundSprite;
    sf::RectangleShape cardBackground;
    sf::Text title;
    sf::Text subtitle;
    sf::Text errorMessage;
    sf::Text dividerText;
    sf::Text signupText;
    
    InputField emailField;
    InputField passwordField;
    
    Button btnLogin;
    Button btnGoogle;
    Button btnApple;
    Button btnForgotPassword;
    Button btnSignUp;
    
    Checkbox chkStayLoggedIn;
    
    bool showError;
    float errorTimer;

public:
    LoginScreen(ScreenManager* manager);
    
    void handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    AppState getStateType() const override { return STATE_LOGIN; }
    
private:
    void attemptLogin();
    void showErrorMessage(const std::string& msg);
};

#pragma once
#include "Screen.h"
#include "Button.h"
#include "InputField.h"
#include <SFML/Graphics.hpp>

class ForgotPasswordScreen : public Screen {
private:
    sf::Sprite background;
    sf::Text title;
    sf::Text instructions;
    sf::Text successMessage;
    sf::Text errorMessage;
    
    InputField emailField;
    InputField newPasswordField;
    InputField confirmPasswordField;
    
    Button btnResetPassword;
    Button btnBack;
    
    bool showSuccess;
    bool showError;
    float messageTimer;

public:
    ForgotPasswordScreen(ScreenManager* manager);
    
    void handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    AppState getStateType() const override { return STATE_FORGOT_PASSWORD; }
    
private:
    void attemptPasswordReset();
    void showSuccessMessage(const std::string& msg);
    void showErrorMessage(const std::string& msg);
};

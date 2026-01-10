#include "EmailPasswordScreen.h"
#include "UIStyles.h"
#include "ScreenManager.h"
#include "UIStyles.h"
#include "UIHelpers.h"
#include "UIStyles.h"
#include <iostream>

EmailPasswordScreen::EmailPasswordScreen(ScreenManager* manager) 
    : Screen(manager), activeInputField(nullptr) {
    
    // Setup background
    sf::Texture* bgTexture = manager->getTextureManager()->getTexture("bg_email");
    if (bgTexture) {
        background.setTexture(*bgTexture);
    }
    
    // Load modern fonts
    sf::Font* titleFont = manager->getFontManager()->getFont(FontType::CINZEL_REGULAR);
    sf::Font* bodyFont = manager->getFontManager()->getFont(FontType::INTER_REGULAR);
    sf::Font* buttonFont = manager->getFontManager()->getFont(FontType::INTER_SEMIBOLD);
    
    // Fallback
    if (!titleFont) titleFont = manager->getFontManager()->getFont(FontType::INTER_REGULAR);
    if (!bodyFont) bodyFont = titleFont;
    if (!buttonFont) buttonFont = bodyFont;
    
    // Title - Cinzel Regular
    titleText.setFont(*titleFont);
    titleText.setString("Enter your Email and a Password");
    titleText.setCharacterSize(UIStyles::Typography::SubtitleSize);
    titleText.setFillColor(sf::Color::White);
    
    // Subtitle - Inter Regular
    subtitleText.setFont(*bodyFont);
    subtitleText.setString("This allows you to log in on any device");
    subtitleText.setCharacterSize(UIStyles::Typography::BodySize);
    subtitleText.setFillColor(sf::Color::White);
    
    // Input fields - Inter Regular
    emailField = InputField(
        sf::Vector2f(0, 0),
        sf::Vector2f(400.f, 50.f),
        "Email",
        *bodyFont
    );
    
    passwordField = InputField(
        sf::Vector2f(0, 0),
        sf::Vector2f(400.f, 50.f),
        "Password",
        *bodyFont
    );
    
    // Continue button - Inter SemiBold
    btnContinue = Button(
        sf::Vector2f(0, 0),
        sf::Vector2f(240.f, 50.f),
        "Continue",
        *buttonFont,
        UIStyles::Typography::BodySize
    );
}

void EmailPasswordScreen::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) {
    if (event.type == sf::Event::TextEntered) {
        if (activeInputField) {
            activeInputField->handleTextInput(event.text.unicode);
        }
    }
    
    if (event.type == sf::Event::MouseButtonPressed && 
        event.mouseButton.button == sf::Mouse::Left) {
        
        if (emailField.clicked(mousePos)) {
            activeInputField = &emailField;
        } else if (passwordField.clicked(mousePos)) {
            activeInputField = &passwordField;
        } else {
            activeInputField = nullptr;
        }
        
        if (btnContinue.clicked(mousePos)) {
            std::cout << "[EmailPasswordScreen] Continue button clicked" << std::endl;
            std::cout << "[EmailPasswordScreen] Email: " << emailField.getContent() << std::endl;
            std::cout << "[EmailPasswordScreen] Password length: " << passwordField.getContent().length() << std::endl;
            
            bool emailValid = emailField.validateEmail();
            bool passwordValid = !passwordField.getContent().empty();
            
            std::cout << "[EmailPasswordScreen] Email valid: " << (emailValid ? "YES" : "NO") << std::endl;
            std::cout << "[EmailPasswordScreen] Password valid: " << (passwordValid ? "YES" : "NO") << std::endl;
            
            if (emailValid && passwordValid) {
                // Save email and password to UserData
                screenManager->getUserData().setEmail(emailField.getContent());
                screenManager->getUserData().setPassword(passwordField.getContent());
                
                std::cout << "[EmailPasswordScreen] ✓ Data saved to UserData, moving to USERNAME screen" << std::endl;
                screenManager->changeState(STATE_USERNAME);
            } else {
                std::cout << "[EmailPasswordScreen] ✗ Validation failed" << std::endl;
            }
        }
    }
}

void EmailPasswordScreen::update(float deltaTime) {
    // No specific update logic needed
}

void EmailPasswordScreen::draw(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f winSizeF((float)windowSize.x, (float)windowSize.y);
    
    // Scale background to fit window
    if (background.getTexture()) {
        UIHelpers::setupBackground(background, *background.getTexture(), true, window);
    }
    window.draw(background);
    
    // Scale and position title
    float scaledFontSize36 = UIHelpers::scaleFont(36.f, winSizeF);
    titleText.setCharacterSize((unsigned int)scaledFontSize36);
    UIHelpers::centerText(titleText, winSizeF.x / 2.f, UIHelpers::scaleFont(80.f, winSizeF));
    window.draw(titleText);
    
    // Scale and position subtitle
    float scaledFontSize20 = UIHelpers::scaleFont(20.f, winSizeF);
    subtitleText.setCharacterSize((unsigned int)scaledFontSize20);
    UIHelpers::centerText(subtitleText, winSizeF.x / 2.f, UIHelpers::scaleFont(130.f, winSizeF));
    window.draw(subtitleText);
    
    // Scale and position input fields
    sf::Vector2f fieldSize = UIHelpers::scaleSize(sf::Vector2f(400.f, 50.f), winSizeF);
    sf::Vector2f emailPos = sf::Vector2f((winSizeF.x - fieldSize.x) / 2.f, UIHelpers::scaleFont(200.f, winSizeF));
    sf::Vector2f passwordPos = sf::Vector2f((winSizeF.x - fieldSize.x) / 2.f, UIHelpers::scaleFont(280.f, winSizeF));
    
    // Update field positions and sizes
    emailField.setPosition(emailPos);
    emailField.setSize(fieldSize);
    passwordField.setPosition(passwordPos);
    passwordField.setSize(fieldSize);
    
    emailField.draw(window);
    passwordField.draw(window);
    
    // Scale and position button
    sf::Vector2f btnSize = UIHelpers::scaleSize(sf::Vector2f(240.f, 50.f), winSizeF);
    sf::Vector2f btnPos = sf::Vector2f((winSizeF.x - btnSize.x) / 2.f, UIHelpers::scaleFont(370.f, winSizeF));
    
    btnContinue.setPosition(btnPos);
    btnContinue.getRect().setSize(btnSize);
    btnContinue.getText().setCharacterSize((unsigned int)UIHelpers::scaleFont(28.f, winSizeF));
    UIHelpers::centerText(btnContinue.getText(), btnPos.x + btnSize.x / 2.f, btnPos.y + btnSize.y / 2.f);
    
    btnContinue.draw(window);
}

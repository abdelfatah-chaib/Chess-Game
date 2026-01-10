#include "ForgotPasswordScreen.h"
#include "UIStyles.h"
#include "ScreenManager.h"
#include "UIStyles.h"
#include "UIHelpers.h"
#include "UIStyles.h"
#include <iostream>

ForgotPasswordScreen::ForgotPasswordScreen(ScreenManager* manager)
    : Screen(manager), showSuccess(false), showError(false), messageTimer(0.0f) {
    
    // Setup background
    sf::Texture* bgTexture = manager->getTextureManager()->getTexture("bg_create");
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
    title.setFont(*titleFont);
    title.setString("Reset Password");
    title.setCharacterSize(UIStyles::Typography::SubtitleSize);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);

    // Instructions - Inter Regular
    instructions.setFont(*bodyFont);
    instructions.setString("Enter your email and a new password");
    instructions.setCharacterSize(UIStyles::Typography::BodySize);
    instructions.setFillColor(sf::Color(200, 200, 200));

    // Success message - Inter Regular
    successMessage.setFont(*bodyFont);
    successMessage.setString("");
    successMessage.setCharacterSize(UIStyles::Typography::BodySize);
    successMessage.setFillColor(UIStyles::Colors::TextSuccess);
    successMessage.setStyle(sf::Text::Bold);

    // Error message - Inter Regular
    errorMessage.setFont(*bodyFont);
    errorMessage.setString("");
    errorMessage.setCharacterSize(UIStyles::Typography::BodySize);
    errorMessage.setFillColor(UIStyles::Colors::TextError);
    errorMessage.setStyle(sf::Text::Bold);

    // Input fields - Inter Regular
    emailField = InputField(sf::Vector2f(400.f, 220.f), sf::Vector2f(400.f, UIStyles::Sizes::InputHeight), "Email", *bodyFont);
    newPasswordField = InputField(sf::Vector2f(400.f, 290.f), sf::Vector2f(400.f, UIStyles::Sizes::InputHeight), "New Password", *bodyFont);
    confirmPasswordField = InputField(sf::Vector2f(400.f, 360.f), sf::Vector2f(400.f, UIStyles::Sizes::InputHeight), "Confirm Password", *bodyFont);

    // Reset button - Inter SemiBold
    btnResetPassword = Button(
        sf::Vector2f(400.f, 430.f),
        sf::Vector2f(200.f, UIStyles::Sizes::ButtonHeight),
        "Reset Password",
        *buttonFont,
        UIStyles::Typography::BodySize
    );
    btnResetPassword.setStyle(ButtonStyle::Primary);

    // Back button - Inter SemiBold
    btnBack = Button(
        sf::Vector2f(50.f, 50.f),
        sf::Vector2f(120.f, UIStyles::Sizes::ButtonHeight),
        "Back",
        *buttonFont,
        UIStyles::Typography::BodySize
    );
}

void ForgotPasswordScreen::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) {
    // Handle text input
    if (event.type == sf::Event::TextEntered) {
        if (emailField.getIsActive()) {
            emailField.handleTextInput(event.text.unicode);
        } else if (newPasswordField.getIsActive()) {
            newPasswordField.handleTextInput(event.text.unicode);
        } else if (confirmPasswordField.getIsActive()) {
            confirmPasswordField.handleTextInput(event.text.unicode);
        }
    }
    
    btnResetPassword.updateHover(mousePos);
    btnBack.updateHover(mousePos);

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        // Check input field clicks
        if (emailField.clicked(mousePos)) {
            emailField.setActive(true);
            newPasswordField.setActive(false);
            confirmPasswordField.setActive(false);
        } else if (newPasswordField.clicked(mousePos)) {
            emailField.setActive(false);
            newPasswordField.setActive(true);
            confirmPasswordField.setActive(false);
        } else if (confirmPasswordField.clicked(mousePos)) {
            emailField.setActive(false);
            newPasswordField.setActive(false);
            confirmPasswordField.setActive(true);
        } else {
            emailField.setActive(false);
            newPasswordField.setActive(false);
            confirmPasswordField.setActive(false);
        }

        if (btnResetPassword.clicked(mousePos)) {
            attemptPasswordReset();
        }
        else if (btnBack.clicked(mousePos)) {
            screenManager->changeState(STATE_LOGIN);
        }
    }

    // Enter key to reset
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
        attemptPasswordReset();
    }
}

void ForgotPasswordScreen::update(float deltaTime) {
    // Hide messages after 5 seconds
    if (showSuccess || showError) {
        messageTimer += deltaTime;
        if (messageTimer >= 5.0f) {
            showSuccess = false;
            showError = false;
            messageTimer = 0.0f;
            
            // If success, go back to login after message disappears
            if (showSuccess) {
                screenManager->changeState(STATE_LOGIN);
            }
        }
    }
}

void ForgotPasswordScreen::draw(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f winSizeF(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y));
    
    // Scale background to fit window
    if (background.getTexture()) {
        UIHelpers::setupBackground(background, *background.getTexture(), true, window);
    }
    window.draw(background);

    // Scale and center title
    title.setCharacterSize(static_cast<unsigned int>(UIHelpers::scaleFont(48.f, winSizeF)));
    UIHelpers::centerText(title, winSizeF.x / 2.f, UIHelpers::scaleFont(80.f, winSizeF));
    window.draw(title);

    // Instructions
    instructions.setCharacterSize(static_cast<unsigned int>(UIHelpers::scaleFont(20.f, winSizeF)));
    UIHelpers::centerText(instructions, winSizeF.x / 2.f, UIHelpers::scaleFont(140.f, winSizeF));
    window.draw(instructions);

    // Scale and position email field
    sf::Vector2f fieldSize = UIHelpers::scaleSize(sf::Vector2f(400.f, 50.f), winSizeF);
    sf::Vector2f emailFieldPos((winSizeF.x - fieldSize.x) / 2.f, UIHelpers::scaleFont(220.f, winSizeF));
    emailField.setPosition(emailFieldPos);
    emailField.setSize(fieldSize);
    emailField.draw(window);

    // New password field
    sf::Vector2f newPasswordFieldPos((winSizeF.x - fieldSize.x) / 2.f, UIHelpers::scaleFont(290.f, winSizeF));
    newPasswordField.setPosition(newPasswordFieldPos);
    newPasswordField.setSize(fieldSize);
    newPasswordField.draw(window);

    // Confirm password field
    sf::Vector2f confirmPasswordFieldPos((winSizeF.x - fieldSize.x) / 2.f, UIHelpers::scaleFont(360.f, winSizeF));
    confirmPasswordField.setPosition(confirmPasswordFieldPos);
    confirmPasswordField.setSize(fieldSize);
    confirmPasswordField.draw(window);

    // Reset button
    sf::Vector2f btnSize = UIHelpers::scaleSize(sf::Vector2f(200.f, 50.f), winSizeF);
    sf::Vector2f btnPos((winSizeF.x - btnSize.x) / 2.f, UIHelpers::scaleFont(430.f, winSizeF));
    btnResetPassword.setPosition(btnPos);
    btnResetPassword.getRect().setSize(btnSize);
    btnResetPassword.getText().setCharacterSize(static_cast<unsigned int>(UIHelpers::scaleFont(22.f, winSizeF)));
    UIHelpers::centerText(btnResetPassword.getText(), btnPos.x + btnSize.x / 2.f, btnPos.y + btnSize.y / 2.f);
    btnResetPassword.draw(window);

    // Back button
    sf::Vector2f backBtnSize = UIHelpers::scaleSize(sf::Vector2f(120.f, 50.f), winSizeF);
    sf::Vector2f backBtnPos(UIHelpers::scaleFont(50.f, winSizeF), UIHelpers::scaleFont(50.f, winSizeF));
    btnBack.setPosition(backBtnPos);
    btnBack.getRect().setSize(backBtnSize);
    btnBack.getText().setCharacterSize(static_cast<unsigned int>(UIHelpers::scaleFont(24.f, winSizeF)));
    UIHelpers::centerText(btnBack.getText(), backBtnPos.x + backBtnSize.x / 2.f, backBtnPos.y + backBtnSize.y / 2.f);
    btnBack.draw(window);

    // Success message
    if (showSuccess) {
        successMessage.setCharacterSize(static_cast<unsigned int>(UIHelpers::scaleFont(18.f, winSizeF)));
        UIHelpers::centerText(successMessage, winSizeF.x / 2.f, UIHelpers::scaleFont(510.f, winSizeF));
        window.draw(successMessage);
    }

    // Error message
    if (showError) {
        errorMessage.setCharacterSize(static_cast<unsigned int>(UIHelpers::scaleFont(18.f, winSizeF)));
        UIHelpers::centerText(errorMessage, winSizeF.x / 2.f, UIHelpers::scaleFont(510.f, winSizeF));
        window.draw(errorMessage);
    }
}

void ForgotPasswordScreen::attemptPasswordReset() {
    std::string email = emailField.getContent();
    std::string newPassword = newPasswordField.getContent();
    std::string confirmPassword = confirmPasswordField.getContent();

    std::cout << "[ForgotPasswordScreen] Attempting password reset for: " << email << std::endl;

    // Validate inputs
    if (email.empty() || newPassword.empty() || confirmPassword.empty()) {
        showErrorMessage("Please fill in all fields");
        return;
    }

    if (!emailField.validateEmail()) {
        showErrorMessage("Invalid email address");
        return;
    }

    if (newPassword.length() < 6) {
        showErrorMessage("Password must be at least 6 characters");
        return;
    }

    if (newPassword != confirmPassword) {
        showErrorMessage("Passwords do not match");
        return;
    }

    SQLiteManager& db = screenManager->getDatabaseManager();

    // Check if email exists
    if (!db.emailExists(email)) {
        showErrorMessage("Email not found");
        return;
    }

    // Reset password
    if (db.resetPassword(email, newPassword)) {
        std::cout << "[ForgotPasswordScreen] Password reset successful!" << std::endl;
        showSuccessMessage("Password reset successful! Redirecting to login...");
        
        // Clear fields
        emailField.setActive(false);
        newPasswordField.setActive(false);
        confirmPasswordField.setActive(false);
    } else {
        std::cout << "[ForgotPasswordScreen] Password reset failed" << std::endl;
        showErrorMessage("Failed to reset password. Please try again.");
    }
}

void ForgotPasswordScreen::showSuccessMessage(const std::string& msg) {
    successMessage.setString(msg);
    showSuccess = true;
    showError = false;
    messageTimer = 0.0f;
}

void ForgotPasswordScreen::showErrorMessage(const std::string& msg) {
    errorMessage.setString(msg);
    showError = true;
    showSuccess = false;
    messageTimer = 0.0f;
}

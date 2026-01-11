#include "LoginScreen.h"
#include "Services/ScreenManager.h"
#include "UIHelpers.h"
#include "UIStyles.h"
#include <iostream>

LoginScreen::LoginScreen(ScreenManager* manager) 
    : Screen(manager), showError(false), errorTimer(0.0f) {
    
    // Load modern fonts
    sf::Font* titleFont = manager->getFontManager()->getFont(FontType::CINZEL_REGULAR);
    sf::Font* bodyFont = manager->getFontManager()->getFont(FontType::INTER_REGULAR);
    sf::Font* buttonFont = manager->getFontManager()->getFont(FontType::INTER_SEMIBOLD);
    
    // Fallback
    if (!titleFont) titleFont = manager->getFontManager()->getFont(FontType::INTER_REGULAR);
    if (!bodyFont) bodyFont = titleFont;
    if (!buttonFont) buttonFont = bodyFont;
    
    // Load background image
    sf::Texture* bgTexture = manager->getTextureManager()->getTexture("bgg");
    if (bgTexture) {
        backgroundSprite.setTexture(*bgTexture);
    }

    // Title - "Welcome Back" - Cinzel for elegance
    title.setFont(*titleFont);
    title.setString("Welcome Back");
    title.setCharacterSize(UIStyles::Typography::TitleSize);
    title.setFillColor(UIStyles::Colors::TextPrimary);
    title.setStyle(sf::Text::Bold);

    // Subtitle - Inter Regular
    subtitle.setFont(*bodyFont);
    subtitle.setString("Enter your credentials to resume your game.");
    subtitle.setCharacterSize(UIStyles::Typography::CaptionSize);
    subtitle.setFillColor(UIStyles::Colors::TextSecondary);

    // Error message - Inter Regular
    errorMessage.setFont(*bodyFont);
    errorMessage.setString("");
    errorMessage.setCharacterSize(UIStyles::Typography::CaptionSize);
    errorMessage.setFillColor(UIStyles::Colors::TextError);
    errorMessage.setStyle(sf::Text::Bold);

    // Input fields with Inter Regular
    emailField = InputField(sf::Vector2f(400.f, 240.f), sf::Vector2f(450.f, UIStyles::Sizes::InputHeight), "Email Address", *bodyFont);
    emailField.setIsPasswordField(false);
    
    passwordField = InputField(sf::Vector2f(400.f, 320.f), sf::Vector2f(450.f, UIStyles::Sizes::InputHeight), "Password", *bodyFont);
    passwordField.setIsPasswordField(true);
    
    // Stay logged in checkbox - Inter Regular
    chkStayLoggedIn = Checkbox(sf::Vector2f(400.f, 380.f), "Stay logged in", *bodyFont);

    // Forgot password link button - Inter SemiBold
    btnForgotPassword = Button(
        sf::Vector2f(600.f, 380.f),
        sf::Vector2f(160.f, 30.f),
        "Forgot password?",
        *buttonFont,
        UIStyles::Typography::CaptionSize
    );
    btnForgotPassword.setStyle(ButtonStyle::Link);

    // Login button - Inter SemiBold Primary style
    btnLogin = Button(
        sf::Vector2f(400.f, 430.f),
        sf::Vector2f(450.f, UIStyles::Sizes::ButtonHeight),
        "Log in",
        *buttonFont,
        UIStyles::Typography::BodySize
    );
    btnLogin.setStyle(ButtonStyle::Primary);

    // Divider text "Or continue with" - Inter Regular
    dividerText.setFont(*bodyFont);
    dividerText.setString("Or continue with");
    dividerText.setCharacterSize(UIStyles::Typography::CaptionSize);
    dividerText.setFillColor(UIStyles::Colors::TextSecondary);

    // OAuth buttons - Inter SemiBold
    btnGoogle = Button(
        sf::Vector2f(400.f, 540.f),
        sf::Vector2f(220.f, UIStyles::Sizes::ButtonHeight),
        "Google",
        *buttonFont,
        UIStyles::Typography::BodySize
    );
    btnGoogle.setStyle(ButtonStyle::OAuth);

    btnApple = Button(
        sf::Vector2f(630.f, 540.f),
        sf::Vector2f(220.f, UIStyles::Sizes::ButtonHeight),
        "Apple",
        *buttonFont,
        UIStyles::Typography::BodySize
    );
    btnApple.setStyle(ButtonStyle::OAuth);

    // Sign up link at bottom - Inter Regular + SemiBold for link
    signupText.setFont(*bodyFont);
    signupText.setString("Don't have an account? ");
    signupText.setCharacterSize(UIStyles::Typography::CaptionSize);
    signupText.setFillColor(UIStyles::Colors::TextSecondary);

    btnSignUp = Button(
        sf::Vector2f(550.f, 600.f),
        sf::Vector2f(80.f, 30.f),
        "Sign up",
        *buttonFont,
        UIStyles::Typography::CaptionSize
    );
    btnSignUp.setStyle(ButtonStyle::Link);

    // Card background rectangle
    cardBackground.setFillColor(UIStyles::Colors::CardBackground);
    cardBackground.setSize(sf::Vector2f(500.f, 620.f));
}

void LoginScreen::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) {
    // Handle text input
    if (event.type == sf::Event::TextEntered) {
        if (emailField.getIsActive()) {
            emailField.handleTextInput(event.text.unicode);
        } else if (passwordField.getIsActive()) {
            passwordField.handleTextInput(event.text.unicode);
        }
    }
    
    // Update hover states
    btnLogin.updateHover(mousePos);
    btnGoogle.updateHover(mousePos);
    btnApple.updateHover(mousePos);
    btnForgotPassword.updateHover(mousePos);
    btnSignUp.updateHover(mousePos);
    chkStayLoggedIn.updateHover(mousePos);
    emailField.updateHover(mousePos);
    passwordField.updateHover(mousePos);

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        // Check input field clicks
        if (emailField.clicked(mousePos)) {
            emailField.setActive(true);
            passwordField.setActive(false);
        } else if (passwordField.clicked(mousePos)) {
            passwordField.setActive(true);
            emailField.setActive(false);
        } else {
            emailField.setActive(false);
            passwordField.setActive(false);
        }
        
        // Check checkbox
        chkStayLoggedIn.clicked(mousePos);

        if (btnLogin.clicked(mousePos)) {
            attemptLogin();
        }
        else if (btnSignUp.clicked(mousePos)) {
            screenManager->changeState(STATE_CREATE_ACCOUNT);
        }
        else if (btnForgotPassword.clicked(mousePos)) {
            screenManager->changeState(STATE_FORGOT_PASSWORD);
        }
        else if (btnGoogle.clicked(mousePos)) {
            std::cout << "[LoginScreen] Google OAuth not implemented" << std::endl;
            // TODO: Implement Google OAuth
        }
        else if (btnApple.clicked(mousePos)) {
            std::cout << "[LoginScreen] Apple OAuth not implemented" << std::endl;
            // TODO: Implement Apple OAuth
        }
    }

    // Enter key to login
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
        attemptLogin();
    }
}

void LoginScreen::update(float deltaTime) {
    // Hide error message after 3 seconds
    if (showError) {
        errorTimer += deltaTime;
        if (errorTimer >= 3.0f) {
            showError = false;
            errorTimer = 0.0f;
        }
    }
}

void LoginScreen::draw(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f winSizeF((float)windowSize.x, (float)windowSize.y);
    
    // Draw background image (full screen)
    if (backgroundSprite.getTexture()) {
        sf::Vector2u bgSize = backgroundSprite.getTexture()->getSize();
        float scaleX = winSizeF.x / bgSize.x;
        float scaleY = winSizeF.y / bgSize.y;
        backgroundSprite.setScale(scaleX, scaleY);
        backgroundSprite.setPosition(0, 0);
        window.draw(backgroundSprite);
    } else {
        // Fallback to light background
        window.clear(UIStyles::Colors::Background);
    }

    // Draw centered card background
    sf::Vector2f cardSize = UIHelpers::scaleSize(sf::Vector2f(550.f, 680.f), winSizeF);
    sf::Vector2f cardPos((winSizeF.x - cardSize.x) / 2.f, (winSizeF.y - cardSize.y) / 2.f);
    cardBackground.setSize(cardSize);
    cardBackground.setPosition(cardPos);
    window.draw(cardBackground);

    // All positions relative to card center
    float centerX = winSizeF.x / 2.f;
    float cardTop = cardPos.y + UIStyles::Spacing::XXLarge;

    // Title
    title.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::TitleSize, winSizeF));
    UIHelpers::centerText(title, centerX, cardTop);
    window.draw(title);

    // Subtitle
    subtitle.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::CaptionSize, winSizeF));
    UIHelpers::centerText(subtitle, centerX, cardTop + UIHelpers::scaleFont(50.f, winSizeF));
    window.draw(subtitle);

    // Email field
    sf::Vector2f inputSize = UIHelpers::scaleSize(sf::Vector2f(450.f, UIStyles::Sizes::InputHeight), winSizeF);
    sf::Vector2f emailPos((winSizeF.x - inputSize.x) / 2.f, cardTop + UIHelpers::scaleFont(110.f, winSizeF));
    emailField.setPosition(emailPos);
    emailField.setSize(inputSize);
    emailField.draw(window);

    // Password field
    sf::Vector2f passPos((winSizeF.x - inputSize.x) / 2.f, cardTop + UIHelpers::scaleFont(190.f, winSizeF));
    passwordField.setPosition(passPos);
    passwordField.setSize(inputSize);
    passwordField.draw(window);

    // Checkbox and forgot password on same row
    float rowY = cardTop + UIHelpers::scaleFont(250.f, winSizeF);
    sf::Vector2f checkboxPos((winSizeF.x - inputSize.x) / 2.f, rowY);
    chkStayLoggedIn.setPosition(checkboxPos);
    chkStayLoggedIn.setLabelSize(static_cast<unsigned int>(UIHelpers::scaleFont((float)UIStyles::Typography::CaptionSize, winSizeF)));
    chkStayLoggedIn.draw(window);

    // Forgot password (right aligned)
    sf::Vector2f forgotSize = UIHelpers::scaleSize(sf::Vector2f(140.f, 30.f), winSizeF);
    sf::Vector2f forgotPos((winSizeF.x + inputSize.x) / 2.f - forgotSize.x, rowY - 5.f);
    btnForgotPassword.setPosition(forgotPos);
    btnForgotPassword.getRect().setSize(forgotSize);
    btnForgotPassword.getText().setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::CaptionSize, winSizeF));
    UIHelpers::centerText(btnForgotPassword.getText(), forgotPos.x + forgotSize.x / 2.f, forgotPos.y + forgotSize.y / 2.f);
    btnForgotPassword.draw(window);

    // Login button
    sf::Vector2f btnSize = UIHelpers::scaleSize(sf::Vector2f(450.f, UIStyles::Sizes::ButtonHeight), winSizeF);
    sf::Vector2f loginPos((winSizeF.x - btnSize.x) / 2.f, cardTop + UIHelpers::scaleFont(290.f, winSizeF));
    btnLogin.setPosition(loginPos);
    btnLogin.getRect().setSize(btnSize);
    btnLogin.getText().setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::BodySize, winSizeF));
    UIHelpers::centerText(btnLogin.getText(), loginPos.x + btnSize.x / 2.f, loginPos.y + btnSize.y / 2.f);
    btnLogin.draw(window);

    // Divider "Or continue with"
    dividerText.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::CaptionSize, winSizeF));
    UIHelpers::centerText(dividerText, centerX, cardTop + UIHelpers::scaleFont(370.f, winSizeF));
    window.draw(dividerText);

    // OAuth buttons (Google and Apple side by side)
    sf::Vector2f oauthBtnSize = UIHelpers::scaleSize(sf::Vector2f(220.f, UIStyles::Sizes::ButtonHeight), winSizeF);
    float oauthY = cardTop + UIHelpers::scaleFont(410.f, winSizeF);
    float spacing = UIHelpers::scaleFont(10.f, winSizeF);
    
    // Google button (left)
    sf::Vector2f googlePos(centerX - oauthBtnSize.x - spacing / 2.f, oauthY);
    btnGoogle.setPosition(googlePos);
    btnGoogle.getRect().setSize(oauthBtnSize);
    btnGoogle.getText().setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::BodySize, winSizeF));
    UIHelpers::centerText(btnGoogle.getText(), googlePos.x + oauthBtnSize.x / 2.f, googlePos.y + oauthBtnSize.y / 2.f);
    btnGoogle.draw(window);

    // Apple button (right)
    sf::Vector2f applePos(centerX + spacing / 2.f, oauthY);
    btnApple.setPosition(applePos);
    btnApple.getRect().setSize(oauthBtnSize);
    btnApple.getText().setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::BodySize, winSizeF));
    UIHelpers::centerText(btnApple.getText(), applePos.x + oauthBtnSize.x / 2.f, applePos.y + oauthBtnSize.y / 2.f);
    btnApple.draw(window);

    // Sign up text and link at bottom
    signupText.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::CaptionSize, winSizeF));
    float signupY = cardTop + UIHelpers::scaleFont(500.f, winSizeF);
    UIHelpers::centerText(signupText, centerX - UIHelpers::scaleFont(40.f, winSizeF), signupY);
    window.draw(signupText);

    sf::Vector2f signupBtnSize = UIHelpers::scaleSize(sf::Vector2f(70.f, 30.f), winSizeF);
    sf::Vector2f signupBtnPos(centerX + UIHelpers::scaleFont(45.f, winSizeF), signupY - 15.f);
    btnSignUp.setPosition(signupBtnPos);
    btnSignUp.getRect().setSize(signupBtnSize);
    btnSignUp.getText().setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::CaptionSize, winSizeF));
    UIHelpers::centerText(btnSignUp.getText(), signupBtnPos.x + signupBtnSize.x / 2.f, signupBtnPos.y + signupBtnSize.y / 2.f);
    btnSignUp.draw(window);

    // Error message (if any)
    if (showError) {
        errorMessage.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::CaptionSize, winSizeF));
        UIHelpers::centerText(errorMessage, centerX, cardTop + UIHelpers::scaleFont(540.f, winSizeF));
        window.draw(errorMessage);
    }

    // Footer text
    sf::Text footer;
    footer.setFont(*screenManager->getFontManager()->getFont(FontType::INTER_REGULAR));
    footer.setString("© 2024 Chess Master Inc. All rights reserved.");
    footer.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::SmallSize, winSizeF));
    footer.setFillColor(UIStyles::Colors::TextSecondary);
    UIHelpers::centerText(footer, centerX, winSizeF.y - UIHelpers::scaleFont(30.f, winSizeF));
    window.draw(footer);
}

void LoginScreen::attemptLogin() {
    std::string email = emailField.getContent();
    std::string password = passwordField.getContent();

    std::cout << "[LoginScreen] Attempting login for email: " << email << std::endl;

    // Validate inputs
    if (email.empty() || password.empty()) {
        showErrorMessage("Please fill in all fields");
        return;
    }

    // Validate with database
    SQLiteManager& db = screenManager->getDatabaseManager();
    
    if (db.validateUser(email, password)) {
        std::cout << "[LoginScreen] Login successful!" << std::endl;
        
        // Get user ID from database
        int userId = db.getUserId(email);
        
        if (userId > 0) {
            // Set user data
            UserData& user = screenManager->getUserData();
            user.id = userId;
            user.setEmail(email);
            user.setPassword(password);
            user.stayLoggedIn = chkStayLoggedIn.isChecked();
            
            // Update stay logged in preference in database
            if (chkStayLoggedIn.isChecked()) {
                db.updateStayLoggedIn(userId, true);
                std::cout << "[LoginScreen] Stay logged in enabled" << std::endl;
            } else {
                db.updateStayLoggedIn(userId, false);
                std::cout << "[LoginScreen] Stay logged in disabled" << std::endl;
            }
            
            std::cout << "[LoginScreen] User logged in with ID: " << userId << std::endl;
            
            screenManager->changeState(STATE_MAIN_MENU);
        } else {
            std::cout << "[LoginScreen] Error: Could not retrieve user ID" << std::endl;
            showErrorMessage("Login error - please try again");
        }
    } else {
        std::cout << "[LoginScreen] Login failed - invalid credentials" << std::endl;
        showErrorMessage("Invalid email or password");
    }
}

void LoginScreen::showErrorMessage(const std::string& msg) {
    errorMessage.setString(msg);
    showError = true;
    errorTimer = 0.0f;
}

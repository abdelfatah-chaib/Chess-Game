#include "CreateAccountScreen.h"
#include "ScreenManager.h"
#include "UIHelpers.h"
#include "UIStyles.h"
#include <iostream>

CreateAccountScreen::CreateAccountScreen(ScreenManager* manager) : Screen(manager) {
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

    // Card background
    cardBackground.setFillColor(UIStyles::Colors::CardBackground);
    cardBackground.setSize(sf::Vector2f(580.f, 750.f));

    // Try to load header image (chess pieces)
    sf::Texture* headerTexture = manager->getTextureManager()->getTexture("bg_create");
    if (headerTexture) {
        headerImage.setTexture(*headerTexture);
    }

    // Title - "Create your account" - Cinzel Regular
    title.setFont(*titleFont);
    title.setString("Create your account");
    title.setCharacterSize(UIStyles::Typography::SubtitleSize);
    title.setFillColor(UIStyles::Colors::TextPrimary);
    title.setStyle(sf::Text::Bold);

    // Subtitle - Inter Regular
    subtitle.setFont(*bodyFont);
    subtitle.setString("Enter your details to start your journey to Grandmaster today.");
    subtitle.setCharacterSize(UIStyles::Typography::CaptionSize);
    subtitle.setFillColor(UIStyles::Colors::TextSecondary);

    // Input fields - Inter Regular
    usernameField = InputField(sf::Vector2f(400.f, 280.f), sf::Vector2f(480.f, UIStyles::Sizes::InputHeight), "Username", *bodyFont);
    
    emailField = InputField(sf::Vector2f(400.f, 360.f), sf::Vector2f(480.f, UIStyles::Sizes::InputHeight), "Email Address", *bodyFont);
    emailField.setIsPasswordField(false);
    
    passwordField = InputField(sf::Vector2f(400.f, 440.f), sf::Vector2f(480.f, UIStyles::Sizes::InputHeight), "Password", *bodyFont);
    passwordField.setIsPasswordField(true);
    
    confirmPasswordField = InputField(sf::Vector2f(400.f, 520.f), sf::Vector2f(480.f, UIStyles::Sizes::InputHeight), "Confirm Password", *bodyFont);
    confirmPasswordField.setIsPasswordField(true);

    // Create Account button - Inter SemiBold Primary style
    btnCreateAccount = Button(
        sf::Vector2f(400.f, 590.f),
        sf::Vector2f(480.f, UIStyles::Sizes::ButtonHeight),
        "Create Account",
        *buttonFont,
        UIStyles::Typography::BodySize
    );
    btnCreateAccount.setStyle(ButtonStyle::Primary);

    // Divider text "Or continue with" - Inter Regular
    dividerText.setFont(*bodyFont);
    dividerText.setString("Or continue with");
    dividerText.setCharacterSize(UIStyles::Typography::CaptionSize);
    dividerText.setFillColor(UIStyles::Colors::TextSecondary);

    // OAuth buttons - Inter SemiBold
    btnGoogle = Button(
        sf::Vector2f(400.f, 690.f),
        sf::Vector2f(235.f, UIStyles::Sizes::ButtonHeight),
        "Google",
        *buttonFont,
        UIStyles::Typography::BodySize
    );
    btnGoogle.setStyle(ButtonStyle::OAuth);

    btnApple = Button(
        sf::Vector2f(645.f, 690.f),
        sf::Vector2f(235.f, UIStyles::Sizes::ButtonHeight),
        "Apple",
        *buttonFont,
        UIStyles::Typography::BodySize
    );
    btnApple.setStyle(ButtonStyle::OAuth);

    // Login link at bottom - Inter Regular + SemiBold
    loginText.setFont(*bodyFont);
    loginText.setString("Already have an account? ");
    loginText.setCharacterSize(UIStyles::Typography::CaptionSize);
    loginText.setFillColor(UIStyles::Colors::TextSecondary);

    btnLogin = Button(
        sf::Vector2f(550.f, 755.f),
        sf::Vector2f(70.f, 30.f),
        "Log in",
        *buttonFont,
        UIStyles::Typography::CaptionSize
    );
    btnLogin.setStyle(ButtonStyle::Link);
}

void CreateAccountScreen::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) {
    // Handle text input
    if (event.type == sf::Event::TextEntered) {
        if (usernameField.getIsActive()) {
            usernameField.handleTextInput(event.text.unicode);
        } else if (emailField.getIsActive()) {
            emailField.handleTextInput(event.text.unicode);
        } else if (passwordField.getIsActive()) {
            passwordField.handleTextInput(event.text.unicode);
        } else if (confirmPasswordField.getIsActive()) {
            confirmPasswordField.handleTextInput(event.text.unicode);
        }
    }

    // Update hover states
    btnCreateAccount.updateHover(mousePos);
    btnGoogle.updateHover(mousePos);
    btnApple.updateHover(mousePos);
    btnLogin.updateHover(mousePos);
    usernameField.updateHover(mousePos);
    emailField.updateHover(mousePos);
    passwordField.updateHover(mousePos);
    confirmPasswordField.updateHover(mousePos);

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        // Deactivate all fields first
        usernameField.setActive(false);
        emailField.setActive(false);
        passwordField.setActive(false);
        confirmPasswordField.setActive(false);

        // Check which field was clicked
        if (usernameField.clicked(mousePos)) {
            usernameField.setActive(true);
        } else if (emailField.clicked(mousePos)) {
            emailField.setActive(true);
        } else if (passwordField.clicked(mousePos)) {
            passwordField.setActive(true);
        } else if (confirmPasswordField.clicked(mousePos)) {
            confirmPasswordField.setActive(true);
        }

        // Button clicks
        if (btnCreateAccount.clicked(mousePos)) {
            attemptCreateAccount();
        }
        else if (btnLogin.clicked(mousePos)) {
            screenManager->changeState(STATE_LOGIN);
        }
        else if (btnGoogle.clicked(mousePos)) {
            std::cout << "[CreateAccountScreen] Google OAuth not implemented" << std::endl;
        }
        else if (btnApple.clicked(mousePos)) {
            std::cout << "[CreateAccountScreen] Apple OAuth not implemented" << std::endl;
        }
    }

    // Enter key
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
        attemptCreateAccount();
    }
}

void CreateAccountScreen::update(float deltaTime) {
    // No specific update logic needed
}

void CreateAccountScreen::draw(sf::RenderWindow& window) {
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

    // Draw centered card
    sf::Vector2f cardSize = UIHelpers::scaleSize(sf::Vector2f(580.f, 820.f), winSizeF);
    sf::Vector2f cardPos((winSizeF.x - cardSize.x) / 2.f, (winSizeF.y - cardSize.y) / 2.f);
    cardBackground.setSize(cardSize);
    cardBackground.setPosition(cardPos);
    window.draw(cardBackground);

    float centerX = winSizeF.x / 2.f;
    float cardTop = cardPos.y;

    // Header image at top of card (if available)
    if (headerImage.getTexture()) {
        sf::Vector2f headerSize = UIHelpers::scaleSize(sf::Vector2f(580.f, 140.f), winSizeF);
        headerImage.setPosition(cardPos);
        
        // Scale to fit card width
        sf::Vector2u texSize = headerImage.getTexture()->getSize();
        float scaleX = headerSize.x / texSize.x;
        float scaleY = headerSize.y / texSize.y;
        headerImage.setScale(scaleX, scaleY);
        
        // Clip to card bounds
        sf::FloatRect headerRect(cardPos.x, cardPos.y, headerSize.x, headerSize.y);
        window.draw(headerImage);
        
        cardTop += headerSize.y;
    }

    cardTop += UIHelpers::scaleFont(UIStyles::Spacing::Large, winSizeF);

    // Title
    title.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::SubtitleSize, winSizeF));
    UIHelpers::centerText(title, centerX, cardTop);
    window.draw(title);

    // Subtitle
    subtitle.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::CaptionSize, winSizeF));
    UIHelpers::centerText(subtitle, centerX, cardTop + UIHelpers::scaleFont(40.f, winSizeF));
    window.draw(subtitle);

    // Input fields
    sf::Vector2f inputSize = UIHelpers::scaleSize(sf::Vector2f(480.f, UIStyles::Sizes::InputHeight), winSizeF);
    float inputX = (winSizeF.x - inputSize.x) / 2.f;
    
    float yOffset = cardTop + UIHelpers::scaleFont(100.f, winSizeF);
    usernameField.setPosition(sf::Vector2f(inputX, yOffset));
    usernameField.setSize(inputSize);
    usernameField.draw(window);

    yOffset += UIHelpers::scaleFont(80.f, winSizeF);
    emailField.setPosition(sf::Vector2f(inputX, yOffset));
    emailField.setSize(inputSize);
    emailField.draw(window);

    yOffset += UIHelpers::scaleFont(80.f, winSizeF);
    passwordField.setPosition(sf::Vector2f(inputX, yOffset));
    passwordField.setSize(inputSize);
    passwordField.draw(window);

    yOffset += UIHelpers::scaleFont(80.f, winSizeF);
    confirmPasswordField.setPosition(sf::Vector2f(inputX, yOffset));
    confirmPasswordField.setSize(inputSize);
    confirmPasswordField.draw(window);

    // Create Account button
    yOffset += UIHelpers::scaleFont(70.f, winSizeF);
    sf::Vector2f btnSize = UIHelpers::scaleSize(sf::Vector2f(480.f, UIStyles::Sizes::ButtonHeight), winSizeF);
    sf::Vector2f btnPos((winSizeF.x - btnSize.x) / 2.f, yOffset);
    btnCreateAccount.setPosition(btnPos);
    btnCreateAccount.getRect().setSize(btnSize);
    btnCreateAccount.getText().setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::BodySize, winSizeF));
    UIHelpers::centerText(btnCreateAccount.getText(), btnPos.x + btnSize.x / 2.f, btnPos.y + btnSize.y / 2.f);
    btnCreateAccount.draw(window);

    // Divider
    yOffset += UIHelpers::scaleFont(70.f, winSizeF);
    dividerText.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::CaptionSize, winSizeF));
    UIHelpers::centerText(dividerText, centerX, yOffset);
    window.draw(dividerText);

    // OAuth buttons
    yOffset += UIHelpers::scaleFont(40.f, winSizeF);
    sf::Vector2f oauthBtnSize = UIHelpers::scaleSize(sf::Vector2f(235.f, UIStyles::Sizes::ButtonHeight), winSizeF);
    float spacing = UIHelpers::scaleFont(10.f, winSizeF);
    
    sf::Vector2f googlePos(centerX - oauthBtnSize.x - spacing / 2.f, yOffset);
    btnGoogle.setPosition(googlePos);
    btnGoogle.getRect().setSize(oauthBtnSize);
    btnGoogle.getText().setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::BodySize, winSizeF));
    UIHelpers::centerText(btnGoogle.getText(), googlePos.x + oauthBtnSize.x / 2.f, googlePos.y + oauthBtnSize.y / 2.f);
    btnGoogle.draw(window);

    sf::Vector2f applePos(centerX + spacing / 2.f, yOffset);
    btnApple.setPosition(applePos);
    btnApple.getRect().setSize(oauthBtnSize);
    btnApple.getText().setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::BodySize, winSizeF));
    UIHelpers::centerText(btnApple.getText(), applePos.x + oauthBtnSize.x / 2.f, applePos.y + oauthBtnSize.y / 2.f);
    btnApple.draw(window);

    // Login link
    yOffset += UIHelpers::scaleFont(60.f, winSizeF);
    loginText.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::CaptionSize, winSizeF));
    UIHelpers::centerText(loginText, centerX - UIHelpers::scaleFont(35.f, winSizeF), yOffset);
    window.draw(loginText);

    sf::Vector2f loginBtnSize = UIHelpers::scaleSize(sf::Vector2f(70.f, 30.f), winSizeF);
    sf::Vector2f loginBtnPos(centerX + UIHelpers::scaleFont(50.f, winSizeF), yOffset - 15.f);
    btnLogin.setPosition(loginBtnPos);
    btnLogin.getRect().setSize(loginBtnSize);
    btnLogin.getText().setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::CaptionSize, winSizeF));
    UIHelpers::centerText(btnLogin.getText(), loginBtnPos.x + loginBtnSize.x / 2.f, loginBtnPos.y + loginBtnSize.y / 2.f);
    btnLogin.draw(window);

    // Footer
    sf::Text footer;
    footer.setFont(*screenManager->getFontManager()->getFont(FontType::INTER_REGULAR));
    footer.setString("© 2024 ChessMaster Inc. All rights reserved.");
    footer.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::SmallSize, winSizeF));
    footer.setFillColor(UIStyles::Colors::TextSecondary);
    UIHelpers::centerText(footer, centerX, winSizeF.y - UIHelpers::scaleFont(30.f, winSizeF));
    window.draw(footer);
}

void CreateAccountScreen::attemptCreateAccount() {
    std::string username = usernameField.getContent();
    std::string email = emailField.getContent();
    std::string password = passwordField.getContent();
    std::string confirmPass = confirmPasswordField.getContent();

    // Validation
    if (username.empty() || email.empty() || password.empty() || confirmPass.empty()) {
        std::cout << "[CreateAccount] All fields required" << std::endl;
        return;
    }

    if (!usernameField.validateUsername()) {
        return;
    }

    if (!emailField.validateEmail()) {
        return;
    }

    if (password != confirmPass) {
        confirmPasswordField.setError("Passwords do not match");
        return;
    }

    if (password.length() < 6) {
        passwordField.setError("Password must be at least 6 characters");
        return;
    }

    // Save to database
    SQLiteManager& db = screenManager->getDatabaseManager();
    
    if (db.insertUser(username, email, password)) {
        std::cout << "[CreateAccount] Account created successfully!" << std::endl;
        
        // Set user data and login
        UserData& user = screenManager->getUserData();
        user.setUsername(username);
        user.setEmail(email);
        user.setPassword(password);
        user.id = db.getUserId(email);
        
        screenManager->changeState(STATE_CONFIRMATION);
    } else {
        std::cerr << "[CreateAccount] Failed to create account" << std::endl;
        emailField.setError("Email already exists or database error");
    }
}

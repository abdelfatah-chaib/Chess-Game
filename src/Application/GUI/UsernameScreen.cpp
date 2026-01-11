#include "UsernameScreen.h"
#include "Services/ScreenManager.h"
#include "UIHelpers.h"
#include "UIStyles.h"
#include <iostream>

UsernameScreen::UsernameScreen(ScreenManager* manager) 
    : Screen(manager), activeInputField(nullptr) {
    
    // Load modern fonts
    sf::Font* titleFont = manager->getFontManager()->getFont(FontType::CINZEL_REGULAR);
    sf::Font* bodyFont = manager->getFontManager()->getFont(FontType::INTER_REGULAR);
    sf::Font* buttonFont = manager->getFontManager()->getFont(FontType::INTER_SEMIBOLD);
    
    // Fallback
    if (!titleFont) titleFont = manager->getFontManager()->getFont(FontType::INTER_REGULAR);
    if (!bodyFont) bodyFont = titleFont;
    if (!buttonFont) buttonFont = bodyFont;

    // Left side - dark background with chess image
    leftBackground.setFillColor(sf::Color(20, 20, 30)); // Dark background
    leftBackground.setSize(sf::Vector2f(600.f, 800.f));

    // Try to load left side chess image
    sf::Texture* chessBgTexture = manager->getTextureManager()->getTexture("bg_username");
    if (chessBgTexture) {
        leftSideImage.setTexture(*chessBgTexture);
    }

    // Left side title - "Master Your Strategy" - Cinzel Regular
    leftTitle.setFont(*titleFont);
    leftTitle.setString("Master Your Strategy");
    leftTitle.setCharacterSize(UIStyles::Typography::TitleSize);
    leftTitle.setFillColor(sf::Color::White);
    leftTitle.setStyle(sf::Text::Bold);

    // Left side description - Inter Regular
    leftDescription.setFont(*bodyFont);
    leftDescription.setString("Join a community of over 5 million players. Track your\nprogress, analyze games, and climb the global\nleaderboards.");
    leftDescription.setCharacterSize(UIStyles::Typography::BodySize);
    leftDescription.setFillColor(sf::Color(200, 200, 210));

    // Badge "SEASON 4 LIVE" - Inter Regular
    badge.setFillColor(sf::Color(255, 255, 255, 30));
    badge.setSize(sf::Vector2f(140.f, 30.f));

    badgeText.setFont(*bodyFont);
    badgeText.setString("SEASON 4 LIVE");
    badgeText.setCharacterSize(UIStyles::Typography::SmallSize);
    badgeText.setFillColor(sf::Color::White);
    badgeText.setStyle(sf::Text::Bold);

    // Right side - white background
    rightBackground.setFillColor(sf::Color::White);
    rightBackground.setSize(sf::Vector2f(600.f, 800.f));

    // Progress indicators (3 dots at top)
    progressIndicator1.setFillColor(UIStyles::Colors::Primary);
    progressIndicator1.setSize(sf::Vector2f(40.f, 4.f));
    
    progressIndicator2.setFillColor(sf::Color(200, 200, 200));
    progressIndicator2.setSize(sf::Vector2f(40.f, 4.f));
    
    progressIndicator3.setFillColor(sf::Color(200, 200, 200));
    progressIndicator3.setSize(sf::Vector2f(40.f, 4.f));

    // Right side title - "Welcome, Grandmaster" - Cinzel Regular
    title.setFont(*titleFont);
    title.setString("Welcome, Grandmaster");
    title.setCharacterSize(UIStyles::Typography::SubtitleSize);
    title.setFillColor(UIStyles::Colors::TextPrimary);
    title.setStyle(sf::Text::Bold);

    // Subtitle - Inter Regular
    subtitle.setFont(*bodyFont);
    subtitle.setString("Create a unique identity to start your journey. This will be\nvisible to your opponents in the arena.");
    subtitle.setCharacterSize(UIStyles::Typography::CaptionSize);
    subtitle.setFillColor(UIStyles::Colors::TextSecondary);

    // Info box text - Inter Regular
    infoText.setFont(*bodyFont);
    infoText.setString("Your ELO rating will be calibrated after your first 5 placement matches.\nGood luck!");
    infoText.setCharacterSize(UIStyles::Typography::SmallSize);
    infoText.setFillColor(UIStyles::Colors::Primary);

    // Username field with user icon - Inter Regular
    usernameField = InputField(
        sf::Vector2f(700.f, 350.f),
        sf::Vector2f(400.f, UIStyles::Sizes::InputHeight),
        "Username",
        *bodyFont
    );
    usernameField.setIsPasswordField(false);

    // Enter Arena button - Inter SemiBold Primary
    btnContinue = Button(
        sf::Vector2f(700.f, 440.f),
        sf::Vector2f(400.f, UIStyles::Sizes::ButtonHeight),
        "Enter Arena →",
        *buttonFont,
        UIStyles::Typography::BodySize
    );
    btnContinue.setStyle(ButtonStyle::Primary);

    // Continue as Guest button - Inter SemiBold Link style
    btnContinueAsGuest = Button(
        sf::Vector2f(700.f, 500.f),
        sf::Vector2f(200.f, 30.f),
        "Continue as Guest",
        *buttonFont,
        UIStyles::Typography::CaptionSize
    );
    btnContinueAsGuest.setStyle(ButtonStyle::Link);
}

void UsernameScreen::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) {
    if (event.type == sf::Event::TextEntered) {
        if (activeInputField) {
            activeInputField->handleTextInput(event.text.unicode);
        }
    }
    
    btnContinue.updateHover(mousePos);
    btnContinueAsGuest.updateHover(mousePos);
    usernameField.updateHover(mousePos);
    
    if (event.type == sf::Event::MouseButtonPressed && 
        event.mouseButton.button == sf::Mouse::Left) {
        
        if (usernameField.clicked(mousePos)) {
            activeInputField = &usernameField;
            usernameField.setActive(true);
        } else {
            activeInputField = nullptr;
            usernameField.setActive(false);
        }
        
        if (btnContinue.clicked(mousePos)) {
            if (usernameField.validateUsername()) {
                UserData& user = screenManager->getUserData();
                user.setUsername(usernameField.getContent());
                
                // Save user to database
                SQLiteManager& db = screenManager->getDatabaseManager();
                
                if (db.insertUser(user.getUsername(), user.getEmail(), user.getPassword())) {
                    std::cout << "[UsernameScreen] User created successfully in database!" << std::endl;
                    
                    // Get the newly created user ID
                    user.id = db.getUserId(user.getEmail());
                    std::cout << "[UsernameScreen] User ID: " << user.id << std::endl;
                    
                    screenManager->changeState(STATE_CONFIRMATION);
                } else {
                    std::cerr << "[UsernameScreen] Failed to create user in database!" << std::endl;
                    screenManager->changeState(STATE_CONFIRMATION);
                }
            }
        }
        else if (btnContinueAsGuest.clicked(mousePos)) {
            std::cout << "[UsernameScreen] Continue as guest" << std::endl;
            UserData& user = screenManager->getUserData();
            user.setUsername("Guest");
            screenManager->changeState(STATE_MAIN_MENU);
        }
    }
}

void UsernameScreen::update(float deltaTime) {
    // No specific update logic needed
}

void UsernameScreen::draw(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f winSizeF((float)windowSize.x, (float)windowSize.y);
    
    // Split screen 50/50
    float halfWidth = winSizeF.x / 2.f;

    // LEFT SIDE - Dark background with chess image
    leftBackground.setSize(sf::Vector2f(halfWidth, winSizeF.y));
    leftBackground.setPosition(0, 0);
    window.draw(leftBackground);

    // Draw left side chess image if available
    if (leftSideImage.getTexture()) {
        sf::Vector2u texSize = leftSideImage.getTexture()->getSize();
        float scale = std::min(halfWidth / texSize.x, winSizeF.y / texSize.y);
        leftSideImage.setScale(scale, scale);
        
        // Center the image
        float imgWidth = texSize.x * scale;
        float imgHeight = texSize.y * scale;
        leftSideImage.setPosition((halfWidth - imgWidth) / 2.f, (winSizeF.y - imgHeight) / 2.f);
        window.draw(leftSideImage);
    }

    // Left side badge (top left corner)
    float badgeX = UIHelpers::scaleFont(UIStyles::Spacing::Large, winSizeF);
    float badgeY = UIHelpers::scaleFont(UIStyles::Spacing::Large, winSizeF);
    sf::Vector2f badgeSize = UIHelpers::scaleSize(sf::Vector2f(140.f, 30.f), winSizeF);
    badge.setSize(badgeSize);
    badge.setPosition(badgeX, badgeY);
    window.draw(badge);

    badgeText.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::SmallSize, winSizeF));
    UIHelpers::centerText(badgeText, badgeX + badgeSize.x / 2.f, badgeY + badgeSize.y / 2.f);
    window.draw(badgeText);

    // Left side title (bottom area)
    float leftTextY = winSizeF.y - UIHelpers::scaleFont(200.f, winSizeF);
    leftTitle.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::SubtitleSize, winSizeF));
    leftTitle.setPosition(badgeX, leftTextY);
    window.draw(leftTitle);

    // Left side description
    leftDescription.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::CaptionSize, winSizeF));
    leftDescription.setPosition(badgeX, leftTextY + UIHelpers::scaleFont(60.f, winSizeF));
    window.draw(leftDescription);

    // RIGHT SIDE - White background
    rightBackground.setSize(sf::Vector2f(halfWidth, winSizeF.y));
    rightBackground.setPosition(halfWidth, 0);
    window.draw(rightBackground);

    float rightStartX = halfWidth + UIHelpers::scaleFont(UIStyles::Spacing::XXLarge, winSizeF);
    float rightCenterX = halfWidth + halfWidth / 2.f;

    // Progress indicators at top
    float progressY = UIHelpers::scaleFont(UIStyles::Spacing::XLarge, winSizeF);
    float progressSpacing = UIHelpers::scaleFont(10.f, winSizeF);
    sf::Vector2f progressSize = UIHelpers::scaleSize(sf::Vector2f(40.f, 4.f), winSizeF);
    
    progressIndicator1.setSize(progressSize);
    progressIndicator1.setPosition(rightCenterX - progressSize.x - progressSpacing - progressSize.x / 2.f, progressY);
    window.draw(progressIndicator1);
    
    progressIndicator2.setSize(progressSize);
    progressIndicator2.setPosition(rightCenterX - progressSize.x / 2.f, progressY);
    window.draw(progressIndicator2);
    
    progressIndicator3.setSize(progressSize);
    progressIndicator3.setPosition(rightCenterX + progressSize.x / 2.f + progressSpacing, progressY);
    window.draw(progressIndicator3);

    // Title
    float contentY = UIHelpers::scaleFont(120.f, winSizeF);
    title.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::SubtitleSize, winSizeF));
    title.setPosition(rightStartX, contentY);
    window.draw(title);

    // Subtitle
    subtitle.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::CaptionSize, winSizeF));
    subtitle.setPosition(rightStartX, contentY + UIHelpers::scaleFont(50.f, winSizeF));
    window.draw(subtitle);

    // Username input field
    sf::Vector2f inputSize = UIHelpers::scaleSize(sf::Vector2f(400.f, UIStyles::Sizes::InputHeight), winSizeF);
    sf::Vector2f inputPos(rightStartX, contentY + UIHelpers::scaleFont(150.f, winSizeF));
    usernameField.setPosition(inputPos);
    usernameField.setSize(inputSize);
    usernameField.draw(window);

    // Info text below input (with icon/indicator)
    infoText.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::SmallSize, winSizeF));
    infoText.setPosition(rightStartX, contentY + UIHelpers::scaleFont(230.f, winSizeF));
    window.draw(infoText);

    // Enter Arena button
    sf::Vector2f btnSize = UIHelpers::scaleSize(sf::Vector2f(400.f, UIStyles::Sizes::ButtonHeight), winSizeF);
    sf::Vector2f btnPos(rightStartX, contentY + UIHelpers::scaleFont(290.f, winSizeF));
    btnContinue.setPosition(btnPos);
    btnContinue.getRect().setSize(btnSize);
    btnContinue.getText().setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::BodySize, winSizeF));
    UIHelpers::centerText(btnContinue.getText(), btnPos.x + btnSize.x / 2.f, btnPos.y + btnSize.y / 2.f);
    btnContinue.draw(window);

    // Continue as Guest button (centered below)
    sf::Vector2f guestBtnSize = UIHelpers::scaleSize(sf::Vector2f(200.f, 30.f), winSizeF);
    sf::Vector2f guestBtnPos(rightCenterX - guestBtnSize.x / 2.f, contentY + UIHelpers::scaleFont(360.f, winSizeF));
    btnContinueAsGuest.setPosition(guestBtnPos);
    btnContinueAsGuest.getRect().setSize(guestBtnSize);
    btnContinueAsGuest.getText().setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::CaptionSize, winSizeF));
    UIHelpers::centerText(btnContinueAsGuest.getText(), guestBtnPos.x + guestBtnSize.x / 2.f, guestBtnPos.y + guestBtnSize.y / 2.f);
    btnContinueAsGuest.draw(window);

    // Footer (bottom right)
    sf::Text footer;
    footer.setFont(*screenManager->getFontManager()->getFont(FontType::INTER_REGULAR));
    footer.setString("Privacy     Terms");
    footer.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::SmallSize, winSizeF));
    footer.setFillColor(UIStyles::Colors::TextSecondary);
    footer.setPosition(rightStartX, winSizeF.y - UIHelpers::scaleFont(40.f, winSizeF));
    window.draw(footer);

    // Copyright
    sf::Text copyright;
    copyright.setFont(*screenManager->getFontManager()->getFont(FontType::INTER_REGULAR));
    copyright.setString("© 2023 ChessMaster Inc.");
    copyright.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::SmallSize, winSizeF));
    copyright.setFillColor(UIStyles::Colors::TextSecondary);
    UIHelpers::centerText(copyright, rightCenterX, winSizeF.y - UIHelpers::scaleFont(40.f, winSizeF));
    window.draw(copyright);
}

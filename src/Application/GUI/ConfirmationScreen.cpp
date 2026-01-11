#include "ConfirmationScreen.h"
#include "Services/ScreenManager.h"
#include "UIHelpers.h"
#include "UIStyles.h"
#include <iostream>
#include <cmath>

ConfirmationScreen::ConfirmationScreen(ScreenManager* manager)
    : Screen(manager), animationTime(0.f), animationComplete(false) {

    // Load modern fonts
    sf::Font* titleFont = manager->getFontManager()->getFont(FontType::CINZEL_BOLD);  // Bold pour dramatic effect
    sf::Font* bodyFont = manager->getFontManager()->getFont(FontType::INTER_REGULAR);
    sf::Font* buttonFont = manager->getFontManager()->getFont(FontType::INTER_SEMIBOLD);
    
    // Fallback
    if (!titleFont) titleFont = manager->getFontManager()->getFont(FontType::INTER_REGULAR);
    if (!bodyFont) bodyFont = titleFont;
    if (!buttonFont) buttonFont = bodyFont;

    // Semi-transparent overlay (light)
    overlay.setSize(sf::Vector2f(1200.f, 800.f));
    overlay.setFillColor(sf::Color(245, 247, 250, 200)); // Light overlay

    // Modal background - centered white card
    modalBackground.setSize(sf::Vector2f(480.f, 500.f));
    modalBackground.setFillColor(sf::Color::White);

    // Success icon - layered circles
    iconOuterCircle.setRadius(60.f);
    iconOuterCircle.setFillColor(sf::Color(225, 240, 255)); // Very light blue
    iconOuterCircle.setOrigin(60.f, 60.f);

    iconMiddleCircle.setRadius(50.f);
    iconMiddleCircle.setFillColor(sf::Color(200, 230, 255)); // Light blue
    iconMiddleCircle.setOrigin(50.f, 50.f);

    iconInnerCircle.setRadius(40.f);
    iconInnerCircle.setFillColor(UIStyles::Colors::Primary); // Solid blue
    iconInnerCircle.setOrigin(40.f, 40.f);

    // Checkmark (using Unicode) - Inter SemiBold
    checkmark.setFont(*buttonFont);
    checkmark.setString("✓");
    checkmark.setCharacterSize(48);
    checkmark.setFillColor(sf::Color::White);
    checkmark.setStyle(sf::Text::Bold);

    // Title - "Checkmate! You're in." - Cinzel Bold for dramatic impact
    title.setFont(*titleFont);
    title.setString("Checkmate! You're in.");
    title.setCharacterSize(UIStyles::Typography::SubtitleSize);
    title.setFillColor(UIStyles::Colors::TextPrimary);
    title.setStyle(sf::Text::Bold);

    // Message - Inter Regular
    message.setFont(*bodyFont);
    message.setString("Your account has been successfully created.\nWelcome to the community. Get ready to\nchallenge opponents from around the world.");
    message.setCharacterSize(UIStyles::Typography::BodySize);
    message.setFillColor(UIStyles::Colors::TextSecondary);

    // Enter Lobby button - Inter SemiBold Primary
    btnEnterLobby = Button(
        sf::Vector2f(0, 0),
        sf::Vector2f(400.f, UIStyles::Sizes::ButtonHeight),
        "Enter Lobby →",
        *buttonFont,
        UIStyles::Typography::BodySize
    );
    btnEnterLobby.setStyle(ButtonStyle::Primary);

    // View Profile Settings link - Inter SemiBold
    btnViewProfile = Button(
        sf::Vector2f(0, 0),
        sf::Vector2f(200.f, 30.f),
        "⚙ View Profile Settings",
        *buttonFont,
        UIStyles::Typography::CaptionSize
    );
    btnViewProfile.setStyle(ButtonStyle::Link);
    btnViewProfile.setTextColor(UIStyles::Colors::TextSecondary);

    // Footer links - Inter Regular
    footerLinks.setFont(*bodyFont);
    footerLinks.setString("Privacy Policy     Terms of Service     Support");
    footerLinks.setCharacterSize(UIStyles::Typography::SmallSize);
    footerLinks.setFillColor(UIStyles::Colors::TextSecondary);

    // Copyright - Inter Regular
    copyright.setFont(*bodyFont);
    copyright.setString("© 2024 ChessMaster Inc. All rights reserved.");
    copyright.setCharacterSize(UIStyles::Typography::SmallSize);
    copyright.setFillColor(UIStyles::Colors::TextSecondary);
}

void ConfirmationScreen::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) {
    btnEnterLobby.updateHover(mousePos);
    btnViewProfile.updateHover(mousePos);

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        if (btnEnterLobby.clicked(mousePos)) {
            std::cout << "[ConfirmationScreen] Entering lobby..." << std::endl;
            screenManager->changeState(STATE_HOME);
        }
        else if (btnViewProfile.clicked(mousePos)) {
            std::cout << "[ConfirmationScreen] View profile settings" << std::endl;
            // Could open settings or profile screen
        }
    }
}

void ConfirmationScreen::update(float deltaTime) {
    // Simple pulse animation for success icon
    if (!animationComplete) {
        animationTime += deltaTime;
        if (animationTime >= 1.0f) {
            animationComplete = true;
        }
    }
}

void ConfirmationScreen::draw(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f winSizeF((float)windowSize.x, (float)windowSize.y);

    // Draw overlay
    overlay.setSize(winSizeF);
    window.draw(overlay);

    // Center modal
    sf::Vector2f modalSize = UIHelpers::scaleSize(sf::Vector2f(480.f, 520.f), winSizeF);
    sf::Vector2f modalPos((winSizeF.x - modalSize.x) / 2.f, (winSizeF.y - modalSize.y) / 2.f);
    modalBackground.setSize(modalSize);
    modalBackground.setPosition(modalPos);
    window.draw(modalBackground);

    float centerX = winSizeF.x / 2.f;
    float modalTop = modalPos.y + UIHelpers::scaleFont(UIStyles::Spacing::XXLarge, winSizeF);

    // Success icon with pulse animation
    float iconY = modalTop + UIHelpers::scaleFont(40.f, winSizeF);
    float pulseScale = 1.0f;
    if (!animationComplete) {
        pulseScale = 1.0f + 0.2f * std::sin(animationTime * 6.28f);
    }

    float baseRadius = UIHelpers::scaleFont(40.f, winSizeF);
    
    iconOuterCircle.setRadius(baseRadius * 1.5f * pulseScale);
    iconOuterCircle.setOrigin(baseRadius * 1.5f * pulseScale, baseRadius * 1.5f * pulseScale);
    iconOuterCircle.setPosition(centerX, iconY);
    window.draw(iconOuterCircle);

    iconMiddleCircle.setRadius(baseRadius * 1.25f);
    iconMiddleCircle.setOrigin(baseRadius * 1.25f, baseRadius * 1.25f);
    iconMiddleCircle.setPosition(centerX, iconY);
    window.draw(iconMiddleCircle);

    iconInnerCircle.setRadius(baseRadius);
    iconInnerCircle.setOrigin(baseRadius, baseRadius);
    iconInnerCircle.setPosition(centerX, iconY);
    window.draw(iconInnerCircle);

    // Checkmark
    checkmark.setCharacterSize((unsigned int)UIHelpers::scaleFont(48.f, winSizeF));
    UIHelpers::centerText(checkmark, centerX, iconY);
    window.draw(checkmark);

    // Title
    title.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::SubtitleSize, winSizeF));
    UIHelpers::centerText(title, centerX, iconY + UIHelpers::scaleFont(100.f, winSizeF));
    window.draw(title);

    // Message (centered, multi-line)
    message.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::BodySize, winSizeF));
    UIHelpers::centerText(message, centerX, iconY + UIHelpers::scaleFont(160.f, winSizeF));
    window.draw(message);

    // Enter Lobby button
    sf::Vector2f btnSize = UIHelpers::scaleSize(sf::Vector2f(400.f, UIStyles::Sizes::ButtonHeight), winSizeF);
    sf::Vector2f btnPos((winSizeF.x - btnSize.x) / 2.f, iconY + UIHelpers::scaleFont(270.f, winSizeF));
    btnEnterLobby.setPosition(btnPos);
    btnEnterLobby.getRect().setSize(btnSize);
    btnEnterLobby.getText().setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::BodySize, winSizeF));
    UIHelpers::centerText(btnEnterLobby.getText(), btnPos.x + btnSize.x / 2.f, btnPos.y + btnSize.y / 2.f);
    btnEnterLobby.draw(window);

    // View Profile Settings link
    sf::Vector2f profileBtnSize = UIHelpers::scaleSize(sf::Vector2f(200.f, 30.f), winSizeF);
    sf::Vector2f profileBtnPos((winSizeF.x - profileBtnSize.x) / 2.f, iconY + UIHelpers::scaleFont(340.f, winSizeF));
    btnViewProfile.setPosition(profileBtnPos);
    btnViewProfile.getRect().setSize(profileBtnSize);
    btnViewProfile.getText().setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::CaptionSize, winSizeF));
    UIHelpers::centerText(btnViewProfile.getText(), profileBtnPos.x + profileBtnSize.x / 2.f, profileBtnPos.y + profileBtnSize.y / 2.f);
    btnViewProfile.draw(window);

    // Footer links
    footerLinks.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::SmallSize, winSizeF));
    UIHelpers::centerText(footerLinks, centerX, winSizeF.y - UIHelpers::scaleFont(60.f, winSizeF));
    window.draw(footerLinks);

    // Copyright
    copyright.setCharacterSize((unsigned int)UIHelpers::scaleFont((float)UIStyles::Typography::SmallSize, winSizeF));
    UIHelpers::centerText(copyright, centerX, winSizeF.y - UIHelpers::scaleFont(30.f, winSizeF));
    window.draw(copyright);
}

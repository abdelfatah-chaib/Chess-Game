#include "TitleScreen.h"
#include "Services/ScreenManager.h"
#include "UIHelpers.h"
#include "UIStyles.h"
#include <iostream>

TitleScreen::TitleScreen(ScreenManager* manager) : Screen(manager) {
    std::cout << "[TitleScreen] Initializing..." << std::endl;
    
    // Setup background
    sf::Texture* bgTexture = manager->getTextureManager()->getTexture("bg_title");
    if (bgTexture) {
        std::cout << "[TitleScreen] Background texture loaded successfully" << std::endl;
        background.setTexture(*bgTexture);
    } else {
        std::cout << "[TitleScreen] ERROR: Background texture not found!" << std::endl;
    }
    
    // Setup title text with Cinzel (branding font)
    sf::Font* titleFont = manager->getFontManager()->getFont(FontType::CINZEL_BOLD);
    if (titleFont) {
        std::cout << "[TitleScreen] Cinzel Bold font loaded successfully" << std::endl;
        titleLeft.setFont(*titleFont);
        titleLeft.setString("CHESS");
        titleLeft.setStyle(sf::Text::Bold);
        titleLeft.setFillColor(sf::Color(101, 67, 33));
        
        titleRight.setFont(*titleFont);
        titleRight.setString("MASTER");
        titleRight.setStyle(sf::Text::Bold);
        titleRight.setFillColor(sf::Color::White);
    } else {
        std::cout << "[TitleScreen] WARNING: Cinzel font not found, using fallback" << std::endl;
        // Fallback to main font
        sf::Font* fallbackFont = manager->getFontManager()->getFont("main");
        if (fallbackFont) {
            titleLeft.setFont(*fallbackFont);
            titleRight.setFont(*fallbackFont);
        }
    }
    
    std::cout << "[TitleScreen] Initialization complete" << std::endl;
}

void TitleScreen::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) {
    if (event.type == sf::Event::MouseButtonPressed && 
        event.mouseButton.button == sf::Mouse::Left) {
        std::cout << "[TitleScreen] Click detected, changing to GetStarted screen" << std::endl;
        screenManager->changeState(STATE_GETSTARTED);
    }
}

void TitleScreen::update(float deltaTime) {
    // No update logic needed for title screen
}

void TitleScreen::draw(sf::RenderWindow& window) {
    static bool firstDraw = true;
    if (firstDraw) {
        std::cout << "[TitleScreen] draw() called for the first time" << std::endl;
        std::cout << "[TitleScreen] Window size in draw: " 
                  << window.getSize().x << "x" << window.getSize().y << std::endl;
        firstDraw = false;
    }
    
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f winSizeF((float)windowSize.x, (float)windowSize.y);
    
    // Scale background to fit window
    if (background.getTexture()) {
        UIHelpers::setupBackground(background, *background.getTexture(), true, window);
    }
    window.draw(background);

    // Scale and position title text with professional typography
    float scaledFontSize = UIHelpers::scaleFont((float)UIStyles::Typography::TitleSize, winSizeF);
    
    titleLeft.setCharacterSize((unsigned int)scaledFontSize);
    sf::Vector2f leftPos = UIHelpers::scalePosition(sf::Vector2f(40.f, 30.f), winSizeF);
    titleLeft.setPosition(leftPos);
    
    titleRight.setCharacterSize((unsigned int)scaledFontSize);
    // Position right title aligned to right edge
    sf::FloatRect rightBounds = titleRight.getLocalBounds();
    sf::Vector2f rightPos = UIHelpers::scalePosition(sf::Vector2f(1160.f, 30.f), winSizeF);
    rightPos.x -= rightBounds.width; // Align to right edge
    titleRight.setPosition(rightPos);
    
    window.draw(titleLeft);
    window.draw(titleRight);
}

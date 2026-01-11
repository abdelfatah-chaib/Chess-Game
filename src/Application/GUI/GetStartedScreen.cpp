#include "GetStartedScreen.h"
#include "Services/ScreenManager.h"
#include "UIHelpers.h"
#include "UIStyles.h"

GetStartedScreen::GetStartedScreen(ScreenManager* manager) : Screen(manager) {
    // Setup background
    sf::Texture* bgTexture = manager->getTextureManager()->getTexture("bg_getstarted");
    if (bgTexture) {
        background.setTexture(*bgTexture);
    }

    // Use Inter Medium for emphasis
    sf::Font* mediumFont = manager->getFontManager()->getFont(FontType::INTER_MEDIUM);
    sf::Font* semiBoldFont = manager->getFontManager()->getFont(FontType::INTER_SEMIBOLD);
    
    // Fallback to main if not available
    if (!mediumFont) mediumFont = manager->getFontManager()->getFont(FontType::INTER_MEDIUM);
    if (!semiBoldFont) semiBoldFont = mediumFont;

    // Subtitle - Inter Medium for emphasis
    subtitle.setFont(*mediumFont);
    subtitle.setString("PLAY CHESS, IMPROVE YOUR GAME AND HAVE FUN !");
    subtitle.setCharacterSize(UIStyles::Typography::SubheadingSize);
    subtitle.setFillColor(sf::Color::White);

    // Button Get Started - Inter SemiBold for primary action
    btnGetStarted = Button(
        sf::Vector2f(340.f, 410.f),
        sf::Vector2f(320.f, 56.f),
        "Get Started",
        *semiBoldFont,
        UIStyles::Typography::BodySize
    );
    
    // Button Login - Inter SemiBold for secondary action
    btnLogin = Button(
        sf::Vector2f(340.f, 480.f),
        sf::Vector2f(320.f, 56.f),
        "Login",
        *semiBoldFont,
        UIStyles::Typography::BodySize
    );
    btnLogin.setFillColor(sf::Color(0, 150, 200, 255));
}

void GetStartedScreen::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) {
    btnGetStarted.updateHover(mousePos);
    btnLogin.updateHover(mousePos);

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        if (btnGetStarted.clicked(mousePos)) {
            screenManager->changeState(STATE_CREATE_ACCOUNT);
        }
        else if (btnLogin.clicked(mousePos)) {
            screenManager->changeState(STATE_LOGIN);
        }
    }
}

void GetStartedScreen::update(float deltaTime) {
    // No update logic needed
}

void GetStartedScreen::draw(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f winSizeF((float)windowSize.x, (float)windowSize.y);
    
    // Scale background to fit window
    if (background.getTexture()) {
        UIHelpers::setupBackground(background, *background.getTexture(), true, window);
    }
    window.draw(background);

    // Scale and center subtitle
    float scaledFontSize = UIHelpers::scaleFont(28.f, winSizeF);
    subtitle.setCharacterSize((unsigned int)scaledFontSize);
    UIHelpers::centerText(subtitle, winSizeF.x / 2.f, UIHelpers::scaleFont(30.f, winSizeF));
    window.draw(subtitle);

    // Scale and center Get Started button
    sf::Vector2f btnSize = UIHelpers::scaleSize(sf::Vector2f(320.f, 56.f), winSizeF);
    sf::Vector2f btnGetStartedPos = sf::Vector2f((winSizeF.x - btnSize.x) / 2.f, UIHelpers::scaleFont(380.f, winSizeF));
    
    btnGetStarted.setPosition(btnGetStartedPos);
    btnGetStarted.getRect().setSize(btnSize);
    btnGetStarted.getText().setCharacterSize((unsigned int)UIHelpers::scaleFont(28.f, winSizeF));
    UIHelpers::centerText(btnGetStarted.getText(), btnGetStartedPos.x + btnSize.x / 2.f, btnGetStartedPos.y + btnSize.y / 2.f);
    
    btnGetStarted.draw(window);
    
    // Scale and center Login button
    sf::Vector2f btnLoginPos = sf::Vector2f((winSizeF.x - btnSize.x) / 2.f, UIHelpers::scaleFont(450.f, winSizeF));
    
    btnLogin.setPosition(btnLoginPos);
    btnLogin.getRect().setSize(btnSize);
    btnLogin.getText().setCharacterSize((unsigned int)UIHelpers::scaleFont(28.f, winSizeF));
    UIHelpers::centerText(btnLogin.getText(), btnLoginPos.x + btnSize.x / 2.f, btnLoginPos.y + btnSize.y / 2.f);
    
    btnLogin.draw(window);
}

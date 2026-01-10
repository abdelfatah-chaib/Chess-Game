#pragma once
#include "Screen.h"
#include "Button.h"
#include <SFML/Graphics.hpp>

class GetStartedScreen : public Screen {
private:
    sf::Sprite background;
    sf::Text subtitle;
    Button btnGetStarted;
    Button btnLogin;

public:
    GetStartedScreen(ScreenManager* manager);
    
    void handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    AppState getStateType() const override { return STATE_GETSTARTED; }
};

#pragma once
#include "Screen.h"
#include <SFML/Graphics.hpp>

class TitleScreen : public Screen {
private:
    sf::Sprite background;
    sf::Text titleLeft;
    sf::Text titleRight;

public:
    TitleScreen(ScreenManager* manager);
    
    void handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) override;
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    AppState getStateType() const override { return STATE_TITLE; }
};

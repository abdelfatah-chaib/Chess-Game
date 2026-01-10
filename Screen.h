#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.h"

class ScreenManager;

class Screen {
protected:
    ScreenManager* screenManager;

public:
    Screen(ScreenManager* manager) : screenManager(manager) {}
    virtual ~Screen() = default;

    virtual void handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual AppState getStateType() const = 0;
};

#pragma once
#include <SFML/Graphics.hpp>

namespace UIHelpers {
    // Center text at specified position
    void centerText(sf::Text& text, float x, float y);

    // Setup background sprite to fill window (scalable)
    void setupBackground(sf::Sprite& sprite, const sf::Texture& texture, bool loaded, sf::RenderWindow& window);
    
    // Scale-aware positioning helpers
    sf::Vector2f scalePosition(const sf::Vector2f& basePos, const sf::Vector2f& windowSize, const sf::Vector2f& baseWindowSize = sf::Vector2f(1200.f, 800.f));
    sf::Vector2f scaleSize(const sf::Vector2f& baseSize, const sf::Vector2f& windowSize, const sf::Vector2f& baseWindowSize = sf::Vector2f(1200.f, 800.f));
    float scaleFont(float baseSize, const sf::Vector2f& windowSize, const sf::Vector2f& baseWindowSize = sf::Vector2f(1200.f, 800.f));
}

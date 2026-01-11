#include "UIHelpers.h"
#include <algorithm>

namespace UIHelpers {
    void centerText(sf::Text& text, float x, float y) {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        text.setPosition(x, y);
    }

    void setupBackground(sf::Sprite& sprite, const sf::Texture& texture, bool loaded, sf::RenderWindow& window) {
        if (loaded) {
            sf::Vector2u windowSize = window.getSize();
            sprite.setScale(
                (float)windowSize.x / texture.getSize().x, 
                (float)windowSize.y / texture.getSize().y
            );
        }
    }

    sf::Vector2f scalePosition(const sf::Vector2f& basePos, const sf::Vector2f& windowSize, const sf::Vector2f& baseWindowSize) {
        float scaleX = windowSize.x / baseWindowSize.x;
        float scaleY = windowSize.y / baseWindowSize.y;
        return sf::Vector2f(basePos.x * scaleX, basePos.y * scaleY);
    }

    sf::Vector2f scaleSize(const sf::Vector2f& baseSize, const sf::Vector2f& windowSize, const sf::Vector2f& baseWindowSize) {
        float scaleX = windowSize.x / baseWindowSize.x;
        float scaleY = windowSize.y / baseWindowSize.y;
        // Use the minimum scale to maintain aspect ratio
        float scale = std::min(scaleX, scaleY);
        return sf::Vector2f(baseSize.x * scale, baseSize.y * scale);
    }

    float scaleFont(float baseSize, const sf::Vector2f& windowSize, const sf::Vector2f& baseWindowSize) {
        float scaleX = windowSize.x / baseWindowSize.x;
        float scaleY = windowSize.y / baseWindowSize.y;
        float scale = std::min(scaleX, scaleY);
        return baseSize * scale;
    }
}

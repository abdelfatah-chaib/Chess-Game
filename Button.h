#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class ButtonStyle {
    Primary,      // Blue filled
    Secondary,    // White with border
    OAuth,        // OAuth buttons (Google, Apple)
    Link          // Text only
};

enum class ButtonIcon {
    None,
    Google,
    Apple,
    Email,
    Lock
};

class Button {
private:
    sf::RectangleShape rect;
    sf::Text text;
    sf::Color idle;
    sf::Color hover;
    sf::Color active;
    bool enabled;
    bool isHovered;
    ButtonStyle style;
    ButtonIcon icon;
    sf::Sprite iconSprite;
    
    void updateColors();

public:
    Button();
    Button(const sf::Vector2f& pos, const sf::Vector2f& size, 
           const std::string& label, sf::Font& font, unsigned charSize = 28);

    void draw(sf::RenderWindow& w);
    void updateHover(const sf::Vector2i& m);
    bool clicked(const sf::Vector2i& m) const;

    // Setters for customization
    void setFillColor(const sf::Color& color);
    void setTextColor(const sf::Color& color);
    void setEnabled(bool enable);
    void setPosition(const sf::Vector2f& pos);
    void setStyle(ButtonStyle buttonStyle);
    void setIcon(ButtonIcon buttonIcon, sf::Texture* iconTexture = nullptr);
    
    // Getters
    sf::RectangleShape& getRect() { return rect; }
    sf::Text& getText() { return text; }
    const sf::Vector2f& getPosition() const { return rect.getPosition(); }
    const sf::Vector2f& getSize() const { return rect.getSize(); }
    bool getIsHovered() const { return isHovered; }
};

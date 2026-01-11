#include "Button.h"
#include "UIHelpers.h"
#include "UIStyles.h"

Button::Button() : enabled(true), isHovered(false), style(ButtonStyle::Primary), icon(ButtonIcon::None) {}

Button::Button(const sf::Vector2f& pos, const sf::Vector2f& size,
    const std::string& label, sf::Font& font, unsigned charSize) 
    : enabled(true), isHovered(false), style(ButtonStyle::Primary), icon(ButtonIcon::None) {
    
    rect.setPosition(pos);
    rect.setSize(size);
    
    // Modern default colors
    idle = UIStyles::Colors::Primary;
    hover = UIStyles::Colors::PrimaryHover;
    active = UIStyles::Colors::PrimaryActive;
    
    rect.setFillColor(idle);
    rect.setOutlineThickness(0);

    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(charSize);
    text.setFillColor(sf::Color::White);
    UIHelpers::centerText(text, pos.x + size.x / 2.f, pos.y + size.y / 2.f);
}

void Button::updateColors() {
    switch (style) {
        case ButtonStyle::Primary:
            idle = UIStyles::Colors::Primary;
            hover = UIStyles::Colors::PrimaryHover;
            active = UIStyles::Colors::PrimaryActive;
            text.setFillColor(sf::Color::White);
            rect.setOutlineThickness(0);
            break;
            
        case ButtonStyle::Secondary:
            idle = sf::Color::White;
            hover = UIStyles::Colors::Background;
            active = sf::Color(230, 230, 230);
            text.setFillColor(UIStyles::Colors::TextPrimary);
            rect.setOutlineThickness(2.f);
            rect.setOutlineColor(UIStyles::Colors::BorderDefault);
            break;
            
        case ButtonStyle::OAuth:
            idle = sf::Color::White;
            hover = UIStyles::Colors::Background;
            active = sf::Color(240, 240, 240);
            text.setFillColor(UIStyles::Colors::TextPrimary);
            rect.setOutlineThickness(1.f);
            rect.setOutlineColor(UIStyles::Colors::GoogleBorder);
            break;
            
        case ButtonStyle::Link:
            idle = sf::Color::Transparent;
            hover = sf::Color(0, 0, 0, 10);
            active = sf::Color(0, 0, 0, 20);
            text.setFillColor(UIStyles::Colors::Primary);
            rect.setOutlineThickness(0);
            break;
    }
}

void Button::draw(sf::RenderWindow& w) {
    if (!enabled) {
        rect.setFillColor(sf::Color(200, 200, 200));
        text.setFillColor(sf::Color(150, 150, 150));
    } else {
        rect.setFillColor(isHovered ? hover : idle);
    }
    
    w.draw(rect);
    
    // Draw icon if present
    if (icon != ButtonIcon::None && iconSprite.getTexture()) {
        w.draw(iconSprite);
    }
    
    w.draw(text);
}

void Button::updateHover(const sf::Vector2i& m) {
    if (!enabled) {
        isHovered = false;
        return;
    }

    isHovered = rect.getGlobalBounds().contains((float)m.x, (float)m.y);
    
    if (style == ButtonStyle::Secondary || style == ButtonStyle::OAuth) {
        if (isHovered) {
            rect.setOutlineColor(UIStyles::Colors::Primary);
        } else {
            rect.setOutlineColor(style == ButtonStyle::Secondary ? 
                UIStyles::Colors::BorderDefault : UIStyles::Colors::GoogleBorder);
        }
    }
}

bool Button::clicked(const sf::Vector2i& m) const {
    return enabled && rect.getGlobalBounds().contains((float)m.x, (float)m.y);
}

void Button::setFillColor(const sf::Color& color) {
    idle = color;
    // Calculate hover color (slightly darker)
    hover = sf::Color(
        static_cast<sf::Uint8>(color.r * 0.85f),
        static_cast<sf::Uint8>(color.g * 0.85f),
        static_cast<sf::Uint8>(color.b * 0.85f),
        color.a
    );
    active = sf::Color(
        static_cast<sf::Uint8>(color.r * 0.7f),
        static_cast<sf::Uint8>(color.g * 0.7f),
        static_cast<sf::Uint8>(color.b * 0.7f),
        color.a
    );
    rect.setFillColor(idle);
}

void Button::setTextColor(const sf::Color& color) {
    text.setFillColor(color);
}

void Button::setEnabled(bool enable) {
    enabled = enable;
}

void Button::setPosition(const sf::Vector2f& pos) {
    rect.setPosition(pos);
    
    // Update icon position if present
    if (icon != ButtonIcon::None && iconSprite.getTexture()) {
        float iconX = pos.x + UIStyles::Spacing::Medium;
        float iconY = pos.y + (rect.getSize().y - UIStyles::Sizes::IconSize) / 2.f;
        iconSprite.setPosition(iconX, iconY);
        
        // Adjust text position to account for icon
        float textX = pos.x + rect.getSize().x / 2.f + UIStyles::Spacing::Medium;
        UIHelpers::centerText(text, textX, pos.y + rect.getSize().y / 2.f);
    } else {
        UIHelpers::centerText(text, pos.x + rect.getSize().x / 2.f, pos.y + rect.getSize().y / 2.f);
    }
}

void Button::setStyle(ButtonStyle buttonStyle) {
    style = buttonStyle;
    updateColors();
    rect.setFillColor(idle);
}

void Button::setIcon(ButtonIcon buttonIcon, sf::Texture* iconTexture) {
    icon = buttonIcon;
    if (iconTexture && icon != ButtonIcon::None) {
        iconSprite.setTexture(*iconTexture);
        
        // Scale icon to appropriate size
        sf::Vector2u texSize = iconTexture->getSize();
        float scale = UIStyles::Sizes::IconSize / static_cast<float>(texSize.x);
        iconSprite.setScale(scale, scale);
        
        // Position will be updated in setPosition
    }
}

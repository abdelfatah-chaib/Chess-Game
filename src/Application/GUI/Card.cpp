#include "Card.h"
#include "UIStyles.h"
#include "UIHelpers.h"

Card::Card() : isHovered(false) {}

Card::Card(const sf::Vector2f& pos, const sf::Vector2f& cardSize, 
           const std::string& titleText, const std::string& descText, 
           sf::Font& font, const sf::Color& iconColor) 
    : position(pos), size(cardSize), isHovered(false) {
    
    // Card background
    background.setPosition(pos);
    background.setSize(cardSize);
    background.setFillColor(sf::Color::White);
    background.setOutlineThickness(0);
    
    // Icon (centered at top)
    icon.setRadius(24.f);
    icon.setFillColor(iconColor);
    icon.setPosition(pos.x + cardSize.x / 2.f - 24.f, pos.y + UIStyles::Spacing::Large);
    
    // Title
    title.setFont(font);
    title.setString(titleText);
    title.setCharacterSize(UIStyles::Typography::BodySize);
    title.setFillColor(UIStyles::Colors::TextPrimary);
    title.setStyle(sf::Text::Bold);
    
    // Center title horizontally
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition(
        pos.x + (cardSize.x - titleBounds.width) / 2.f,
        pos.y + UIStyles::Spacing::Large + 48.f + UIStyles::Spacing::Small
    );
    
    // Description
    description.setFont(font);
    description.setString(descText);
    description.setCharacterSize(UIStyles::Typography::CaptionSize);
    description.setFillColor(UIStyles::Colors::TextSecondary);
    
    // Center description horizontally
    sf::FloatRect descBounds = description.getLocalBounds();
    description.setPosition(
        pos.x + (cardSize.x - descBounds.width) / 2.f,
        pos.y + UIStyles::Spacing::Large + 48.f + UIStyles::Spacing::Medium + UIStyles::Typography::BodySize
    );
}

void Card::draw(sf::RenderWindow& window) {
    // Apply hover effect
    if (isHovered) {
        background.setOutlineThickness(2.f);
        background.setOutlineColor(UIStyles::Colors::Primary);
    } else {
        background.setOutlineThickness(1.f);
        background.setOutlineColor(sf::Color(230, 230, 230));
    }
    
    window.draw(background);
    window.draw(icon);
    window.draw(title);
    window.draw(description);
}

void Card::updateHover(const sf::Vector2i& mousePos) {
    isHovered = background.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y);
}

bool Card::clicked(const sf::Vector2i& mousePos) const {
    return background.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y);
}

void Card::setPosition(const sf::Vector2f& pos) {
    position = pos;
    background.setPosition(pos);
    
    // Update icon position
    icon.setPosition(pos.x + size.x / 2.f - 24.f, pos.y + UIStyles::Spacing::Large);
    
    // Update text positions
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition(
        pos.x + (size.x - titleBounds.width) / 2.f,
        pos.y + UIStyles::Spacing::Large + 48.f + UIStyles::Spacing::Small
    );
    
    sf::FloatRect descBounds = description.getLocalBounds();
    description.setPosition(
        pos.x + (size.x - descBounds.width) / 2.f,
        pos.y + UIStyles::Spacing::Large + 48.f + UIStyles::Spacing::Medium + UIStyles::Typography::BodySize
    );
}

void Card::setSize(const sf::Vector2f& cardSize) {
    size = cardSize;
    background.setSize(cardSize);
    
    // Recenter elements
    setPosition(position);
}

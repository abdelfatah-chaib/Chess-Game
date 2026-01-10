#include "Checkbox.h"

Checkbox::Checkbox() : checked(false), hovered(false) {
    box.setSize(sf::Vector2f(20.f, 20.f));
    box.setFillColor(sf::Color(200, 200, 200, 100));
    box.setOutlineThickness(2.f);
    box.setOutlineColor(sf::Color::White);
    
    checkmark.setSize(sf::Vector2f(12.f, 12.f));
    checkmark.setFillColor(sf::Color(0, 255, 0));
}

Checkbox::Checkbox(const sf::Vector2f& pos, const std::string& labelText, sf::Font& font)
    : Checkbox() {
    
    box.setPosition(pos);
    checkmark.setPosition(pos.x + 4.f, pos.y + 4.f);
    
    label.setFont(font);
    label.setString(labelText);
    label.setCharacterSize(18);
    label.setFillColor(sf::Color::White);
    label.setPosition(pos.x + 30.f, pos.y - 2.f);
}

void Checkbox::draw(sf::RenderWindow& window) {
    // Update colors based on hover
    if (hovered) {
        box.setOutlineColor(sf::Color::Yellow);
    } else {
        box.setOutlineColor(sf::Color::White);
    }
    
    window.draw(box);
    
    if (checked) {
        window.draw(checkmark);
    }
    
    window.draw(label);
}

void Checkbox::updateHover(const sf::Vector2i& mousePos) {
    sf::FloatRect bounds = box.getGlobalBounds();
    hovered = bounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
}

bool Checkbox::clicked(const sf::Vector2i& mousePos) {
    sf::FloatRect bounds = box.getGlobalBounds();
    if (bounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
        checked = !checked;
        return true;
    }
    return false;
}

void Checkbox::setPosition(const sf::Vector2f& pos) {
    box.setPosition(pos);
    checkmark.setPosition(pos.x + 4.f, pos.y + 4.f);
    label.setPosition(pos.x + 30.f, pos.y - 2.f);
}

void Checkbox::setLabelSize(unsigned int size) {
    label.setCharacterSize(size);
}

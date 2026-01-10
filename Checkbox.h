#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Checkbox {
private:
    sf::RectangleShape box;
    sf::RectangleShape checkmark;
    sf::Text label;
    bool checked;
    bool hovered;

public:
    Checkbox();
    Checkbox(const sf::Vector2f& pos, const std::string& labelText, sf::Font& font);

    void draw(sf::RenderWindow& window);
    void updateHover(const sf::Vector2i& mousePos);
    bool clicked(const sf::Vector2i& mousePos);
    
    bool isChecked() const { return checked; }
    void setChecked(bool value) { checked = value; }
    
    void setPosition(const sf::Vector2f& pos);
    void setLabelSize(unsigned int size);
    
    sf::FloatRect getBounds() const { return box.getGlobalBounds(); }
};

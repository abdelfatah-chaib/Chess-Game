#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Card {
private:
    sf::RectangleShape background;
    sf::CircleShape icon;
    sf::Text title;
    sf::Text description;
    bool isHovered;
    
    sf::Vector2f position;
    sf::Vector2f size;

public:
    Card();
    Card(const sf::Vector2f& pos, const sf::Vector2f& cardSize, 
         const std::string& titleText, const std::string& descText, 
         sf::Font& font, const sf::Color& iconColor = sf::Color::Blue);
    
    void draw(sf::RenderWindow& window);
    void updateHover(const sf::Vector2i& mousePos);
    bool clicked(const sf::Vector2i& mousePos) const;
    
    void setPosition(const sf::Vector2f& pos);
    void setSize(const sf::Vector2f& cardSize);
    
    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getSize() const { return size; }
    bool getIsHovered() const { return isHovered; }
};

#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>

enum class SidebarItem {
    Home,
    Play,
    Puzzles,
    Learn,
    Watch,
    Settings,
    Logout
};

class Sidebar {
private:
    sf::RectangleShape background;
    sf::Text logo;
    sf::Sprite logoIcon;
    
    struct MenuItem {
        SidebarItem id;
        sf::Text text;
        sf::RectangleShape background;
        sf::CircleShape icon; // Simplified icon (will use shapes/text)
        bool isActive;
        bool isHovered;
    };
    
    std::vector<MenuItem> menuItems;
    SidebarItem currentActive;
    
    float width;
    float height;
    sf::Vector2f position;
    
    void updateItemPositions();
    void updateItemColors(MenuItem& item);

public:
    Sidebar(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Font& font);
    
    void draw(sf::RenderWindow& window);
    void updateHover(const sf::Vector2i& mousePos);
    SidebarItem handleClick(const sf::Vector2i& mousePos);
    
    void setActiveItem(SidebarItem item);
    SidebarItem getActiveItem() const { return currentActive; }
    
    float getWidth() const { return width; }
    sf::Vector2f getPosition() const { return position; }
};

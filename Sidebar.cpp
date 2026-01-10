#include "Sidebar.h"
#include "UIStyles.h"
#include "UIHelpers.h"

Sidebar::Sidebar(const sf::Vector2f& pos, const sf::Vector2f& size, sf::Font& font) 
    : position(pos), width(size.x), height(size.y), currentActive(SidebarItem::Home) {
    
    // Background
    background.setPosition(pos);
    background.setSize(size);
    background.setFillColor(sf::Color::White);
    
    // Logo text
    logo.setFont(font);
    logo.setString("ChessMaster");
    logo.setCharacterSize(18);
    logo.setFillColor(UIStyles::Colors::TextPrimary);
    logo.setStyle(sf::Text::Bold);
    logo.setPosition(pos.x + 50.f, pos.y + 20.f);
    
    // Create menu items
    std::vector<std::pair<SidebarItem, std::string>> items = {
        {SidebarItem::Home, "Home"},
        {SidebarItem::Play, "Play"},
        {SidebarItem::Puzzles, "Puzzles"},
        {SidebarItem::Learn, "Learn"},
        {SidebarItem::Watch, "Watch"}
    };
    
    float startY = pos.y + 80.f;
    float itemHeight = 48.f;
    float spacing = 4.f;
    
    for (size_t i = 0; i < items.size(); ++i) {
        MenuItem menuItem;
        menuItem.id = items[i].first;
        menuItem.isActive = (items[i].first == SidebarItem::Home);
        menuItem.isHovered = false;
        
        // Background
        menuItem.background.setSize(sf::Vector2f(width - 16.f, itemHeight));
        menuItem.background.setPosition(pos.x + 8.f, startY + i * (itemHeight + spacing));
        
        // Text
        menuItem.text.setFont(font);
        menuItem.text.setString(items[i].second);
        menuItem.text.setCharacterSize(16);
        menuItem.text.setPosition(pos.x + 50.f, startY + i * (itemHeight + spacing) + 14.f);
        
        // Icon placeholder (simple circle for now)
        menuItem.icon.setRadius(10.f);
        menuItem.icon.setPosition(pos.x + 20.f, startY + i * (itemHeight + spacing) + 14.f);
        
        updateItemColors(menuItem);
        menuItems.push_back(menuItem);
    }
    
    // Bottom items (Settings, Logout)
    std::vector<std::pair<SidebarItem, std::string>> bottomItems = {
        {SidebarItem::Settings, "Settings"},
        {SidebarItem::Logout, "Logout"}
    };
    
    float bottomY = pos.y + height - 120.f;
    
    for (size_t i = 0; i < bottomItems.size(); ++i) {
        MenuItem menuItem;
        menuItem.id = bottomItems[i].first;
        menuItem.isActive = false;
        menuItem.isHovered = false;
        
        // Background
        menuItem.background.setSize(sf::Vector2f(width - 16.f, itemHeight));
        menuItem.background.setPosition(pos.x + 8.f, bottomY + i * (itemHeight + spacing));
        
        // Text
        menuItem.text.setFont(font);
        menuItem.text.setString(bottomItems[i].second);
        menuItem.text.setCharacterSize(16);
        menuItem.text.setPosition(pos.x + 50.f, bottomY + i * (itemHeight + spacing) + 14.f);
        
        // Icon placeholder
        menuItem.icon.setRadius(10.f);
        menuItem.icon.setPosition(pos.x + 20.f, bottomY + i * (itemHeight + spacing) + 14.f);
        
        updateItemColors(menuItem);
        menuItems.push_back(menuItem);
    }
}

void Sidebar::updateItemColors(MenuItem& item) {
    if (item.isActive) {
        item.background.setFillColor(sf::Color(230, 240, 255)); // Light blue
        item.text.setFillColor(UIStyles::Colors::Primary);
        item.icon.setFillColor(UIStyles::Colors::Primary);
    } else if (item.isHovered) {
        item.background.setFillColor(sf::Color(245, 245, 245)); // Light gray
        item.text.setFillColor(UIStyles::Colors::TextPrimary);
        item.icon.setFillColor(UIStyles::Colors::TextSecondary);
    } else {
        item.background.setFillColor(sf::Color::Transparent);
        item.text.setFillColor(UIStyles::Colors::TextSecondary);
        item.icon.setFillColor(UIStyles::Colors::TextSecondary);
    }
}

void Sidebar::draw(sf::RenderWindow& window) {
    // Draw background
    window.draw(background);
    
    // Draw logo
    window.draw(logo);
    
    // Draw menu items
    for (auto& item : menuItems) {
        window.draw(item.background);
        window.draw(item.icon);
        window.draw(item.text);
    }
}

void Sidebar::updateHover(const sf::Vector2i& mousePos) {
    for (auto& item : menuItems) {
        bool wasHovered = item.isHovered;
        item.isHovered = item.background.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y);
        
        if (item.isHovered != wasHovered) {
            updateItemColors(item);
        }
    }
}

SidebarItem Sidebar::handleClick(const sf::Vector2i& mousePos) {
    for (auto& item : menuItems) {
        if (item.background.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)) {
            // Don't change active state for Settings/Logout
            if (item.id != SidebarItem::Settings && item.id != SidebarItem::Logout) {
                setActiveItem(item.id);
            }
            return item.id;
        }
    }
    return currentActive;
}

void Sidebar::setActiveItem(SidebarItem item) {
    currentActive = item;
    
    for (auto& menuItem : menuItems) {
        bool wasActive = menuItem.isActive;
        menuItem.isActive = (menuItem.id == item);
        
        if (menuItem.isActive != wasActive) {
            updateItemColors(menuItem);
        }
    }
}

void Sidebar::updateItemPositions() {
    // Called if sidebar needs to be resized/repositioned
}

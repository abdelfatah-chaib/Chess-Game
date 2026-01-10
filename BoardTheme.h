#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class BoardTheme {
    Wooden,
    RedWine,
    BlueSky
};

struct ThemeColors {
    sf::Color BG;
    sf::Color FRAME;
    sf::Color BORDER;
    sf::Color LIGHT;
    sf::Color DARK;
    
    ThemeColors() = default;
    ThemeColors(sf::Color bg, sf::Color frame, sf::Color border, sf::Color light, sf::Color dark)
        : BG(bg), FRAME(frame), BORDER(border), LIGHT(light), DARK(dark) {}
};

class BoardThemeManager {
private:
    static BoardTheme currentTheme;
    
public:
    static ThemeColors getThemeColors(BoardTheme theme);
    static ThemeColors getCurrentThemeColors();
    static void setCurrentTheme(BoardTheme theme);
    static BoardTheme getCurrentTheme();
    static std::string getThemeName(BoardTheme theme);
};

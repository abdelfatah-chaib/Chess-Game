#include "BoardTheme.h"

BoardTheme BoardThemeManager::currentTheme = BoardTheme::Wooden;

ThemeColors BoardThemeManager::getThemeColors(BoardTheme theme) {
    switch (theme) {
        case BoardTheme::Wooden:
            return ThemeColors(
                sf::Color(18, 14, 12),      // BG
                sf::Color(43, 28, 23),      // FRAME
                sf::Color(107, 70, 54),     // BORDER
                sf::Color(238, 210, 168),   // LIGHT
                sf::Color(139, 90, 60)      // DARK
            );
            
        case BoardTheme::RedWine:
            return ThemeColors(
                sf::Color(26, 15, 18),      // BG
                sf::Color(75, 30, 37),      // FRAME
                sf::Color(122, 58, 63),     // BORDER
                sf::Color(242, 232, 213),   // LIGHT
                sf::Color(123, 45, 51)      // DARK
            );
            
        case BoardTheme::BlueSky:
            return ThemeColors(
                sf::Color(46, 59, 78),      // BG
                sf::Color(63, 81, 104),     // FRAME
                sf::Color(95, 118, 147),    // BORDER
                sf::Color(230, 238, 246),   // LIGHT
                sf::Color(116, 142, 172)    // DARK
            );
            
        default:
            return getThemeColors(BoardTheme::Wooden);
    }
}

ThemeColors BoardThemeManager::getCurrentThemeColors() {
    return getThemeColors(currentTheme);
}

void BoardThemeManager::setCurrentTheme(BoardTheme theme) {
    currentTheme = theme;
}

BoardTheme BoardThemeManager::getCurrentTheme() {
    return currentTheme;
}

std::string BoardThemeManager::getThemeName(BoardTheme theme) {
    switch (theme) {
        case BoardTheme::Wooden:
            return "Wooden";
        case BoardTheme::RedWine:
            return "Red Wine";
        case BoardTheme::BlueSky:
            return "Blue Sky";
        default:
            return "Unknown";
    }
}

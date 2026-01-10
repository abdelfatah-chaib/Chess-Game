#include "FontManager.h"
#include <iostream>

FontManager::FontManager(const std::string& assetPath) : basePath(assetPath) {}

bool FontManager::loadFont(const std::string& name, const std::string& filename) {
    sf::Font font;
    std::string fullPath = basePath + filename;
    
    if (font.loadFromFile(fullPath)) {
        fonts[name] = font;
        std::cout << "[FontManager] ✓ Loaded font: " << name << std::endl;
        return true;
    }
    
    std::cerr << "[FontManager] ✗ Failed to load font: " << fullPath << std::endl;
    return false;
}

sf::Font* FontManager::getFont(const std::string& name) {
    auto it = fonts.find(name);
    if (it != fonts.end()) {
        return &(it->second);
    }
    
    // Try fallback to inter-regular (modern default)
    it = fonts.find("inter-regular");
    if (it != fonts.end()) {
        std::cout << "[FontManager] ⚠ Font '" << name << "' not found, using fallback 'inter-regular'" << std::endl;
        return &(it->second);
    }
    
    std::cerr << "[FontManager] ✗ Font not found: " << name << std::endl;
    return nullptr;
}

sf::Font* FontManager::getFont(FontType type) {
    std::string name = fontTypeToString(type);
    return getFont(name);
}

bool FontManager::hasFont(const std::string& name) const {
    return fonts.find(name) != fonts.end();
}

std::string FontManager::fontTypeToString(FontType type) {
    switch (type) {
        case FontType::INTER_REGULAR:   return "inter-regular";
        case FontType::INTER_MEDIUM:    return "inter-medium";
        case FontType::INTER_SEMIBOLD:  return "inter-semibold";
        case FontType::CINZEL_REGULAR:  return "cinzel-regular";
        case FontType::CINZEL_BOLD:     return "cinzel-bold";
        case FontType::JETBRAINS_MONO:  return "jetbrains-mono";
        case FontType::MAIN:            return "main";
        default:                        return "main";
    }
}

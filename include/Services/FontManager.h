#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <map>

// Typography Font Types for Chess UI
enum class FontType {
    // Inter (Primary UI Font)
    INTER_REGULAR,
    INTER_MEDIUM,
    INTER_SEMIBOLD,
    
    // Cinzel (Branding & Titles)
    CINZEL_REGULAR,
    CINZEL_BOLD,
    
    // JetBrains Mono (Monospace for numbers/code)
    JETBRAINS_MONO,
    
    // Legacy fallback
    MAIN
};

class FontManager {
private:
    std::map<std::string, sf::Font> fonts;
    std::string basePath;

public:
    FontManager(const std::string& assetPath = "");

    bool loadFont(const std::string& name, const std::string& filename);
    sf::Font* getFont(const std::string& name);
    sf::Font* getFont(FontType type);
    bool hasFont(const std::string& name) const;
    
    void setBasePath(const std::string& path) { basePath = path; }
    const std::string& getBasePath() const { return basePath; }
    
    // Helper to convert FontType to string key
    static std::string fontTypeToString(FontType type);
};

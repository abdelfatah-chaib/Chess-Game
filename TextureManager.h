#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <map>

class TextureManager {
private:
    std::map<std::string, sf::Texture> textures;
    std::string basePath;

public:
    TextureManager(const std::string& assetPath = "");

    bool loadTexture(const std::string& name, const std::string& filename);
    sf::Texture* getTexture(const std::string& name);
    bool hasTexture(const std::string& name) const;
    
    void setBasePath(const std::string& path) { basePath = path; }
    const std::string& getBasePath() const { return basePath; }
};

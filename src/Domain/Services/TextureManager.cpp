#include "Services/TextureManager.h"
#include <iostream>

TextureManager::TextureManager(const std::string& assetPath) : basePath(assetPath) {}

bool TextureManager::loadTexture(const std::string& name, const std::string& filename) {
    sf::Texture texture;
    std::string fullPath = basePath + filename;
    
    if (texture.loadFromFile(fullPath)) {
        textures[name] = texture;
        return true;
    }
    
    std::cout << "Failed to load texture: " << fullPath << std::endl;
    return false;
}

sf::Texture* TextureManager::getTexture(const std::string& name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        return &(it->second);
    }
    return nullptr;
}

bool TextureManager::hasTexture(const std::string& name) const {
    return textures.find(name) != textures.end();
}

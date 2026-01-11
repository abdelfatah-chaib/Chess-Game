#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class InputIcon {
    None,
    Email,
    Lock,
    User,
    Search
};

class InputField {
private:
    sf::RectangleShape rect;
    sf::Text label;
    sf::Text inputText;
    sf::Text placeholder;
    sf::Text errorText;
    sf::Sprite iconSprite;
    sf::Sprite toggleVisibilitySprite;
    bool isActive;
    bool isFocused;
    std::string content;
    std::string fieldName;
    bool hasError;
    InputIcon icon;
    bool isPasswordField;
    bool passwordVisible;
    
    void updateTextPosition();

public:
    InputField() = default;
    InputField(const sf::Vector2f& pos, const sf::Vector2f& size, 
               const std::string& fieldLabel, sf::Font& font);

    void draw(sf::RenderWindow& w);
    void updateHover(const sf::Vector2i& m);
    bool clicked(const sf::Vector2i& m);
    void handleTextInput(sf::Uint32 unicode);

    bool validateEmail();
    bool validateUsername();

    // Getters
    const std::string& getContent() const { return content; }
    bool getIsActive() const { return isActive; }
    
    // Setters
    void setActive(bool active);
    void setError(const std::string& message);
    void clearError();
    void setPosition(const sf::Vector2f& pos);
    void setSize(const sf::Vector2f& size);
    void setIcon(InputIcon inputIcon, sf::Texture* iconTexture = nullptr);
    void setIsPasswordField(bool isPassword);
    void togglePasswordVisibility();
};

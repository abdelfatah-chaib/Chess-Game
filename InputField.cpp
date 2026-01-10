#include "InputField.h"
#include "UIStyles.h"
#include <iostream>

InputField::InputField(const sf::Vector2f& pos, const sf::Vector2f& size,
                       const std::string& fieldLabel, sf::Font& font)
    : isActive(false), isFocused(false), hasError(false),
      icon(InputIcon::None), isPasswordField(false), passwordVisible(false) {
    
    rect.setPosition(pos);
    rect.setSize(size);
    rect.setFillColor(sf::Color::White);
    rect.setOutlineThickness(2.f);
    rect.setOutlineColor(UIStyles::Colors::BorderDefault);

    label.setFont(font);
    label.setString(fieldLabel);
    label.setCharacterSize(UIStyles::Typography::CaptionSize);
    label.setFillColor(UIStyles::Colors::TextPrimary);
    label.setPosition(pos.x, pos.y - 25);

    inputText.setFont(font);
    inputText.setString("");
    inputText.setCharacterSize(UIStyles::Typography::BodySize);
    inputText.setFillColor(UIStyles::Colors::TextPrimary);
    
    placeholder.setFont(font);
    placeholder.setString(fieldLabel);
    placeholder.setCharacterSize(UIStyles::Typography::BodySize);
    placeholder.setFillColor(UIStyles::Colors::TextPlaceholder);

    errorText.setFont(font);
    errorText.setString("");
    errorText.setCharacterSize(UIStyles::Typography::SmallSize);
    errorText.setFillColor(UIStyles::Colors::TextError);
    errorText.setPosition(pos.x, pos.y + size.y + 5);

    content = "";
    fieldName = fieldLabel;
    
    updateTextPosition();
}

void InputField::updateTextPosition() {
    sf::Vector2f pos = rect.getPosition();
    sf::Vector2f size = rect.getSize();
    
    // Calculate left padding (base + icon space if present)
    float leftPadding = UIStyles::Spacing::Medium;
    if (icon != InputIcon::None) {
        leftPadding += UIStyles::Sizes::IconSize + UIStyles::Spacing::Small;
    }
    
    float textY = pos.y + (size.y - UIStyles::Typography::BodySize) / 2.f;
    
    inputText.setPosition(pos.x + leftPadding, textY);
    placeholder.setPosition(pos.x + leftPadding, textY);
}

void InputField::draw(sf::RenderWindow& w) {
    // Draw main rectangle
    w.draw(rect);
    
    // Draw label above field
    w.draw(label);
    
    // Draw icon if present
    if (icon != InputIcon::None && iconSprite.getTexture()) {
        w.draw(iconSprite);
    }
    
    // Draw input text or placeholder
    if (content.empty() && !isFocused) {
        w.draw(placeholder);
    } else {
        w.draw(inputText);
    }
    
    // Draw password visibility toggle for password fields
    if (isPasswordField && !content.empty() && toggleVisibilitySprite.getTexture()) {
        w.draw(toggleVisibilitySprite);
    }
    
    // Draw error message
    if (hasError) {
        w.draw(errorText);
    }
}

void InputField::updateHover(const sf::Vector2i& m) {
    bool isHovering = rect.getGlobalBounds().contains((float)m.x, (float)m.y);
    
    if (!isFocused && !hasError) {
        if (isHovering) {
            rect.setOutlineColor(UIStyles::Colors::Primary);
        } else {
            rect.setOutlineColor(UIStyles::Colors::BorderDefault);
        }
    }
}

bool InputField::clicked(const sf::Vector2i& m) {
    bool wasClicked = rect.getGlobalBounds().contains((float)m.x, (float)m.y);
    
    // Check if visibility toggle was clicked (for password fields)
    if (isPasswordField && !content.empty() && toggleVisibilitySprite.getTexture()) {
        sf::FloatRect toggleBounds = toggleVisibilitySprite.getGlobalBounds();
        if (toggleBounds.contains((float)m.x, (float)m.y)) {
            togglePasswordVisibility();
            return false; // Don't activate the field, just toggle visibility
        }
    }
    
    if (wasClicked) {
        isActive = true;
        isFocused = true;
        rect.setOutlineColor(UIStyles::Colors::Primary);
        rect.setOutlineThickness(2.f);
        return true;
    }
    
    isActive = false;
    isFocused = false;
    
    if (!hasError) {
        rect.setOutlineColor(UIStyles::Colors::BorderDefault);
    } else {
        rect.setOutlineColor(UIStyles::Colors::BorderError);
    }
    
    return false;
}

void InputField::handleTextInput(sf::Uint32 unicode) {
    if (!isActive) return;

    if (unicode == 8) { // Backspace
        if (!content.empty()) {
            content.pop_back();
        }
    } else if (unicode == 13) { // Enter key
        isActive = false;
        isFocused = false;
        if (!hasError) {
            rect.setOutlineColor(UIStyles::Colors::BorderDefault);
        }
    } else if (unicode >= 32 && unicode < 128) { // Printable characters
        if (fieldName == "username" && content.length() < 15) {
            content += static_cast<char>(unicode);
        } else if (fieldName != "username") {
            content += static_cast<char>(unicode);
        }
    }

    // Update displayed text (mask if password and not visible)
    if (isPasswordField && !passwordVisible) {
        inputText.setString(std::string(content.size(), '•'));
    } else {
        inputText.setString(content);
    }

    if (hasError) {
        hasError = false;
        errorText.setString("");
        rect.setOutlineColor(UIStyles::Colors::Primary);
    }
}

bool InputField::validateEmail() {
    if (content.find('@') == std::string::npos || content.find('.') == std::string::npos) {
        hasError = true;
        errorText.setString("Please enter a valid email address");
        rect.setOutlineColor(UIStyles::Colors::BorderError);
        return false;
    }
    hasError = false;
    errorText.setString("");
    rect.setOutlineColor(UIStyles::Colors::BorderDefault);
    return true;
}

bool InputField::validateUsername() {
    if (content.empty()) {
        hasError = true;
        errorText.setString("Username cannot be empty");
        rect.setOutlineColor(UIStyles::Colors::BorderError);
        return false;
    }
    if (content.length() < 3) {
        hasError = true;
        errorText.setString("Username must be at least 3 characters");
        rect.setOutlineColor(UIStyles::Colors::BorderError);
        return false;
    }
    hasError = false;
    errorText.setString("");
    rect.setOutlineColor(UIStyles::Colors::BorderDefault);
    return true;
}

void InputField::setError(const std::string& message) {
    hasError = true;
    errorText.setString(message);
    rect.setOutlineColor(UIStyles::Colors::BorderError);
}

void InputField::clearError() {
    hasError = false;
    errorText.setString("");
    rect.setOutlineColor(isFocused ? UIStyles::Colors::Primary : UIStyles::Colors::BorderDefault);
}

void InputField::setActive(bool active) {
    isActive = active;
    isFocused = active;
    
    if (active) {
        rect.setOutlineColor(UIStyles::Colors::Primary);
    } else if (!hasError) {
        rect.setOutlineColor(UIStyles::Colors::BorderDefault);
    }
}

void InputField::setPosition(const sf::Vector2f& pos) {
    rect.setPosition(pos);
    label.setPosition(pos.x, pos.y - 25);
    errorText.setPosition(pos.x, pos.y + rect.getSize().y + 5);
    
    // Update icon position if present
    if (icon != InputIcon::None && iconSprite.getTexture()) {
        float iconX = pos.x + UIStyles::Spacing::Medium;
        float iconY = pos.y + (rect.getSize().y - UIStyles::Sizes::IconSize) / 2.f;
        iconSprite.setPosition(iconX, iconY);
    }
    
    // Update toggle visibility position for password fields
    if (isPasswordField && toggleVisibilitySprite.getTexture()) {
        float toggleX = pos.x + rect.getSize().x - UIStyles::Sizes::IconSize - UIStyles::Spacing::Medium;
        float toggleY = pos.y + (rect.getSize().y - UIStyles::Sizes::IconSize) / 2.f;
        toggleVisibilitySprite.setPosition(toggleX, toggleY);
    }
    
    updateTextPosition();
}

void InputField::setSize(const sf::Vector2f& size) {
    rect.setSize(size);
    sf::Vector2f pos = rect.getPosition();
    errorText.setPosition(pos.x, pos.y + size.y + 5);
    
    // Update toggle visibility position for password fields
    if (isPasswordField && toggleVisibilitySprite.getTexture()) {
        float toggleX = pos.x + size.x - UIStyles::Sizes::IconSize - UIStyles::Spacing::Medium;
        float toggleY = pos.y + (size.y - UIStyles::Sizes::IconSize) / 2.f;
        toggleVisibilitySprite.setPosition(toggleX, toggleY);
    }
    
    updateTextPosition();
}

void InputField::setIcon(InputIcon inputIcon, sf::Texture* iconTexture) {
    icon = inputIcon;
    if (iconTexture && icon != InputIcon::None) {
        iconSprite.setTexture(*iconTexture);
        
        // Scale icon to appropriate size
        sf::Vector2u texSize = iconTexture->getSize();
        float scale = UIStyles::Sizes::IconSize / static_cast<float>(texSize.x);
        iconSprite.setScale(scale, scale);
        
        // Position icon
        sf::Vector2f pos = rect.getPosition();
        float iconX = pos.x + UIStyles::Spacing::Medium;
        float iconY = pos.y + (rect.getSize().y - UIStyles::Sizes::IconSize) / 2.f;
        iconSprite.setPosition(iconX, iconY);
        
        updateTextPosition();
    }
}

void InputField::setIsPasswordField(bool isPassword) {
    isPasswordField = isPassword;
    if (isPassword) {
        passwordVisible = false;
    }
}

void InputField::togglePasswordVisibility() {
    if (!isPasswordField) return;
    
    passwordVisible = !passwordVisible;
    
    // Update displayed text
    if (passwordVisible) {
        inputText.setString(content);
    } else {
        inputText.setString(std::string(content.size(), '•'));
    }
}

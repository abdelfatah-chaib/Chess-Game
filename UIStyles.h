#pragma once
#include <SFML/Graphics.hpp>
#include "FontManager.h"

namespace UIStyles {
    // Color Palette - Modern Chess Master Theme
    namespace Colors {
        // Primary Colors
        const sf::Color Primary = sf::Color(0, 128, 255);        // #0080FF - Main blue
        const sf::Color PrimaryHover = sf::Color(0, 112, 230);   // Darker blue on hover
        const sf::Color PrimaryActive = sf::Color(0, 96, 204);   // Even darker on click
        
        // Background Colors
        const sf::Color Background = sf::Color(245, 247, 250);   // #F5F7FA - Light grey bg
        const sf::Color CardBackground = sf::Color::White;       // #FFFFFF - White cards
        
        // Text Colors
        const sf::Color TextPrimary = sf::Color(44, 62, 80);     // #2C3E50 - Dark grey
        const sf::Color TextSecondary = sf::Color(107, 114, 128); // #6B7280 - Medium grey
        const sf::Color TextPlaceholder = sf::Color(156, 163, 175); // #9CA3AF - Light grey
        const sf::Color TextError = sf::Color(239, 68, 68);      // #EF4444 - Red
        const sf::Color TextSuccess = sf::Color(34, 197, 94);    // #22C55E - Green
        
        // Border Colors
        const sf::Color BorderDefault = sf::Color(224, 228, 232); // #E0E4E8
        const sf::Color BorderFocus = sf::Color(0, 128, 255, 128); // Primary with alpha
        const sf::Color BorderError = sf::Color(239, 68, 68);
        
        // OAuth Button Colors
        const sf::Color GoogleBg = sf::Color::White;
        const sf::Color GoogleBorder = sf::Color(229, 231, 235); // #E5E7EB
        const sf::Color AppleBg = sf::Color(0, 0, 0);
        const sf::Color AppleText = sf::Color::White;
        
        // Shadow Color
        const sf::Color Shadow = sf::Color(0, 0, 0, 15);         // Very subtle shadow
    }
    
    // Border Radius
    namespace Radius {
        const float Small = 8.f;      // For buttons, inputs
        const float Medium = 12.f;    // For cards
        const float Large = 16.f;     // For main containers
    }
    
    // Spacing
    namespace Spacing {
        const float XSmall = 4.f;
        const float Small = 8.f;
        const float Medium = 16.f;
        const float Large = 24.f;
        const float XLarge = 32.f;
        const float XXLarge = 48.f;
    }
    
    // Typography - Professional Chess UI Font System
    namespace Typography {
        // Size Scale
        const unsigned int TitleSize = 64;        // Screen titles (Cinzel)
        const unsigned int SubtitleSize = 48;     // Section headers (Cinzel)
        const unsigned int HeadingSize = 32;      // Card headers (Inter SemiBold)
        const unsigned int SubheadingSize = 24;   // Subsections (Inter Medium)
        const unsigned int BodySize = 18;         // Body text (Inter Regular)
        const unsigned int CaptionSize = 14;      // Secondary info (Inter Regular)
        const unsigned int SmallSize = 12;        // Hints/footnotes (Inter Regular)
        const unsigned int MonoSize = 16;         // Numbers/code (JetBrains Mono)
        
        // Legacy sizes (kept for backward compatibility)
        const unsigned int LegacyTitleSize = 48;
        const unsigned int LegacySubtitleSize = 32;
        const unsigned int LegacyHeadingSize = 24;
        const unsigned int LegacyBodySize = 16;
        
        // Font Type Usage Guidelines
        // 
        // INTER (Primary UI) → Buttons, body text, labels, inputs
        //   - Regular (400): Default body text
        //   - Medium (500): Emphasized text, input values
        //   - SemiBold (600): Buttons, headings, important labels
        //
        // CINZEL (Branding) → Screen titles, app name, victory/defeat
        //   - Regular (400): Subtitles, section headers
        //   - Bold (700): Main title, app branding
        //
        // JETBRAINS MONO (Monospace) → Timer, scores, coordinates, move history
        //   - Regular (400): All numeric/code displays
        
        // Helper function to get appropriate font for text type
        inline FontType getFontForContext(const std::string& context) {
            if (context == "title" || context == "app-name" || context == "victory" || context == "defeat") {
                return FontType::CINZEL_BOLD;
            }
            else if (context == "subtitle" || context == "section-header") {
                return FontType::CINZEL_REGULAR;
            }
            else if (context == "timer" || context == "score" || context == "coordinate" || context == "move-history") {
                return FontType::JETBRAINS_MONO;
            }
            else if (context == "button" || context == "heading") {
                return FontType::INTER_SEMIBOLD;
            }
            else if (context == "emphasized") {
                return FontType::INTER_MEDIUM;
            }
            else {
                return FontType::INTER_REGULAR; // Default for body text
            }
        }
        
        // Helper to configure text with proper font and size
        inline void applyTextStyle(sf::Text& text, FontManager* fontManager, 
                                   const std::string& context, unsigned int size = 0) {
            FontType fontType = getFontForContext(context);
            sf::Font* font = fontManager->getFont(fontType);
            
            if (font) {
                text.setFont(*font);
            }
            
            // Set size based on context if not specified
            if (size == 0) {
                if (context == "title" || context == "app-name") size = TitleSize;
                else if (context == "subtitle") size = SubtitleSize;
                else if (context == "heading") size = HeadingSize;
                else if (context == "subheading") size = SubheadingSize;
                else if (context == "timer" || context == "score") size = MonoSize;
                else size = BodySize;
            }
            
            text.setCharacterSize(size);
        }
    }
    
    // Component Sizes
    namespace Sizes {
        const float InputHeight = 48.f;
        const float ButtonHeight = 48.f;
        const float IconSize = 20.f;
        const float CheckboxSize = 20.f;
    }
    
    // Card Shadow (for modern card effect)
    struct CardShadow {
        static void apply(sf::RectangleShape& shape) {
            // SFML doesn't support box-shadow directly
            // We'll use outline for now, but could implement multi-layered rectangles
            shape.setOutlineThickness(0.f);
        }
    };
    
    // Helper function to create rounded rectangle (approximation using outline)
    inline void applyRoundedCorners(sf::RectangleShape& rect, float radius) {
        // SFML limitation: true rounded corners require custom shape or shader
        // For now, we'll use visual tricks with outline
        rect.setOutlineThickness(0.f);
    }
}

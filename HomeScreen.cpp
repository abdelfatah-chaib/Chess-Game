#include "HomeScreen.h"
#include "ScreenManager.h"
#include "UIHelpers.h"
#include "UIStyles.h"
#include "BoardTheme.h"
#include <iostream>

HomeScreen::HomeScreen(ScreenManager* manager) 
    : Screen(manager), 
      sidebar(sf::Vector2f(0, 0), sf::Vector2f(190.f, 800.f), *manager->getFontManager()->getFont("main")),
      scrollOffset(0.f),
      maxScrollOffset(0.f) {
    
    // Initialize theme from UserData or default
    selectedTheme = manager->getUserData().getSelectedBoardTheme();
    BoardThemeManager::setCurrentTheme(selectedTheme);
    
    sf::Font* font = manager->getFontManager()->getFont("main");
    if (!font) return;

    // Header - Welcome text
    welcomeText.setFont(*font);
    welcomeText.setString("Welcome back, Alex");
    welcomeText.setCharacterSize(UIStyles::Typography::SubtitleSize);
    welcomeText.setFillColor(UIStyles::Colors::TextPrimary);
    welcomeText.setStyle(sf::Text::Bold);

    readyText.setFont(*font);
    readyText.setString("Ready for your next challenge?");
    readyText.setCharacterSize(UIStyles::Typography::BodySize);
    readyText.setFillColor(UIStyles::Colors::TextSecondary);

    onlinePlayersText.setFont(*font);
    onlinePlayersText.setString("● 12,403 Players Online");
    onlinePlayersText.setCharacterSize(UIStyles::Typography::CaptionSize);
    onlinePlayersText.setFillColor(UIStyles::Colors::TextSecondary);

    // Upgrade button
    btnUpgradePro = Button(
        sf::Vector2f(900.f, 20.f),
        sf::Vector2f(160.f, 40.f),
        "UPGRADE TO PRO",
        *font,
        UIStyles::Typography::CaptionSize
    );
    btnUpgradePro.setFillColor(sf::Color(255, 193, 7)); // Yellow/gold

    // User info
    userAvatar.setSize(sf::Vector2f(40.f, 40.f));
    userAvatar.setFillColor(sf::Color(200, 200, 200));

    userName.setFont(*font);
    userName.setString("Alex Gambit");
    userName.setCharacterSize(UIStyles::Typography::CaptionSize);
    userName.setFillColor(UIStyles::Colors::TextPrimary);
    userName.setStyle(sf::Text::Bold);

    userRating.setFont(*font);
    userRating.setString("Rating: 1450");
    userRating.setCharacterSize(UIStyles::Typography::SmallSize);
    userRating.setFillColor(UIStyles::Colors::TextSecondary);

    // Quick Pair card
    quickPairCard.setSize(sf::Vector2f(640.f, 280.f));
    quickPairCard.setFillColor(sf::Color::White);

    // Try to load chess image
    sf::Texture* chessTexture = manager->getTextureManager()->getTexture("bg_username");
    if (chessTexture) {
        quickPairImage.setTexture(*chessTexture);
    }

    quickPairTitle.setFont(*font);
    quickPairTitle.setString("⚡ Quick Pair");
    quickPairTitle.setCharacterSize(UIStyles::Typography::HeadingSize);
    quickPairTitle.setFillColor(UIStyles::Colors::TextPrimary);
    quickPairTitle.setStyle(sf::Text::Bold);

    quickPairDesc.setFont(*font);
    quickPairDesc.setString("Instantly match with players of similar skill level for\na ranked game.");
    quickPairDesc.setCharacterSize(UIStyles::Typography::CaptionSize);
    quickPairDesc.setFillColor(UIStyles::Colors::TextSecondary);

    // Time control dropdown
    timeControlDropdown.setSize(sf::Vector2f(200.f, 40.f));
    timeControlDropdown.setFillColor(sf::Color::White);
    timeControlDropdown.setOutlineThickness(1.f);
    timeControlDropdown.setOutlineColor(UIStyles::Colors::BorderDefault);

    timeControlText.setFont(*font);
    timeControlText.setString("10 min (Rapid)");
    timeControlText.setCharacterSize(UIStyles::Typography::CaptionSize);
    timeControlText.setFillColor(UIStyles::Colors::TextPrimary);

    // Play Now button
    btnPlayNow = Button(
        sf::Vector2f(0, 0),
        sf::Vector2f(200.f, UIStyles::Sizes::ButtonHeight),
        "Play Now →",
        *font,
        UIStyles::Typography::BodySize
    );
    btnPlayNow.setStyle(ButtonStyle::Primary);

    // Game Modes title
    gameModesTitle.setFont(*font);
    gameModesTitle.setString("Game Modes");
    gameModesTitle.setCharacterSize(UIStyles::Typography::HeadingSize);
    gameModesTitle.setFillColor(UIStyles::Colors::TextPrimary);
    gameModesTitle.setStyle(sf::Text::Bold);

    // Create game mode cards
    createGameModeCards(*font);
    
    // Theme selector title
    themeSelectorTitle.setFont(*font);
    themeSelectorTitle.setString("Choose Board Theme");
    themeSelectorTitle.setCharacterSize(UIStyles::Typography::HeadingSize);
    themeSelectorTitle.setFillColor(UIStyles::Colors::TextPrimary);
    themeSelectorTitle.setStyle(sf::Text::Bold);
    
    // Create theme cards
    createThemeCards(*font);

    // Stats card
    statsCard.setSize(sf::Vector2f(310.f, 200.f));
    statsCard.setFillColor(sf::Color::White);

    statsTitle.setFont(*font);
    statsTitle.setString("Your Stats");
    statsTitle.setCharacterSize(UIStyles::Typography::HeadingSize);
    statsTitle.setFillColor(UIStyles::Colors::TextPrimary);
    statsTitle.setStyle(sf::Text::Bold);

    ratingText.setFont(*font);
    ratingText.setString("1450");
    ratingText.setCharacterSize(48);
    ratingText.setFillColor(UIStyles::Colors::TextPrimary);
    ratingText.setStyle(sf::Text::Bold);

    ratingLabel.setFont(*font);
    ratingLabel.setString("RAPID RATING");
    ratingLabel.setCharacterSize(UIStyles::Typography::SmallSize);
    ratingLabel.setFillColor(UIStyles::Colors::TextSecondary);

    // Progress bar
    progressBar.setSize(sf::Vector2f(280.f, 8.f));
    progressBar.setFillColor(sf::Color(230, 230, 230));

    progressFill.setSize(sf::Vector2f(180.f, 8.f)); // 64% filled
    progressFill.setFillColor(sf::Color(76, 175, 80)); // Green

    // Recent Matches
    matchesCard.setSize(sf::Vector2f(310.f, 200.f));
    matchesCard.setFillColor(sf::Color::White);

    matchesTitle.setFont(*font);
    matchesTitle.setString("Recent Matches");
    matchesTitle.setCharacterSize(UIStyles::Typography::HeadingSize);
    matchesTitle.setFillColor(UIStyles::Colors::TextPrimary);
    matchesTitle.setStyle(sf::Text::Bold);

    btnViewAll = Button(
        sf::Vector2f(0, 0),
        sf::Vector2f(80.f, 30.f),
        "View All",
        *font,
        UIStyles::Typography::CaptionSize
    );
    btnViewAll.setStyle(ButtonStyle::Link);

    createRecentMatches(*font);
}

void HomeScreen::createGameModeCards(sf::Font& font) {
    gameModeCards.clear();

    std::vector<std::tuple<std::string, std::string, sf::Color>> modes = {
        {"Play Online", "Challenge players worldwide", sf::Color(33, 150, 243)},
        {"Vs Computer", "Practice against Stockfish", sf::Color(76, 175, 80)},
        {"Play a Friend", "Invite and play casually", sf::Color(255, 152, 0)}
    };

    for (size_t i = 0; i < modes.size(); ++i) {
        Card card(
            sf::Vector2f(220.f + i * 220.f, 420.f),
            sf::Vector2f(200.f, 140.f),
            std::get<0>(modes[i]),
            std::get<1>(modes[i]),
            font,
            std::get<2>(modes[i])
        );
        gameModeCards.push_back(card);
    }
}

void HomeScreen::createThemeCards(sf::Font& font) {
    themeCards.clear();

    std::vector<std::tuple<std::string, std::string, sf::Color>> themes = {
        {"Wooden", "Classic wooden board", sf::Color(139, 90, 60)},
        {"Red Wine", "Elegant burgundy theme", sf::Color(123, 45, 51)},
        {"Blue Sky", "Modern blue aesthetic", sf::Color(116, 142, 172)}
    };

    for (size_t i = 0; i < themes.size(); ++i) {
        Card card(
            sf::Vector2f(220.f + i * 220.f, 590.f),
            sf::Vector2f(200.f, 120.f),
            std::get<0>(themes[i]),
            std::get<1>(themes[i]),
            font,
            std::get<2>(themes[i])
        );
        themeCards.push_back(card);
    }
}

void HomeScreen::createRecentMatches(sf::Font& font) {
    recentMatches.clear();

    std::vector<std::tuple<std::string, std::string, std::string, std::string>> matches = {
        {"WIN", "vs. GrandMaster2000", "Rapid • 10 min", "Dec 12, 2023"},
        {"LOSS", "vs. DeepBlueJr", "Rapid • 10 min", "Dec 11, 2023"}
    };

    for (size_t i = 0; i < matches.size(); ++i) {
        MatchItem match;

        match.background.setSize(sf::Vector2f(280.f, 50.f));
        match.background.setFillColor(sf::Color::Transparent);

        match.resultBadge.setFont(font);
        match.resultBadge.setString(std::get<0>(matches[i]));
        match.resultBadge.setCharacterSize(UIStyles::Typography::SmallSize);
        match.resultBadge.setStyle(sf::Text::Bold);
        
        if (std::get<0>(matches[i]) == "WIN") {
            match.resultBadge.setFillColor(sf::Color(76, 175, 80));
        } else {
            match.resultBadge.setFillColor(sf::Color(244, 67, 54));
        }

        match.opponentName.setFont(font);
        match.opponentName.setString(std::get<1>(matches[i]));
        match.opponentName.setCharacterSize(UIStyles::Typography::CaptionSize);
        match.opponentName.setFillColor(UIStyles::Colors::TextPrimary);
        match.opponentName.setStyle(sf::Text::Bold);

        match.matchInfo.setFont(font);
        match.matchInfo.setString(std::get<2>(matches[i]));
        match.matchInfo.setCharacterSize(UIStyles::Typography::SmallSize);
        match.matchInfo.setFillColor(UIStyles::Colors::TextSecondary);

        match.date.setFont(font);
        match.date.setString(std::get<3>(matches[i]));
        match.date.setCharacterSize(UIStyles::Typography::SmallSize);
        match.date.setFillColor(UIStyles::Colors::TextSecondary);

        recentMatches.push_back(match);
    }
}

void HomeScreen::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) {
    // Handle mouse wheel scrolling
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
            float scrollSpeed = 30.f;
            scrollOffset -= event.mouseWheelScroll.delta * scrollSpeed;
            
            // Clamp scroll offset
            if (scrollOffset < 0.f) scrollOffset = 0.f;
            if (scrollOffset > maxScrollOffset) scrollOffset = maxScrollOffset;
            
            std::cout << "[HomeScreen] Scroll: " << scrollOffset << " / " << maxScrollOffset << std::endl;
        }
    }
    
    // Update hover states (adjust mouse position for scrolling)
    sf::Vector2i adjustedMousePos(mousePos.x, static_cast<int>(mousePos.y + scrollOffset));
    
    sidebar.updateHover(mousePos); // Sidebar doesn't scroll
    btnUpgradePro.updateHover(adjustedMousePos);
    btnPlayNow.updateHover(adjustedMousePos);
    btnViewAll.updateHover(adjustedMousePos);

    for (auto& card : gameModeCards) {
        card.updateHover(adjustedMousePos);
    }
    
    for (auto& card : themeCards) {
        card.updateHover(adjustedMousePos);
    }

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        // Sidebar clicks (no scroll adjustment)
        SidebarItem clicked = sidebar.handleClick(mousePos);
        if (clicked != SidebarItem::Home) {
            switch (clicked) {
                case SidebarItem::Play:
                    std::cout << "[HomeScreen] Navigate to Play" << std::endl;
                    break;
                case SidebarItem::Settings:
                    std::cout << "[HomeScreen] Navigate to Settings" << std::endl;
                    break;
                case SidebarItem::Logout:
                    std::cout << "[HomeScreen] Logout" << std::endl;
                    screenManager->changeState(STATE_LOGIN);
                    break;
                default:
                    break;
            }
        }

        // Quick Pair Play Now button (with scroll adjustment)
        if (btnPlayNow.clicked(adjustedMousePos)) {
            std::cout << "[HomeScreen] Starting Quick Pair..." << std::endl;
        }

        // Game Mode cards (with scroll adjustment)
        for (size_t i = 0; i < gameModeCards.size(); ++i) {
            if (gameModeCards[i].clicked(adjustedMousePos)) {
                std::cout << "[HomeScreen] Game mode " << i << " clicked" << std::endl;
            }
        }
        
        // Theme cards (with scroll adjustment)
        for (size_t i = 0; i < themeCards.size(); ++i) {
            if (themeCards[i].clicked(adjustedMousePos)) {
                selectedTheme = static_cast<BoardTheme>(i);
                BoardThemeManager::setCurrentTheme(selectedTheme);
                screenManager->getUserData().setSelectedBoardTheme(selectedTheme);
                std::cout << "[HomeScreen] Theme selected: " << BoardThemeManager::getThemeName(selectedTheme) << std::endl;
            }
        }

        // View All button (with scroll adjustment)
        if (btnViewAll.clicked(adjustedMousePos)) {
            std::cout << "[HomeScreen] View all matches" << std::endl;
        }

        // Upgrade button (with scroll adjustment)
        if (btnUpgradePro.clicked(adjustedMousePos)) {
            std::cout << "[HomeScreen] Upgrade to PRO" << std::endl;
        }
    }
}

void HomeScreen::update(float deltaTime) {
    // Animation updates if needed
}

void HomeScreen::updateMaxScroll(float windowHeight) {
    // Calculate total content height
    float totalHeight = UIStyles::Spacing::Large + 100.f + 300.f + 220.f + 190.f + 250.f;
    
    // Max scroll is content height minus window height
    maxScrollOffset = std::max(0.f, totalHeight - windowHeight + 100.f);
}

void HomeScreen::draw(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f winSizeF((float)windowSize.x, (float)windowSize.y);

    // Draw background
    window.clear(UIStyles::Colors::Background);

    // Draw sidebar (doesn't scroll)
    sidebar.draw(window);

    float sidebarWidth = sidebar.getWidth();
    float contentX = sidebarWidth + UIStyles::Spacing::Large;
    float contentY = UIStyles::Spacing::Large - scrollOffset; // Apply scroll offset

    // Calculate total content height for scrolling
    float totalContentHeight = contentY + 100.f + 300.f + 220.f + 190.f + 250.f; // Approximate
    updateMaxScroll(winSizeF.y);

    // Header - Welcome text
    welcomeText.setPosition(contentX, contentY);
    window.draw(welcomeText);

    readyText.setPosition(contentX, contentY + 40.f);
    window.draw(readyText);

    // Online players (top right) - fixed position, doesn't scroll
    onlinePlayersText.setPosition(winSizeF.x - 200.f, UIStyles::Spacing::Large);
    window.draw(onlinePlayersText);

    // User avatar and info (top right) - fixed position
    userAvatar.setPosition(winSizeF.x - 180.f, UIStyles::Spacing::Large + 30.f);
    window.draw(userAvatar);

    userName.setPosition(winSizeF.x - 130.f, UIStyles::Spacing::Large + 35.f);
    window.draw(userName);

    userRating.setPosition(winSizeF.x - 130.f, UIStyles::Spacing::Large + 52.f);
    window.draw(userRating);

    // Upgrade button - fixed position
    btnUpgradePro.setPosition(sf::Vector2f(winSizeF.x - 180.f, UIStyles::Spacing::Large + 80.f));
    btnUpgradePro.draw(window);

    // Quick Pair section (scrollable)
    float quickPairY = contentY + 100.f;
    quickPairCard.setPosition(contentX, quickPairY);
    window.draw(quickPairCard);

    // Chess image on left side of card
    if (quickPairImage.getTexture()) {
        sf::Vector2u texSize = quickPairImage.getTexture()->getSize();
        float imgWidth = 280.f;
        float imgHeight = 280.f;
        float scaleX = imgWidth / texSize.x;
        float scaleY = imgHeight / texSize.y;
        quickPairImage.setScale(scaleX, scaleY);
        quickPairImage.setPosition(contentX + 10.f, quickPairY);
        window.draw(quickPairImage);
    }

    // Quick Pair content (right side)
    float textX = contentX + 310.f;
    quickPairTitle.setPosition(textX, quickPairY + 40.f);
    window.draw(quickPairTitle);

    quickPairDesc.setPosition(textX, quickPairY + 80.f);
    window.draw(quickPairDesc);

    // Time control label
    sf::Text timeLabel;
    timeLabel.setFont(*screenManager->getFontManager()->getFont(FontType::INTER_REGULAR));
    timeLabel.setString("TIME CONTROL");
    timeLabel.setCharacterSize(UIStyles::Typography::SmallSize);
    timeLabel.setFillColor(UIStyles::Colors::TextSecondary);
    timeLabel.setPosition(textX, quickPairY + 130.f);
    window.draw(timeLabel);

    // Time control dropdown
    timeControlDropdown.setPosition(textX, quickPairY + 150.f);
    window.draw(timeControlDropdown);

    timeControlText.setPosition(textX + 10.f, quickPairY + 158.f);
    window.draw(timeControlText);

    // Play Now button
    btnPlayNow.setPosition(sf::Vector2f(textX, quickPairY + 210.f));
    btnPlayNow.draw(window);

    // Game Modes section (scrollable)
    float gameModesY = quickPairY + 300.f;
    gameModesTitle.setPosition(contentX, gameModesY);
    window.draw(gameModesTitle);

    for (size_t i = 0; i < gameModeCards.size(); ++i) {
        gameModeCards[i].setPosition(sf::Vector2f(contentX + i * 220.f, gameModesY + 50.f));
        gameModeCards[i].draw(window);
    }
    
    // Theme Selector section (scrollable) ✨
    float themeSelectorY = gameModesY + 220.f;
    themeSelectorTitle.setPosition(contentX, themeSelectorY);
    window.draw(themeSelectorTitle);

    for (size_t i = 0; i < themeCards.size(); ++i) {
        themeCards[i].setPosition(sf::Vector2f(contentX + i * 220.f, themeSelectorY + 50.f));
        themeCards[i].draw(window);
        
        // Highlight selected theme
        if (static_cast<BoardTheme>(i) == selectedTheme) {
            sf::RectangleShape highlight;
            highlight.setSize(sf::Vector2f(200.f, 120.f));
            highlight.setPosition(sf::Vector2f(contentX + i * 220.f, themeSelectorY + 50.f));
            highlight.setFillColor(sf::Color::Transparent);
            highlight.setOutlineThickness(3.f);
            highlight.setOutlineColor(sf::Color(255, 215, 0));
            window.draw(highlight);
        }
    }

    // Stats and Matches section (scrollable)
    float bottomSectionY = themeSelectorY + 190.f;

    statsCard.setPosition(contentX, bottomSectionY);
    window.draw(statsCard);

    statsTitle.setPosition(contentX + 15.f, bottomSectionY + 15.f);
    window.draw(statsTitle);

    ratingText.setPosition(contentX + 15.f, bottomSectionY + 60.f);
    window.draw(ratingText);

    ratingLabel.setPosition(contentX + 15.f, bottomSectionY + 115.f);
    window.draw(ratingLabel);

    progressBar.setPosition(contentX + 15.f, bottomSectionY + 140.f);
    window.draw(progressBar);

    progressFill.setPosition(contentX + 15.f, bottomSectionY + 140.f);
    window.draw(progressFill);

    float matchesX = contentX + 330.f;
    matchesCard.setPosition(matchesX, bottomSectionY);
    window.draw(matchesCard);

    matchesTitle.setPosition(matchesX + 15.f, bottomSectionY + 15.f);
    window.draw(matchesTitle);

    btnViewAll.setPosition(sf::Vector2f(matchesX + 210.f, bottomSectionY + 15.f));
    btnViewAll.draw(window);

    for (size_t i = 0; i < recentMatches.size(); ++i) {
        float matchY = bottomSectionY + 70.f + i * 60.f;

        recentMatches[i].resultBadge.setPosition(matchesX + 15.f, matchY);
        window.draw(recentMatches[i].resultBadge);

        recentMatches[i].opponentName.setPosition(matchesX + 60.f, matchY);
        window.draw(recentMatches[i].opponentName);

        recentMatches[i].matchInfo.setPosition(matchesX + 60.f, matchY + 18.f);
        window.draw(recentMatches[i].matchInfo);

        recentMatches[i].date.setPosition(matchesX + 210.f, matchY + 5.f);
        window.draw(recentMatches[i].date);
    }
    
    // Draw scroll indicator if content is scrollable
    if (maxScrollOffset > 0.f) {
        float scrollbarHeight = 100.f;
        float scrollbarY = (scrollOffset / maxScrollOffset) * (winSizeF.y - scrollbarHeight);
        
        sf::RectangleShape scrollbar(sf::Vector2f(5.f, scrollbarHeight));
        scrollbar.setPosition(winSizeF.x - 10.f, scrollbarY);
        scrollbar.setFillColor(sf::Color(150, 150, 150, 200));
        window.draw(scrollbar);
    }
}

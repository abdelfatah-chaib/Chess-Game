#include "GUI/StatisticsScreen.h"
#include "UIStyles.h"
#include "Services/ScreenManager.h"
#include "UIStyles.h"
#include "UIHelpers.h"
#include "UIStyles.h"
#include <iostream>
#include <sstream>
#include <iomanip>

StatisticsScreen::StatisticsScreen(ScreenManager* manager)
    : Screen(manager), totalGames(0), wins(0), losses(0), draws(0), winRate(0.0f), scrollOffset(0) {
    
    // Setup background
    sf::Texture* bgTexture = manager->getTextureManager()->getTexture("bg_main_menu");
    if (bgTexture) {
        background.setTexture(*bgTexture);
    }

    // Load modern fonts
    sf::Font* titleFont = manager->getFontManager()->getFont(FontType::CINZEL_REGULAR);
    sf::Font* headingFont = manager->getFontManager()->getFont(FontType::INTER_SEMIBOLD);
    sf::Font* bodyFont = manager->getFontManager()->getFont(FontType::INTER_REGULAR);
    sf::Font* monoFont = manager->getFontManager()->getFont(FontType::JETBRAINS_MONO);
    
    // Fallback
    if (!titleFont) titleFont = manager->getFontManager()->getFont(FontType::INTER_REGULAR);
    if (!headingFont) headingFont = titleFont;
    if (!bodyFont) bodyFont = headingFont;
    if (!monoFont) monoFont = bodyFont;

    // Title - Cinzel Regular
    title.setFont(*titleFont);
    title.setString("Statistics");
    title.setCharacterSize(UIStyles::Typography::SubtitleSize);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);

    // Stats text - JetBrains Mono (monospace pour les nombres!)
    statsText.setFont(*monoFont);
    statsText.setCharacterSize(UIStyles::Typography::MonoSize);
    statsText.setFillColor(sf::Color::White);

    // History title - Inter SemiBold
    historyTitle.setFont(*headingFont);
    historyTitle.setString("Recent Games");
    historyTitle.setCharacterSize(UIStyles::Typography::HeadingSize);
    historyTitle.setFillColor(sf::Color::White);
    historyTitle.setStyle(sf::Text::Bold);

    // Back button - Inter SemiBold
    btnBack = Button(
        sf::Vector2f(50.f, 50.f),
        sf::Vector2f(120.f, UIStyles::Sizes::ButtonHeight),
        "Back",
        *headingFont,
        UIStyles::Typography::BodySize
    );
}

void StatisticsScreen::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) {
    btnBack.updateHover(mousePos);

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        
        if (btnBack.clicked(mousePos)) {
            screenManager->changeState(STATE_MAIN_MENU);
        }
    }
    
    // Handle scroll for history
    if (event.type == sf::Event::MouseWheelScrolled) {
        scrollOffset -= static_cast<int>(event.mouseWheelScroll.delta);
        if (scrollOffset < 0) scrollOffset = 0;
        if (scrollOffset > static_cast<int>(gameHistory.size()) - 5) {
            scrollOffset = std::max(0, static_cast<int>(gameHistory.size()) - 5);
        }
    }
}

void StatisticsScreen::update(float deltaTime) {
    // Reload stats when entering screen
    static bool statsLoaded = false;
    if (!statsLoaded) {
        loadPlayerStats();
        loadGameHistory();
        updateStatsDisplay();
        updateHistoryDisplay();
        statsLoaded = true;
    }
}

void StatisticsScreen::draw(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f winSizeF(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y));
    
    // Scale background to fit window
    if (background.getTexture()) {
        UIHelpers::setupBackground(background, *background.getTexture(), true, window);
    }
    window.draw(background);

    // Semi-transparent overlay
    sf::RectangleShape overlay(winSizeF);
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    // Scale and center title
    title.setCharacterSize(static_cast<unsigned int>(UIHelpers::scaleFont(48.f, winSizeF)));
    UIHelpers::centerText(title, winSizeF.x / 2.f, UIHelpers::scaleFont(60.f, winSizeF));
    window.draw(title);

    // Draw stats box
    sf::RectangleShape statsBox(UIHelpers::scaleSize(sf::Vector2f(500.f, 200.f), winSizeF));
    statsBox.setPosition((winSizeF.x - statsBox.getSize().x) / 2.f, UIHelpers::scaleFont(140.f, winSizeF));
    statsBox.setFillColor(sf::Color(40, 40, 60, 220));
    statsBox.setOutlineThickness(3.f);
    statsBox.setOutlineColor(sf::Color(100, 150, 255));
    window.draw(statsBox);

    // Draw stats text
    statsText.setCharacterSize(static_cast<unsigned int>(UIHelpers::scaleFont(24.f, winSizeF)));
    statsText.setPosition(statsBox.getPosition().x + UIHelpers::scaleFont(20.f, winSizeF),
                          statsBox.getPosition().y + UIHelpers::scaleFont(20.f, winSizeF));
    window.draw(statsText);

    // Draw history title - INCREASED spacing to avoid overlap
    historyTitle.setCharacterSize(static_cast<unsigned int>(UIHelpers::scaleFont(32.f, winSizeF)));
    UIHelpers::centerText(historyTitle, winSizeF.x / 2.f, UIHelpers::scaleFont(400.f, winSizeF));  // Changed from 370 to 400
    window.draw(historyTitle);

    // Draw history box - INCREASED spacing
    sf::RectangleShape historyBox(UIHelpers::scaleSize(sf::Vector2f(800.f, 250.f), winSizeF));
    historyBox.setPosition((winSizeF.x - historyBox.getSize().x) / 2.f, UIHelpers::scaleFont(450.f, winSizeF));  // Changed from 420 to 450
    historyBox.setFillColor(sf::Color(40, 40, 60, 220));
    historyBox.setOutlineThickness(3.f);
    historyBox.setOutlineColor(sf::Color(100, 150, 255));
    window.draw(historyBox);

    // Draw history entries
    float yPos = historyBox.getPosition().y + UIHelpers::scaleFont(15.f, winSizeF);
    int displayCount = std::min(5, static_cast<int>(historyTexts.size()) - scrollOffset);
    
    for (int i = 0; i < displayCount; i++) {
        int index = i + scrollOffset;
        if (index < historyTexts.size()) {
            historyTexts[index].setCharacterSize(static_cast<unsigned int>(UIHelpers::scaleFont(18.f, winSizeF)));
            historyTexts[index].setPosition(historyBox.getPosition().x + UIHelpers::scaleFont(20.f, winSizeF), yPos);
            window.draw(historyTexts[index]);
            yPos += UIHelpers::scaleFont(45.f, winSizeF);
        }
    }

    // Back button
    sf::Vector2f btnSize = UIHelpers::scaleSize(sf::Vector2f(120.f, 50.f), winSizeF);
    sf::Vector2f btnPos(UIHelpers::scaleFont(50.f, winSizeF), UIHelpers::scaleFont(50.f, winSizeF));
    btnBack.setPosition(btnPos);
    btnBack.getRect().setSize(btnSize);
    btnBack.getText().setCharacterSize(static_cast<unsigned int>(UIHelpers::scaleFont(24.f, winSizeF)));
    UIHelpers::centerText(btnBack.getText(), btnPos.x + btnSize.x / 2.f, btnPos.y + btnSize.y / 2.f);
    btnBack.draw(window);
}

void StatisticsScreen::loadPlayerStats() {
    UserData& user = screenManager->getUserData();
    
    if (user.id < 0) {
        std::cout << "[StatisticsScreen] No user logged in" << std::endl;
        return;
    }

    SQLiteManager& db = screenManager->getDatabaseManager();
    
    if (db.getPlayerStats(user.id, totalGames, wins, losses, draws)) {
        winRate = totalGames > 0 ? (static_cast<float>(wins) / totalGames) * 100.0f : 0.0f;
        std::cout << "[StatisticsScreen] Loaded stats - Games: " << totalGames 
                  << ", Wins: " << wins << ", Losses: " << losses << ", Draws: " << draws << std::endl;
    } else {
        std::cout << "[StatisticsScreen] No stats found for user " << user.id << std::endl;
        totalGames = wins = losses = draws = 0;
        winRate = 0.0f;
    }
}

void StatisticsScreen::loadGameHistory() {
    UserData& user = screenManager->getUserData();
    
    if (user.id < 0) {
        std::cout << "[StatisticsScreen] No user logged in" << std::endl;
        return;
    }

    SQLiteManager& db = screenManager->getDatabaseManager();
    gameHistory = db.getGameHistory(user.id, 20); // Get last 20 games
    
    std::cout << "[StatisticsScreen] Loaded " << gameHistory.size() << " game history entries" << std::endl;
}

void StatisticsScreen::updateStatsDisplay() {
    std::stringstream ss;
    ss << "Total Games: " << totalGames << "\n\n";
    ss << "Wins: " << wins << "\n";
    ss << "Losses: " << losses << "\n";
    ss << "Draws: " << draws << "\n\n";
    ss << "Win Rate: " << std::fixed << std::setprecision(1) << winRate << "%";
    
    statsText.setString(ss.str());
}

void StatisticsScreen::updateHistoryDisplay() {
    historyTexts.clear();
    
    // Load modern fonts
    sf::Font* bodyFont = screenManager->getFontManager()->getFont(FontType::INTER_REGULAR);
    sf::Font* monoFont = screenManager->getFontManager()->getFont(FontType::JETBRAINS_MONO);
    
    // Fallback
    if (!bodyFont) bodyFont = screenManager->getFontManager()->getFont(FontType::INTER_REGULAR);
    if (!monoFont) monoFont = bodyFont;

    for (const auto& entry : gameHistory) {
        sf::Text text;
        text.setFont(*monoFont);  // Use monospace for game history
        
        // Format: "WIN vs Black (Easy) - 24 moves - 2024-01-15"
        std::stringstream ss;
        
        // Result with color
        std::string resultStr = entry.result;
        std::transform(resultStr.begin(), resultStr.end(), resultStr.begin(), ::toupper);
        ss << resultStr;
        
        // Color
        std::string colorStr = entry.playerColor;
        colorStr[0] = std::toupper(colorStr[0]);
        ss << " as " << colorStr;
        
        // Difficulty
        std::string diffStr;
        switch (entry.difficulty) {
            case 1: diffStr = "Easy"; break;
            case 2: diffStr = "Medium"; break;
            case 3: diffStr = "Hard"; break;
            default: diffStr = "Unknown"; break;
        }
        ss << " (" << diffStr << ")";
        
        // Moves
        ss << " - " << entry.movesCount << " moves";
        
        // Date (extract date part from datetime)
        if (entry.playedAt.length() >= 10) {
            ss << " - " << entry.playedAt.substr(0, 10);
        }
        
        text.setString(ss.str());
        
        // Color based on result
        if (entry.result == "win") {
            text.setFillColor(sf::Color::Green);
        } else if (entry.result == "loss") {
            text.setFillColor(sf::Color(255, 100, 100));
        } else {
            text.setFillColor(sf::Color::Yellow);
        }
        
        historyTexts.push_back(text);
    }
    
    if (gameHistory.empty()) {
        sf::Text emptyText;
        emptyText.setFont(*bodyFont);
        emptyText.setString("No games played yet. Start playing to see your history!");
        emptyText.setFillColor(sf::Color(150, 150, 150));
        historyTexts.push_back(emptyText);
    }
}

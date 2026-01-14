#include "GameBoardScreen.h"
#include "UIStyles.h"
#include "Services/ScreenManager.h"
#include "UIStyles.h"
#include "UIHelpers.h"
#include "UIStyles.h"
#include "BoardTheme.h"
#include <cmath>
#include <iostream>

GameBoardScreen::GameBoardScreen(ScreenManager* manager, GameController* gc)
    : Screen(manager), gameController(gc), boardInitialized(false),
      pendingPromotionRow(-1), pendingPromotionCol(-1) {

    // Get selected theme from UserData AND BoardThemeManager
    BoardTheme selectedTheme = manager->getUserData().getSelectedBoardTheme();
    BoardThemeManager::setCurrentTheme(selectedTheme);
    currentTheme = BoardThemeManager::getThemeColors(selectedTheme);
    
    std::cout << "[GameBoardScreen] Initialized with theme: " << BoardThemeManager::getThemeName(selectedTheme) << std::endl;
    
    // Setup background
    sf::Texture* bgTexture = manager->getTextureManager()->getTexture("bgk");
    if (bgTexture) {
        background.setTexture(*bgTexture);
    }

    // Load modern fonts
    sf::Font* headingFont = manager->getFontManager()->getFont(FontType::INTER_SEMIBOLD);
    sf::Font* bodyFont = manager->getFontManager()->getFont(FontType::INTER_REGULAR);
    sf::Font* monoFont = manager->getFontManager()->getFont(FontType::JETBRAINS_MONO);
    
    // Fallback
    if (!headingFont) headingFont = manager->getFontManager()->getFont(FontType::INTER_SEMIBOLD);
    if (!bodyFont) bodyFont = headingFont;
    if (!monoFont) monoFont = bodyFont;

    // Initialize UI elements with base positions/sizes (will be scaled in draw())
    // Player panels
    whitePlayerPanel.setSize(sf::Vector2f(150.f, 80.f));
    whitePlayerPanel.setFillColor(sf::Color(50, 50, 80, 220));
    whitePlayerPanel.setOutlineThickness(3.f);
    whitePlayerPanel.setOutlineColor(sf::Color(0, 255, 255));

    blackPlayerPanel.setSize(sf::Vector2f(150.f, 80.f));
    blackPlayerPanel.setFillColor(sf::Color(50, 50, 80, 220));
    blackPlayerPanel.setOutlineThickness(3.f);
    blackPlayerPanel.setOutlineColor(sf::Color(255, 100, 100));

    // Load pawn icons
    sf::Texture* whitePawnTex = manager->getTextureManager()->getTexture("white_pawn");
    if (whitePawnTex) {
        whitePawnIcon.setTexture(*whitePawnTex);
    }

    sf::Texture* blackPawnTex = manager->getTextureManager()->getTexture("black_pawn");
    if (blackPawnTex) {
        blackPawnIcon.setTexture(*blackPawnTex);
    }

    // Points text - JetBrains Mono pour les scores!
    whitePointsText.setFont(*monoFont);
    whitePointsText.setString("Score: 0");
    whitePointsText.setCharacterSize(UIStyles::Typography::MonoSize);
    whitePointsText.setFillColor(sf::Color::Green);
    whitePointsText.setOutlineColor(sf::Color::Black);
    whitePointsText.setOutlineThickness(1.f);
    whitePointsText.setStyle(sf::Text::Bold);

    blackPointsText.setFont(*monoFont);
    blackPointsText.setString("Score: 0");
    blackPointsText.setCharacterSize(UIStyles::Typography::MonoSize);
    blackPointsText.setFillColor(sf::Color(255, 100, 100));
    blackPointsText.setOutlineColor(sf::Color::Black);
    blackPointsText.setOutlineThickness(1.f);
    blackPointsText.setStyle(sf::Text::Bold);

    // Player titles - Inter SemiBold
    whitePlayerTitle.setFont(*headingFont);
    whitePlayerTitle.setString("White");
    whitePlayerTitle.setCharacterSize(UIStyles::Typography::BodySize);
    whitePlayerTitle.setFillColor(sf::Color::Green);
    whitePlayerTitle.setOutlineColor(sf::Color::Black);
    whitePlayerTitle.setOutlineThickness(2.f);
    whitePlayerTitle.setStyle(sf::Text::Bold);

    blackPlayerTitle.setFont(*headingFont);
    blackPlayerTitle.setString("Black");
    blackPlayerTitle.setCharacterSize(UIStyles::Typography::BodySize);
    blackPlayerTitle.setFillColor(sf::Color(255, 100, 100));
    blackPlayerTitle.setOutlineColor(sf::Color::Black);
    blackPlayerTitle.setOutlineThickness(2.f);
    blackPlayerTitle.setStyle(sf::Text::Bold);

    // Chess clock panels and text - JetBrains Mono for time display
    whiteClockPanel.setSize(sf::Vector2f(120.f, 50.f));
    whiteClockPanel.setFillColor(sf::Color(40, 40, 60, 220));
    whiteClockPanel.setOutlineThickness(3.f);
    whiteClockPanel.setOutlineColor(sf::Color(100, 200, 100));

    blackClockPanel.setSize(sf::Vector2f(120.f, 50.f));
    blackClockPanel.setFillColor(sf::Color(40, 40, 60, 220));
    blackClockPanel.setOutlineThickness(3.f);
    blackClockPanel.setOutlineColor(sf::Color(100, 200, 100));

    whiteClockText.setFont(*monoFont);
    whiteClockText.setString("10:00");
    whiteClockText.setCharacterSize(UIStyles::Typography::HeadingSize);
    whiteClockText.setFillColor(sf::Color::White);
    whiteClockText.setStyle(sf::Text::Bold);
    whiteClockText.setOutlineColor(sf::Color::Black);
    whiteClockText.setOutlineThickness(2.f);

    blackClockText.setFont(*monoFont);
    blackClockText.setString("10:00");
    blackClockText.setCharacterSize(UIStyles::Typography::HeadingSize);
    blackClockText.setFillColor(sf::Color::White);
    blackClockText.setStyle(sf::Text::Bold);
    blackClockText.setOutlineColor(sf::Color::Black);
    blackClockText.setOutlineThickness(2.f);

    // Control buttons - Inter SemiBold
    btnRestart = Button(sf::Vector2f(0, 0), sf::Vector2f(100.f, 40.f), "Restart", *headingFont, UIStyles::Typography::CaptionSize);
    btnRestart.setFillColor(sf::Color(218, 165, 32, 255)); // Goldenrod yellow

    btnUndo = Button(sf::Vector2f(0, 0), sf::Vector2f(100.f, 40.f), "Undo", *headingFont, UIStyles::Typography::CaptionSize);
    btnUndo.setFillColor(sf::Color(220, 20, 60, 255)); // Crimson red

    btnBack = Button(sf::Vector2f(0, 0), sf::Vector2f(100.f, 40.f), "Menu", *headingFont, UIStyles::Typography::CaptionSize);
    btnBack.setFillColor(sf::Color(70, 130, 180, 255)); // Steel blue

    // Turn text - Inter SemiBold
    turnText.setFont(*headingFont);
    turnText.setString("White's Turn");
    turnText.setCharacterSize(UIStyles::Typography::BodySize);
    turnText.setFillColor(sf::Color::Green);
    turnText.setStyle(sf::Text::Bold);
    
    // Position evaluation text - Inter Regular
    aiStatusText.setFont(*bodyFont);
    aiStatusText.setString("");
    aiStatusText.setCharacterSize(UIStyles::Typography::CaptionSize);
    aiStatusText.setFillColor(sf::Color::Yellow);
    aiStatusText.setStyle(sf::Text::Bold);
    
    // Game end text - Inter SemiBold
    gameEndText.setFont(*headingFont);
    gameEndText.setString("");
    gameEndText.setCharacterSize(UIStyles::Typography::HeadingSize);
    gameEndText.setFillColor(sf::Color::White);
    gameEndText.setStyle(sf::Text::Bold);
    gameEndText.setOutlineColor(sf::Color::Black);
    gameEndText.setOutlineThickness(2.f);
    
    // Game end overlay
    gameEndOverlay.setFillColor(sf::Color(0, 0, 0, 180));
}

void GameBoardScreen::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) {
    // Handle promotion dialog events first (priority - blocks all other input)
    if (promotionDialog.isVisible()) {
        promotionDialog.handleEvent(event, mousePos);
        promotionDialog.updateHover(mousePos);
        return; // Block ALL other interactions while dialog is open
    }
    
    // Check if promotion is pending and show dialog
    if (gameController->isPromotionPending() && !promotionDialog.isVisible()) {
        std::cout << "[GameBoardScreen] Promotion pending - showing dialog" << std::endl;
        
        // Get font from screen manager
        const sf::Font* font = screenManager->getFontManager()->getFont(FontType::INTER_SEMIBOLD);
        
        // Show promotion dialog with callback
        promotionDialog.show(
            gameController->getPendingPromotionColor(),
            gameController->getPendingPromotionRow(),
            gameController->getPendingPromotionCol(),
            [this](const std::string& promotionPiece) {
                std::cout << "[GameBoardScreen] Player selected promotion piece: " << promotionPiece << std::endl;
                
                // Execute the promotion through GameController
                gameController->executePawnPromotion(promotionPiece);
            },
            font
        );
        return; // Don't process other events this frame
    }
    
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        // Check board click only if game is not ended and no promotion pending
        if (!gameController->isPaused() && !gameController->isGameEnded() && !gameController->isPromotionPending()) {
            std::pair<int, int> square = gameController->getBoard().getSquareAtPosition(
                (float)mousePos.x, (float)mousePos.y);
            int row = square.first;
            int col = square.second;

            if (row != -1 && col != -1) {
                handleBoardClick(row, col);
            }
        }

        // Control buttons (only if no promotion pending)
        if (!gameController->isPromotionPending()) {
            if (btnRestart.clicked(mousePos)) {
                std::cout << "[GameBoardScreen] Restart button clicked - restarting game" << std::endl;
                // Reset the game state WITHOUT forcing board re-initialization
                gameController->resetGame();
                // DON'T set boardInitialized = false - keep board in same position
            }
            else if (btnUndo.clicked(mousePos) && !gameController->isGameEnded()) {
                std::cout << "[GameBoardScreen] Undo button clicked" << std::endl;
                gameController->undoMove();
            }
            else if (btnBack.clicked(mousePos)) {
                std::cout << "[GameBoardScreen] Menu button clicked - returning to main menu" << std::endl;
                // Reset screen state before leaving
                resetScreen();
                // Pause game and return to main menu
                gameController->pauseGame();
                screenManager->changeState(STATE_MAIN_MENU);
            }
        }
    }
}

void GameBoardScreen::update(float deltaTime) {
    if (!boardInitialized) {
        initializeBoard();
        // Configure AI based on user selection
        configureAI();
        boardInitialized = true;
        
        // Set player names for score tracking
        gameController->setPlayerNames("White Player", "Black Player");
    }
    
    // *** FORCE RELOAD SETTINGS FROM USER DATA ON EACH FRAME ***
    // This ensures that if user returns from MainMenu with new settings,
    // they are applied immediately when returning to game board
    static BoardTheme lastTheme = BoardTheme::Wooden;
    static std::string lastColor = "";
    static int lastDifficulty = 0;
    static bool firstRun = true;  // Track if this is the first run
    
    BoardTheme currentThemeEnum = screenManager->getUserData().getSelectedBoardTheme();
    std::string currentColor = screenManager->getUserData().getSelectedColor();
    int currentDifficulty = screenManager->getUserData().getSelectedDifficulty();
    
    // Initialize tracking variables on first run
    if (firstRun) {
        lastTheme = currentThemeEnum;
        lastColor = currentColor;
        lastDifficulty = currentDifficulty;
        firstRun = false;
    }
    
    // Detect if settings changed while in MainMenu
    bool settingsChanged = (currentThemeEnum != lastTheme || 
                           currentColor != lastColor || 
                           currentDifficulty != lastDifficulty);
    
    if (settingsChanged && boardInitialized) {
        std::cout << "[GameBoardScreen] Settings changed - reinitializing game..." << std::endl;
        std::cout << "  Theme: " << static_cast<int>(currentThemeEnum) << std::endl;
        std::cout << "  Color: " << currentColor << std::endl;
        std::cout << "  Difficulty: " << currentDifficulty << std::endl;
        
        // Force full reinitialization
        boardInitialized = false;
        gameController->resetGame();
        
        // Update tracking variables
        lastTheme = currentThemeEnum;
        lastColor = currentColor;
        lastDifficulty = currentDifficulty;
        
        return; // Will reinitialize on next frame
    }
    
    // Update tracking variables
    lastTheme = currentThemeEnum;
    lastColor = currentColor;
    lastDifficulty = currentDifficulty;

    // Check if game just ended to save to database
    static bool gameWasSaved = false;
    bool currentlyEnded = gameController->isGameEnded();
    
    if (currentlyEnded && !gameWasSaved) {
        saveGameToDatabase();
        gameWasSaved = true;
    } else if (!currentlyEnded) {
        gameWasSaved = false;
    }

    gameController->update(deltaTime);
    updateTurnIndicator();
    updateScoreDisplay();
    updateClockDisplay();  // Update chess clock display
    updateAIStatus();
}

void GameBoardScreen::draw(sf::RenderWindow& window) {
    updateBoardScale(window);
    
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f winSizeF((float)windowSize.x, (float)windowSize.y);
    
    // Draw background image if available, otherwise use themed color
    if (background.getTexture()) {
        UIHelpers::setupBackground(background, *background.getTexture(), true, window);
        window.draw(background);
        
        // Add semi-transparent theme overlay on top of background image
        sf::RectangleShape themeOverlay(winSizeF);
        themeOverlay.setFillColor(sf::Color(currentTheme.BG.r, currentTheme.BG.g, currentTheme.BG.b, 100)); // Semi-transparent
        window.draw(themeOverlay);
    } else {
        // Fallback: solid themed background if no image
        sf::RectangleShape solidBackground(winSizeF);
        solidBackground.setFillColor(currentTheme.BG);
        window.draw(solidBackground);
    }
    
    // Add frame around the board
    sf::FloatRect boardBounds = gameController->getBoard().getBounds();
    float padding = 20.f;
    
    sf::RectangleShape boardFrame;
    boardFrame.setSize(sf::Vector2f(boardBounds.width + padding * 2, boardBounds.height + padding * 2));
    boardFrame.setPosition(boardBounds.left - padding, boardBounds.top - padding);
    boardFrame.setFillColor(currentTheme.FRAME);
    window.draw(boardFrame);
    
    sf::RectangleShape boardBorder;
    boardBorder.setSize(sf::Vector2f(boardBounds.width + padding, boardBounds.height + padding));
    boardBorder.setPosition(boardBounds.left - padding / 2, boardBounds.top - padding / 2);
    boardBorder.setOutlineThickness(3.f);
    boardBorder.setOutlineColor(currentTheme.BORDER);
    boardBorder.setFillColor(sf::Color::Transparent);
    window.draw(boardBorder);

    // Scale and position UI elements
    sf::Vector2f basePanelSize(150.f, 80.f);  // Increased height for score display
    sf::Vector2f scaledPanelSize = UIHelpers::scaleSize(basePanelSize, winSizeF);
    
    // White player panel - top left
    whitePlayerPanel.setSize(scaledPanelSize);
    sf::Vector2f whitePanelPos = UIHelpers::scalePosition(sf::Vector2f(20.f, 20.f), winSizeF);
    whitePlayerPanel.setPosition(whitePanelPos);
    window.draw(whitePlayerPanel);
    
    // Black player panel - top right  
    blackPlayerPanel.setSize(scaledPanelSize);
    sf::Vector2f blackPanelPos = UIHelpers::scalePosition(sf::Vector2f(1030.f, 20.f), winSizeF);
    blackPlayerPanel.setPosition(blackPanelPos);
    window.draw(blackPlayerPanel);

    // Pawn icons in panels
    if (whitePawnIcon.getTexture()) {
        sf::Vector2f pawnCaseSize = UIHelpers::scaleSize(sf::Vector2f(60.f, 60.f), winSizeF);
        sf::RectangleShape whitePawnCase(pawnCaseSize);
        whitePawnCase.setFillColor(sf::Color(70, 70, 90, 200));
        whitePawnCase.setOutlineThickness(2);
        whitePawnCase.setOutlineColor(sf::Color(0, 255, 255));
        whitePawnCase.setPosition(whitePanelPos);
        window.draw(whitePawnCase);

        float iconScale = UIHelpers::scaleFont(0.12f, winSizeF);
        whitePawnIcon.setScale(iconScale, iconScale);
        float pawnX = whitePanelPos.x + (pawnCaseSize.x - whitePawnIcon.getGlobalBounds().width) / 2;
        float pawnY = whitePanelPos.y + (pawnCaseSize.y - whitePawnIcon.getGlobalBounds().height) / 2;
        whitePawnIcon.setPosition(pawnX, pawnY);
        window.draw(whitePawnIcon);
        
        // White player title - positioned to the right of pawn icon
        whitePlayerTitle.setCharacterSize((unsigned int)UIHelpers::scaleFont(16.f, winSizeF));
        sf::Vector2f whiteTitlePos(whitePanelPos.x + pawnCaseSize.x + UIHelpers::scaleFont(5.f, winSizeF),
                                    whitePanelPos.y + UIHelpers::scaleFont(10.f, winSizeF));
        whitePlayerTitle.setPosition(whiteTitlePos);
        window.draw(whitePlayerTitle);
        
        // White score - positioned below title
        whitePointsText.setCharacterSize((unsigned int)UIHelpers::scaleFont(12.f, winSizeF));
        sf::Vector2f whiteScorePos(whitePanelPos.x + pawnCaseSize.x + UIHelpers::scaleFont(5.f, winSizeF),
                                    whitePanelPos.y + UIHelpers::scaleFont(35.f, winSizeF));
        whitePointsText.setPosition(whiteScorePos);
        window.draw(whitePointsText);
    }

    if (blackPawnIcon.getTexture()) {
        sf::Vector2f pawnCaseSize = UIHelpers::scaleSize(sf::Vector2f(60.f, 60.f), winSizeF);
        sf::RectangleShape blackPawnCase(pawnCaseSize);
        blackPawnCase.setFillColor(sf::Color(70, 70, 90, 200));
        blackPawnCase.setOutlineThickness(2);
        blackPawnCase.setOutlineColor(sf::Color(255, 100, 100));
        blackPawnCase.setPosition(blackPanelPos);
        window.draw(blackPawnCase);

        float iconScale = UIHelpers::scaleFont(0.12f, winSizeF);
        blackPawnIcon.setScale(iconScale, iconScale);
        float pawnX = blackPanelPos.x + (pawnCaseSize.x - blackPawnIcon.getGlobalBounds().width) / 2;
        float pawnY = blackPanelPos.y + (pawnCaseSize.y - blackPawnIcon.getGlobalBounds().height) / 2;
        blackPawnIcon.setPosition(pawnX, pawnY);
        window.draw(blackPawnIcon);
        
        // Black player title - positioned to the right of pawn icon
        blackPlayerTitle.setCharacterSize((unsigned int)UIHelpers::scaleFont(16.f, winSizeF));
        sf::Vector2f blackTitlePos(blackPanelPos.x + pawnCaseSize.x + UIHelpers::scaleFont(5.f, winSizeF),
                                    blackPanelPos.y + UIHelpers::scaleFont(10.f, winSizeF));
        blackPlayerTitle.setPosition(blackTitlePos);
        window.draw(blackPlayerTitle);
        
        // Black score - positioned below title
        blackPointsText.setCharacterSize((unsigned int)UIHelpers::scaleFont(12.f, winSizeF));
        sf::Vector2f blackScorePos(blackPanelPos.x + pawnCaseSize.x + UIHelpers::scaleFont(5.f, winSizeF),
                                    blackPanelPos.y + UIHelpers::scaleFont(35.f, winSizeF));
        blackPointsText.setPosition(blackScorePos);
        window.draw(blackPointsText);
    }

    // Chess clock displays - positioned BELOW the score panels with spacing
    sf::Vector2f clockSize = UIHelpers::scaleSize(sf::Vector2f(120.f, 50.f), winSizeF);
    float clockFontSize = UIHelpers::scaleFont(20.f, winSizeF);
    float clockSpacingBelowPanel = UIHelpers::scaleFont(15.f, winSizeF);

    // White clock (below white panel)
    whiteClockPanel.setSize(clockSize);
    sf::Vector2f whiteClockPos(whitePanelPos.x, whitePanelPos.y + scaledPanelSize.y + clockSpacingBelowPanel);
    whiteClockPanel.setPosition(whiteClockPos);
    window.draw(whiteClockPanel);

    whiteClockText.setCharacterSize((unsigned int)clockFontSize);
    UIHelpers::centerText(whiteClockText, whiteClockPos.x + clockSize.x / 2.f, whiteClockPos.y + clockSize.y / 2.f);
    window.draw(whiteClockText);

    // Black clock (below black panel)
    blackClockPanel.setSize(clockSize);
    sf::Vector2f blackClockPos(blackPanelPos.x, blackPanelPos.y + scaledPanelSize.y + clockSpacingBelowPanel);
    blackClockPanel.setPosition(blackClockPos);
    window.draw(blackClockPanel);

    blackClockText.setCharacterSize((unsigned int)clockFontSize);
    UIHelpers::centerText(blackClockText, blackClockPos.x + clockSize.x / 2.f, blackClockPos.y + clockSize.y / 2.f);
    window.draw(blackClockText);

    // Chess board
    drawBoard(window);

    // Control buttons - Positioned at the bottom center with spacing
    sf::Vector2f btnSize = UIHelpers::scaleSize(sf::Vector2f(100.f, 40.f), winSizeF);
    float scaledFontSize = UIHelpers::scaleFont(16.f, winSizeF);
    
    // Calculate center position for buttons at the bottom
    float totalBtnWidth = btnSize.x * 3;  // 3 buttons
    float btnSpacing = UIHelpers::scaleFont(20.f, winSizeF);  // Spacing between buttons
    totalBtnWidth += btnSpacing * 2;  // Add spacing
    
    float startX = (winSizeF.x - totalBtnWidth) / 2.f;
    float btnY = winSizeF.y - btnSize.y - UIHelpers::scaleFont(30.f, winSizeF);  // Position at bottom with margin
    
    // Restart button
    sf::Vector2f restartPos(startX, btnY);
    btnRestart.setPosition(restartPos);
    btnRestart.getRect().setSize(btnSize);
    btnRestart.getText().setCharacterSize((unsigned int)scaledFontSize);
    UIHelpers::centerText(btnRestart.getText(), restartPos.x + btnSize.x / 2, restartPos.y + btnSize.y / 2);
    
    // Undo button (middle)
    sf::Vector2f undoPos(startX + btnSize.x + btnSpacing, btnY);
    btnUndo.setPosition(undoPos);
    btnUndo.getRect().setSize(btnSize);
    btnUndo.getText().setCharacterSize((unsigned int)scaledFontSize);
    UIHelpers::centerText(btnUndo.getText(), undoPos.x + btnSize.x / 2, undoPos.y + btnSize.y / 2);
    
    // Menu button (right)
    sf::Vector2f menuPos(startX + (btnSize.x + btnSpacing) * 2, btnY);
    btnBack.setPosition(menuPos);
    btnBack.getRect().setSize(btnSize);
    btnBack.getText().setCharacterSize((unsigned int)scaledFontSize);
    UIHelpers::centerText(btnBack.getText(), menuPos.x + btnSize.x / 2, menuPos.y + btnSize.y / 2);
    
    btnRestart.draw(window);
    btnUndo.draw(window);
    btnBack.draw(window);

    // Turn text - Position ABOVE the board with REDUCED top margin and INCREASED spacing from board
    float turnTextY = boardBounds.top - UIHelpers::scaleFont(50.f, winSizeF);  // Increased from 30px to 50px
    
    turnText.setCharacterSize((unsigned int)UIHelpers::scaleFont(18.f, winSizeF));
    UIHelpers::centerText(turnText, winSizeF.x / 2.f, turnTextY);
    window.draw(turnText);

    // AI status text (if AI is enabled) - Position below turn text with more spacing
    if (gameController->isAIEnabled()) {
        float aiStatusY = boardBounds.top - UIHelpers::scaleFont(25.f, winSizeF);  // Increased from 10px to 25px
        aiStatusText.setCharacterSize((unsigned int)UIHelpers::scaleFont(14.f, winSizeF));
        UIHelpers::centerText(aiStatusText, winSizeF.x / 2.f, aiStatusY);
        window.draw(aiStatusText);
    }

    // Draw game end overlay if game is finished
    if (gameController->isGameEnded()) {
        drawGameEndOverlay(window);
    }
    
    // Draw promotion dialog (always on top)
    if (promotionDialog.isVisible()) {
        promotionDialog.draw(window);
    }
}

void GameBoardScreen::initializeBoard() {
    sf::Vector2f winSize(1200.f, 800.f); // Default size for initialization
    float minDimension = std::min(winSize.x, winSize.y);
    float boardSize = minDimension * 0.70f;  // Increased from 0.6 to 0.7 for better visibility
    float boardX = (winSize.x - boardSize) / 2.f;
    float boardY = (winSize.y - boardSize) / 2.f;  // REMOVED the +20.f offset
    
    // Get selected piece set from user data
    PieceSetType selectedPieceSet = screenManager->getUserData().getSelectedPieceSet();
    
    std::cout << "[GameBoardScreen] Initializing board with piece set: " 
              << (selectedPieceSet == PieceSetType::CLASSIC ? "CLASSIC" : "THEMED") << std::endl;
    
    gameController->initializeBoard(boardX, boardY, boardSize / 8.f, currentTheme, selectedPieceSet);
}

void GameBoardScreen::configureAI() {
    std::cout << "[GameBoardScreen] Configuring AI..." << std::endl;
    
    // Get user preferences from screen manager
    const std::string& selectedColor = screenManager->getUserData().getSelectedColor();
    int selectedDifficulty = screenManager->getUserData().getSelectedDifficulty();
    
    std::cout << "[GameBoardScreen] Player selected: " << selectedColor << std::endl;
    std::cout << "[GameBoardScreen] AI difficulty: " << selectedDifficulty << std::endl;
    
    // Set AI difficulty
    gameController->setDifficulty(selectedDifficulty);
    
    // Determine AI color (opposite of player)
    std::string aiColor = (selectedColor == "White") ? "black" : "white";
    
    // Enable AI
    gameController->enableAI(true, aiColor);
    
    std::cout << "[GameBoardScreen] AI configured to play as " << aiColor 
              << " with difficulty level " << selectedDifficulty << std::endl;
}

void GameBoardScreen::updateAIStatus() {
    // Don't show AI status if game is ended
    if (gameController->isGameEnded()) {
        aiStatusText.setString("");
        return;
    }
    
    if (!gameController->isAIEnabled()) {
        aiStatusText.setString("");
        return;
    }
    
    if (gameController->isAIThinking()) {
        float timeRemaining = gameController->getAIThinkingTimeRemaining();
        
        // Animation de points pour montrer que l'IA réfléchit
        static sf::Clock statusClock;
        float elapsed = statusClock.getElapsedTime().asSeconds();
        int dotCount = (int)(elapsed * 3) % 4; // Change toutes les 0.33 secondes
        std::string dots = "";
        for (int i = 0; i <= dotCount; i++) {
            dots += ".";
        }
        
        // Afficher le temps restant approximatif
        int secondsLeft = (int)std::ceil(timeRemaining);
        if (secondsLeft > 0) {
            aiStatusText.setString("AI is thinking" + dots + " (" + std::to_string(secondsLeft) + "s left)");
        } else {
            aiStatusText.setString("AI is finishing" + dots);
        }
        aiStatusText.setFillColor(sf::Color::Yellow);
        
        // Redémarrer le clock toutes les 1.2 secondes pour l'animation
        if (elapsed > 1.2f) {
            statusClock.restart();
        }
    } else if (gameController->isAITurn()) {
        aiStatusText.setString("AI ready to move");
        aiStatusText.setFillColor(sf::Color::Cyan);
    } else {
        aiStatusText.setString("Your turn to play");
        aiStatusText.setFillColor(sf::Color::Green);
    }
}

void GameBoardScreen::updateTurnIndicator() {
    // Don't show turn indicator if game is ended
    if (gameController->isGameEnded()) {
        turnText.setString(gameController->getGameResultDescription());
        turnText.setFillColor(sf::Color::Yellow);
        return;
    }
    
    if (gameController->isWhiteTurn()) {
        whitePlayerPanel.setOutlineColor(sf::Color(101, 67, 33));
        blackPlayerPanel.setOutlineColor(sf::Color(100, 100, 100));
        
        if (gameController->isAIEnabled() && gameController->getAIColor() == "white") {
            turnText.setString("AI Turn (White)");
            turnText.setFillColor(sf::Color::Cyan);
        } else {
            turnText.setString("White's Turn");
            turnText.setFillColor(sf::Color::Green);
        }
    }
    else {
        blackPlayerPanel.setOutlineColor(sf::Color(101, 67, 33));
        whitePlayerPanel.setOutlineColor(sf::Color(200, 200, 200));
        
        if (gameController->isAIEnabled() && gameController->getAIColor() == "black") {
            turnText.setString("AI Turn (Black)");
            turnText.setFillColor(sf::Color::Cyan);
        } else {
            turnText.setString("Black's Turn");
            turnText.setFillColor(sf::Color(255, 100, 100));
        }
    }
}

void GameBoardScreen::updateScoreDisplay() {
    // Get current score string from GameController
    std::string scoreString = gameController->getCurrentScoreString();
    
    // Parse the score string: "White: X, Black: Y, Diff: Z"
    // Extract white and black scores
    size_t whitePos = scoreString.find("White: ");
    size_t blackPos = scoreString.find("Black: ");
    
    int whiteScore = 0;
    int blackScore = 0;
    
    if (whitePos != std::string::npos) {
        size_t commaPos = scoreString.find(",", whitePos);
        std::string whiteScoreStr = scoreString.substr(whitePos + 7, commaPos - (whitePos + 7));
        whiteScore = std::stoi(whiteScoreStr);
    }
    
    if (blackPos != std::string::npos) {
        size_t commaPos = scoreString.find(",", blackPos);
        std::string blackScoreStr = scoreString.substr(blackPos + 7, commaPos - (blackPos + 7));
        blackScore = std::stoi(blackScoreStr);
    }
    
    // Update display texts
    whitePointsText.setString("Score: " + std::to_string(whiteScore));
    blackPointsText.setString("Score: " + std::to_string(blackScore));
}

void GameBoardScreen::updateClockDisplay() {
    const ChessClock& chessClock = gameController->getChessClock();
    const PlayerClock& whiteClock = chessClock.getWhiteClock();
    const PlayerClock& blackClock = chessClock.getBlackClock();
    
    // Update clock text
    whiteClockText.setString(whiteClock.getFormattedTime());
    blackClockText.setString(blackClock.getFormattedTime());
    
    // Update white clock color based on time thresholds
    if (whiteClock.isVeryLow()) {
        // Flashing red at <= 10 seconds
        static sf::Clock flashClock;
        bool flash = (static_cast<int>(flashClock.getElapsedTime().asSeconds() * 2) % 2) == 0;
        whiteClockText.setFillColor(flash ? sf::Color::Red : sf::Color(255, 100, 100));
        whiteClockPanel.setOutlineColor(sf::Color::Red);
        whiteClockPanel.setFillColor(sf::Color(80, 20, 20, 240));
    } else if (whiteClock.isCritical()) {
        // Solid red at <= 1 minute
        whiteClockText.setFillColor(sf::Color::Red);
        whiteClockPanel.setOutlineColor(sf::Color::Red);
        whiteClockPanel.setFillColor(sf::Color(60, 20, 20, 240));
    } else if (whiteClock.isWarning()) {
        // Yellow at <= 3 minutes
        whiteClockText.setFillColor(sf::Color::Yellow);
        whiteClockPanel.setOutlineColor(sf::Color(255, 200, 0));
        whiteClockPanel.setFillColor(sf::Color(60, 50, 20, 240));
    } else {
        // Normal white
        whiteClockText.setFillColor(sf::Color::White);
        whiteClockPanel.setOutlineColor(whiteClock.isRunning ? sf::Color(100, 255, 100) : sf::Color(100, 200, 100));
        whiteClockPanel.setFillColor(whiteClock.isRunning ? sf::Color(60, 60, 80, 240) : sf::Color(40, 40, 60, 220));
    }
    
    // Update black clock color based on time thresholds
    if (blackClock.isVeryLow()) {
        // Flashing red at <= 10 seconds
        static sf::Clock flashClock;
        bool flash = (static_cast<int>(flashClock.getElapsedTime().asSeconds() * 2) % 2) == 0;
        blackClockText.setFillColor(flash ? sf::Color::Red : sf::Color(255, 100, 100));
        blackClockPanel.setOutlineColor(sf::Color::Red);
        blackClockPanel.setFillColor(sf::Color(80, 20, 20, 240));
    } else if (blackClock.isCritical()) {
        // Solid red at <= 1 minute
        blackClockText.setFillColor(sf::Color::Red);
        blackClockPanel.setOutlineColor(sf::Color::Red);
        blackClockPanel.setFillColor(sf::Color(60, 20, 20, 240));
    } else if (blackClock.isWarning()) {
        // Yellow at <= 3 minutes
        blackClockText.setFillColor(sf::Color::Yellow);
        blackClockPanel.setOutlineColor(sf::Color(255, 200, 0));
        blackClockPanel.setFillColor(sf::Color(60, 50, 20, 240));
    } else {
        // Normal white
        blackClockText.setFillColor(sf::Color::White);
        blackClockPanel.setOutlineColor(blackClock.isRunning ? sf::Color(100, 255, 100) : sf::Color(100, 200, 100));
        blackClockPanel.setFillColor(blackClock.isRunning ? sf::Color(60, 60, 80, 240) : sf::Color(40, 40, 60, 220));
    }
}

void GameBoardScreen::handleBoardClick(int row, int col) {
    std::cout << "[GameBoardScreen] Board clicked at row=" << row << ", col=" << col << std::endl;
    
    // Normal piece selection logic - GameController handles promotion detection
    bool result = gameController->selectPiece(row, col);
    std::cout << "[GameBoardScreen] selectPiece returned: " << (result ? "true" : "false") << std::endl;
}

void GameBoardScreen::drawBoard(sf::RenderWindow& window) {
    gameController->getBoard().draw(window);

    // Highlight selected piece
    if (gameController->isPieceSelected()) {
        // Add pulsing effect
        static sf::Clock pulseClock;
        float pulse = std::sin(pulseClock.getElapsedTime().asSeconds() * 5.f) * 0.3f + 0.7f;
        
        gameController->getBoard().highlightSquare(
            gameController->getSelectedRow(),
            gameController->getSelectedCol(),
            window,
            sf::Color(101, 67, 33, static_cast<sf::Uint8>(120 * pulse))
        );

        // Dessiner les points verts pour les mouvements légaux
        const auto& legalMoves = gameController->getLegalMoves();
        for (const auto& move : legalMoves) {
            gameController->getBoard().drawLegalMoveIndicator(move.first, move.second, window);
        }
    }
}

void GameBoardScreen::updateBoardScale(sf::RenderWindow& window) {
    // Recalculate board size and position based on current window size
    sf::Vector2u winSize = window.getSize();
    float minDimension = std::min((float)winSize.x, (float)winSize.y);
    float boardSize = minDimension * 0.70f;  // Increased from 0.6 to 0.7 for better visibility
    float boardX = (winSize.x - boardSize) / 2.f;
    float boardY = (winSize.y - boardSize) / 2.f;  // REMOVED the +20.f offset - perfectly centered
    
    // IMPORTANT: Reload theme AND piece set from BoardThemeManager and UserData
    BoardTheme selectedTheme = screenManager->getUserData().getSelectedBoardTheme();
    PieceSetType selectedPieceSet = screenManager->getUserData().getSelectedPieceSet();
    currentTheme = BoardThemeManager::getThemeColors(selectedTheme);
    
    // Only update board geometry if it has changed significantly OR if board was just reinitialized
    static float lastBoardX = -1.f;  // Initialize to -1 to force first update
    static float lastBoardY = -1.f;
    static float lastSquareSize = -1.f;
    float currentSquareSize = boardSize / 8.f;
    
    // Force update if coming from menu (boardInitialized was reset)
    bool forceUpdate = (lastBoardX < 0.f || lastBoardY < 0.f || lastSquareSize < 0.f);
    
    if (forceUpdate || 
        std::abs(boardX - lastBoardX) > 1.0f || 
        std::abs(boardY - lastBoardY) > 1.0f || 
        std::abs(currentSquareSize - lastSquareSize) > 1.0f) {
        
        std::cout << "[GameBoardScreen] Updating board scale: " << boardX << "," << boardY << " size=" << currentSquareSize << std::endl;
        std::cout << "[GameBoardScreen] Applied theme: " << BoardThemeManager::getThemeName(selectedTheme) << std::endl;
        std::cout << "[GameBoardScreen] Applied piece set: " << (selectedPieceSet == PieceSetType::CLASSIC ? "CLASSIC" : "THEMED") << std::endl;
        
        // Update board geometry WITH THEME AND PIECE SET
        gameController->getBoard().initialize(boardX, boardY, currentSquareSize, currentTheme, selectedPieceSet);
        
        lastBoardX = boardX;
        lastBoardY = boardY;
        lastSquareSize = currentSquareSize;
    }
}

void GameBoardScreen::drawGameEndOverlay(sf::RenderWindow& window) {
    sf::Vector2u winSize = window.getSize();
    sf::Vector2f winSizeF((float)winSize.x, (float)winSize.y);
    
    // Semi-transparent overlay over the entire window
    gameEndOverlay.setSize(winSizeF);
    gameEndOverlay.setPosition(0, 0);
    window.draw(gameEndOverlay);
    
    // Draw game result text
    std::string resultText = gameController->getGameResultDescription();
    gameEndText.setString(resultText);
    
    // Scale font size based on window
    gameEndText.setCharacterSize((unsigned int)UIHelpers::scaleFont(32.f, winSizeF));
    
    // Set color based on result
    GameResult result = gameController->getGameResult();
    if (result == GameResult::WHITE_WIN) {
        gameEndText.setFillColor(sf::Color::Green);
    } else if (result == GameResult::BLACK_WIN) {
        gameEndText.setFillColor(sf::Color(255, 100, 100));
    } else {
        gameEndText.setFillColor(sf::Color::Yellow);
    }
    
    // Center the text
    UIHelpers::centerText(gameEndText, winSizeF.x / 2.f, winSizeF.y / 2.f - UIHelpers::scaleFont(40.f, winSizeF));
    window.draw(gameEndText);
    
    // Draw "Press Restart to play again" text
    sf::Text instructionText;
    instructionText.setFont(*gameEndText.getFont());
    instructionText.setString("Press Restart to play again");
    instructionText.setCharacterSize((unsigned int)UIHelpers::scaleFont(16.f, winSizeF));
    instructionText.setFillColor(sf::Color::White);
    instructionText.setStyle(sf::Text::Bold);
    instructionText.setOutlineColor(sf::Color::Black);
    instructionText.setOutlineThickness(1.f);
    
    UIHelpers::centerText(instructionText, winSizeF.x / 2.f, winSizeF.y / 2.f + UIHelpers::scaleFont(40.f, winSizeF));
    window.draw(instructionText);
}

void GameBoardScreen::saveGameToDatabase() {
    UserData& user = screenManager->getUserData();
    
    // Only save if user is logged in
    if (user.id < 0) {
        std::cout << "[GameBoardScreen] No user logged in, skipping database save\n";
        return;
    }

    // Get game result
    GameResult result = gameController->getGameResult();
    std::string resultStr;
    
    if (result == GameResult::WHITE_WIN) {
        resultStr = "win";
    } else if (result == GameResult::BLACK_WIN) {
        resultStr = "loss";
    } else {
        resultStr = "draw";
    }

    // Get player color from user data using getter
    std::string playerColor = user.getSelectedColor();
    if (playerColor.empty()) {
        playerColor = "white"; // Default if not set
    }

    // Adjust result based on player color
    if (playerColor == "Black" || playerColor == "black") {
        // Player was black, so invert the result
        if (resultStr == "win") {
            resultStr = "loss";
        } else if (resultStr == "loss") {
            resultStr = "win";
        }
    }

    // Get difficulty from user data using getter
    int difficulty = user.getSelectedDifficulty();
    if (difficulty == 0) difficulty = 1; // Default difficulty

    // Get actual move count from GameController
    int movesCount = gameController->getMoveCount();

    // Save to database
    SQLiteManager& db = screenManager->getDatabaseManager();
    bool success = db.saveGameResult(user.id, resultStr, playerColor, difficulty, movesCount);

    if (success) {
        std::cout << "[GameBoardScreen] Game result saved to database: " 
                  << resultStr << " as " << playerColor << " with " << movesCount << " moves\n";
    } else {
        std::cerr << "[GameBoardScreen] Failed to save game result to database\n";
    }
}

void GameBoardScreen::resetScreen() {
    std::cout << "[GameBoardScreen] Resetting screen state for menu return" << std::endl;
    
    // Force board reinitialization on next visit
    boardInitialized = false;
    
    // This will ensure clean state when returning to game board
    std::cout << "[GameBoardScreen] Screen reset complete" << std::endl;
}

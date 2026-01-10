#include "MainMenuScreen.h"
#include "Application.h"
#include "UIHelpers.h"
#include "ScreenManager.h"
#include <iostream>
#include <algorithm>

MainMenuScreen::MainMenuScreen(ScreenManager* manager)
    : Screen(manager),
    whiteSelected(true),
    blackSelected(false),
    selectedDifficulty(2),
    selectedTheme(BoardTheme::Wooden),
    selectedPieceSet(PieceSetType::CLASSIC)
{
    std::cout << "[MainMenuScreen] Constructor START" << std::endl;

    if (!manager) {
        std::cerr << "[MainMenuScreen] ERROR: ScreenManager is null" << std::endl;
        return;
    }

    // Get managers
    TextureManager* texManager = manager->getTextureManager();
    FontManager* fontManager = manager->getFontManager();

    if (!texManager || !fontManager) {
        std::cerr << "[MainMenuScreen] ERROR: TextureManager or FontManager is null" << std::endl;
        return;
    }

    // Load user preferences
    UserData& userData = manager->getUserData();
    selectedTheme = userData.getSelectedBoardTheme();
    selectedPieceSet = userData.getSelectedPieceSet();

    // FORCER la sélection WHITE par défaut - ne pas charger depuis userData
    whiteSelected = true;
    blackSelected = false;
    
    selectedDifficulty = userData.getSelectedDifficulty();

    // Background
    sf::Texture* bgTex = texManager->getTexture("bg_main_menu");
    if (bgTex) {
        background.setTexture(*bgTex);
    }

    // Fonts
    sf::Font* interSemiBold = fontManager->getFont(FontType::INTER_SEMIBOLD);
    sf::Font* interRegular = fontManager->getFont(FontType::INTER_REGULAR);

    if (!interSemiBold || !interRegular) {
        std::cerr << "[MainMenuScreen] ERROR: Fonts not loaded" << std::endl;
        return;
    }

    // === HEADER ===
    welcomeText.setFont(*interSemiBold);
    welcomeText.setString("Welcome, " + userData.getUsername() + "!");
    welcomeText.setFillColor(sf::Color(245, 245, 245));

    // Add Chess Game Setup subtitle  
    chessGameSetupText.setFont(*interRegular);
    chessGameSetupText.setString("Chess Game Setup");
    chessGameSetupText.setFillColor(sf::Color(200, 200, 200));

    levelText.setFont(*interRegular);
    levelText.setString("Level: " + std::to_string(userData.getLevel()));
    levelText.setFillColor(sf::Color(220, 220, 220));

    // === HEADER ACTION BUTTONS (Statistics & Logout) ===
    btnStatistics = Button(
        sf::Vector2f(0.f, 0.f),
        sf::Vector2f(110.f, 35.f),
        "Statistics",
        *interRegular
    );
    btnStatistics.setFillColor(sf::Color(70, 130, 180));
    btnStatistics.setTextColor(sf::Color::White);

    btnLogout = Button(
        sf::Vector2f(0.f, 0.f),
        sf::Vector2f(110.f, 35.f),
        "Logout",
        *interRegular
    );
    btnLogout.setFillColor(sf::Color(180, 70, 70));
    btnLogout.setTextColor(sf::Color::White);

    // === MAIN CONTAINER (Dark Wood Card with Glow) ===
    containerShadow.setFillColor(sf::Color(0, 0, 0, 100)); // Uniformisé l'ombre à 100 alpha

    mainContainer.setFillColor(sf::Color(65, 48, 40)); // #413028 couleur plus riche
    mainContainer.setOutlineColor(sf::Color(200, 160, 100, 200)); // Border dorée plus visible
    
    // Add glow effect container avec effet amélioré
    containerGlow.setFillColor(sf::Color::Transparent);
    containerGlow.setOutlineColor(sf::Color(255, 215, 100, 150)); // Glow doré plus intense

    // === PLAY AS SECTION ===
    playAsTitle.setFont(*interSemiBold);
    playAsTitle.setString("Play as");
    playAsTitle.setFillColor(sf::Color(240, 210, 180)); // Couleur plus claire et chaleureuse

    // King icons and frames avec couleurs améliorées
    sf::Texture* whiteKingTex = texManager->getTexture("w_king");
    sf::Texture* blackKingTex = texManager->getTexture("b_king");
    if (!blackKingTex) blackKingTex = texManager->getTexture("black_pawn");

    whiteKingFrame.setFillColor(sf::Color(245, 245, 245)); // Plus lumineux
    whiteKingFrame.setOutlineColor(sf::Color(140, 100, 70)); // Bordure plus contrastée

    blackKingFrame.setFillColor(sf::Color(245, 245, 245));
    blackKingFrame.setOutlineColor(sf::Color(140, 100, 70));

    if (whiteKingTex) {
        whiteKingIcon.setTexture(*whiteKingTex);
    }
    if (blackKingTex) {
        blackKingIcon.setTexture(*blackKingTex);
    }

    // Invisible buttons for king selection
    btnWhiteKing = Button(
        sf::Vector2f(0.f, 0.f),
        sf::Vector2f(0.f, 0.f),
        "", *interRegular
    );
    btnBlackKing = Button(
        sf::Vector2f(0.f, 0.f),
        sf::Vector2f(0.f, 0.f),
        "", *interRegular
    );

    // === DIFFICULTY SECTION ===
    difficultyTitle.setFont(*interSemiBold);
    difficultyTitle.setString("Difficulty");
    difficultyTitle.setFillColor(sf::Color(240, 210, 180)); // Couleur harmonisée avec Play as

    btnEasy = Button(sf::Vector2f(0.f, 0.f), sf::Vector2f(140.f, 45.f), "Easy", *interRegular);
    btnEasy.setFillColor(sf::Color(85, 65, 50)); // Couleur plus riche
    btnEasy.setTextColor(sf::Color(255, 255, 255));

    btnMedium = Button(sf::Vector2f(0.f, 0.f), sf::Vector2f(140.f, 45.f), "Medium", *interRegular);
    btnMedium.setFillColor(sf::Color(85, 65, 50));
    btnMedium.setTextColor(sf::Color(255, 255, 255));

    btnHard = Button(sf::Vector2f(0.f, 0.f), sf::Vector2f(140.f, 45.f), "Hard", *interRegular);
    btnHard.setFillColor(sf::Color(85, 65, 50));
    btnHard.setTextColor(sf::Color(255, 255, 255));

    // === PIECE SET SELECTION SECTION ===
    pieceSetContainerShadow.setFillColor(sf::Color(0, 0, 0, 100)); // Uniformisé l'ombre

    pieceSetContainer.setFillColor(sf::Color(65, 48, 40)); // Harmonisé avec main container
    pieceSetContainer.setOutlineColor(sf::Color(90, 70, 55)); // Bordure plus contrastée

    pieceSetTitle.setFont(*interSemiBold);
    pieceSetTitle.setString("Piece Set");
    pieceSetTitle.setFillColor(sf::Color(240, 210, 180)); // Couleur harmonisée

    pieceSetSubtitle.setFont(*interRegular);
    pieceSetSubtitle.setString("Choose your piece style");
    pieceSetSubtitle.setFillColor(sf::Color(190, 190, 190)); // Plus lumineux pour meilleure lisibilité

    // === BOARD THEME SECTION (SEPARATE) ===
    themeContainerShadow.setFillColor(sf::Color(0, 0, 0, 100)); // Uniformisé l'ombre

    themeContainer.setFillColor(sf::Color(65, 48, 40)); // Harmonisé avec les autres conteneurs
    themeContainer.setOutlineColor(sf::Color(90, 70, 55)); // Bordure plus contrastée

    themeTitle.setFont(*interSemiBold);
    themeTitle.setString("Board Theme");
    themeTitle.setFillColor(sf::Color(240, 210, 180)); // Couleur harmonisée

    themeSubtitle.setFont(*interRegular);
    themeSubtitle.setString("Select a color scheme for the chessboard");
    themeSubtitle.setFillColor(sf::Color(190, 190, 190)); // Plus lumineux pour meilleure lisibilité

    // === PLAY BUTTON ===
    btnPlayNow = Button(sf::Vector2f(0.f, 0.f), sf::Vector2f(280.f, 50.f), "PLAY vs AI", *interSemiBold);
    btnPlayNow.setFillColor(sf::Color(74, 144, 226, 240));
    btnPlayNow.setTextColor(sf::Color::White);

    // Initialize animation variables
    playButtonPulseTime = 0.0f;
    playButtonPulseDirection = 1.0f;

    loadPieceSetPreviews();

    applyResponsiveLayout(sf::Vector2u(static_cast<unsigned int>(currentWindowSize.x), static_cast<unsigned int>(currentWindowSize.y)));
    updateVisualFeedback();

    std::cout << "[MainMenuScreen] Constructor END - Success" << std::endl;
}

void MainMenuScreen::applyResponsiveLayout(const sf::Vector2u& windowSizeU) {
    currentWindowSize = sf::Vector2f(static_cast<float>(windowSizeU.x), static_cast<float>(windowSizeU.y));
    sf::Vector2f windowSize(currentWindowSize);

    FontManager* fontManager = screenManager ? screenManager->getFontManager() : nullptr;
    sf::Font* interSemiBold = fontManager ? fontManager->getFont(FontType::INTER_SEMIBOLD) : nullptr;
    sf::Font* interRegular = fontManager ? fontManager->getFont(FontType::INTER_REGULAR) : nullptr;

    if (interSemiBold) {
        welcomeText.setFont(*interSemiBold);
        playAsTitle.setFont(*interSemiBold);
        difficultyTitle.setFont(*interSemiBold);
        pieceSetTitle.setFont(*interSemiBold);
        themeTitle.setFont(*interSemiBold);
        btnPlayNow.getText().setFont(*interSemiBold);
    }
    if (interRegular) {
        chessGameSetupText.setFont(*interRegular);
        levelText.setFont(*interRegular);
        btnStatistics.getText().setFont(*interRegular);
        btnLogout.getText().setFont(*interRegular);
        btnEasy.getText().setFont(*interRegular);
        btnMedium.getText().setFont(*interRegular);
        btnHard.getText().setFont(*interRegular);
        pieceSetSubtitle.setFont(*interRegular);
        themeSubtitle.setFont(*interRegular);
        btnWhiteKing.getText().setFont(*interRegular);
        btnBlackKing.getText().setFont(*interRegular);
    }

    // Espacements réduits et équilibrés
    float margin = windowSize.x * 0.025f;
    float verticalSpacing = std::max(windowSize.y * 0.012f, 8.f); // Réduit de 0.02f à 0.012f
    float smallSpacing = std::max(windowSize.y * 0.008f, 6.f); // Réduit de 0.012f à 0.008f
    float shadowOffset = std::max(UIHelpers::scaleSize(sf::Vector2f(4.f, 4.f), windowSize).x, 3.f);

    // Background scaling
    if (background.getTexture()) {
        auto texSize = background.getTexture()->getSize();
        if (texSize.x > 0 && texSize.y > 0) {
            background.setScale(windowSize.x / static_cast<float>(texSize.x), windowSize.y / static_cast<float>(texSize.y));
        }
    }

    // Header avec espacements réduits
    welcomeText.setCharacterSize(UIHelpers::scaleFont(24.f, windowSize));
    chessGameSetupText.setCharacterSize(UIHelpers::scaleFont(28.f, windowSize));
    levelText.setCharacterSize(UIHelpers::scaleFont(18.f, windowSize));

    welcomeText.setPosition(margin, margin);
    chessGameSetupText.setPosition(margin, welcomeText.getPosition().y + welcomeText.getLocalBounds().height + smallSpacing);
    
    // Right side avec proportions normales
    sf::FloatRect levelBounds = levelText.getLocalBounds();
    levelText.setPosition(windowSize.x - margin - levelBounds.width, margin);

    float rightBtnWidth = std::max(120.f, windowSize.x * 0.1f);
    float rightBtnHeight = std::max(35.f, windowSize.y * 0.045f);
    float rightBtnSpacing = smallSpacing;
    float rightBtnsX = windowSize.x - margin - rightBtnWidth;
    float rightBtnsY = levelText.getPosition().y + levelBounds.height + smallSpacing;

    btnStatistics.getRect().setSize(sf::Vector2f(rightBtnWidth, rightBtnHeight));
    btnStatistics.setPosition(sf::Vector2f(rightBtnsX, rightBtnsY));
    btnStatistics.getText().setCharacterSize(UIHelpers::scaleFont(14.f, windowSize));
    btnStatistics.setFillColor(sf::Color(74, 144, 226, 220));

    btnLogout.getRect().setSize(sf::Vector2f(rightBtnWidth, rightBtnHeight));
    btnLogout.setPosition(sf::Vector2f(rightBtnsX, rightBtnsY + rightBtnHeight + rightBtnSpacing));
    btnLogout.getText().setCharacterSize(UIHelpers::scaleFont(14.f, windowSize));
    btnLogout.setFillColor(sf::Color(226, 74, 74, 220));

    float headerBottom = std::max(
        chessGameSetupText.getPosition().y + chessGameSetupText.getLocalBounds().height,
        btnLogout.getPosition().y + rightBtnHeight
    ) + verticalSpacing * 0.3f; // RÉDUIT de 0.8f à 0.3f pour rapprocher davantage

    // Main panel avec proportions originales mais mieux équilibrées
    float panelWidth = windowSize.x * 0.55f;
    float panelPadding = std::max(18.f, panelWidth * 0.035f);
    float panelX = (windowSize.x - panelWidth) / 2.f;
    float panelY = headerBottom + verticalSpacing * 0.2f; // RÉDUIT de 0.5f à 0.2f
    float currentY = panelY + panelPadding;

    // Play as section
    playAsTitle.setCharacterSize(UIHelpers::scaleFont(18.f, windowSize));
    sf::FloatRect playAsBounds = playAsTitle.getLocalBounds();
    playAsTitle.setPosition(panelX + (panelWidth - playAsBounds.width) / 2.f, currentY);
    currentY += playAsBounds.height + smallSpacing * 1.2f;

    float kingFrameSize = panelWidth * 0.14f;
    kingFrameSize = std::max(58.f, std::min(kingFrameSize, 70.f));
    
    float kingSpacing = std::max(10.f, panelWidth * 0.04f);
    float kingsRowWidth = kingFrameSize * 2.f + kingSpacing;
    float kingStartX = panelX + (panelWidth - kingsRowWidth) / 2.f;
    float kingY = currentY;

    // Configuration des frames des rois avec bordures normales
    whiteKingFrame.setSize(sf::Vector2f(kingFrameSize, kingFrameSize));
    whiteKingFrame.setPosition(kingStartX, kingY);
    whiteKingFrame.setOutlineThickness(UIHelpers::scaleSize(sf::Vector2f(2.f, 2.f), windowSize).x);

    blackKingFrame.setSize(sf::Vector2f(kingFrameSize, kingFrameSize));
    blackKingFrame.setPosition(kingStartX + kingFrameSize + kingSpacing, kingY);
    blackKingFrame.setOutlineThickness(UIHelpers::scaleSize(sf::Vector2f(2.f, 2.f), windowSize).x);

    // Icônes des rois
    if (whiteKingIcon.getTexture()) {
        float iconSize = kingFrameSize * 0.6f;
        float texW = static_cast<float>(whiteKingIcon.getTexture()->getSize().x);
        float texH = static_cast<float>(whiteKingIcon.getTexture()->getSize().y);
        whiteKingIcon.setScale(iconSize / texW, iconSize / texH);
        whiteKingIcon.setPosition(
            whiteKingFrame.getPosition().x + (kingFrameSize - iconSize) / 2.f,
            whiteKingFrame.getPosition().y + (kingFrameSize - iconSize) / 2.f
        );
    }

    if (blackKingIcon.getTexture()) {
        float iconSize = kingFrameSize * 0.6f;
        float texW = static_cast<float>(blackKingIcon.getTexture()->getSize().x);
        float texH = static_cast<float>(blackKingIcon.getTexture()->getSize().y);
        blackKingIcon.setScale(iconSize / texW, iconSize / texH);
        blackKingIcon.setPosition(
            blackKingFrame.getPosition().x + (kingFrameSize - iconSize) / 2.f,
            blackKingFrame.getPosition().y + (kingFrameSize - iconSize) / 2.f
        );
    }

    btnWhiteKing.setPosition(whiteKingFrame.getPosition());
    btnWhiteKing.getRect().setSize(sf::Vector2f(kingFrameSize, kingFrameSize));
    btnBlackKing.setPosition(blackKingFrame.getPosition());
    btnBlackKing.getRect().setSize(sf::Vector2f(kingFrameSize, kingFrameSize));

    currentY += kingFrameSize + verticalSpacing * 1.1f;

    // Difficulty section
    difficultyTitle.setCharacterSize(UIHelpers::scaleFont(15.f, windowSize));
    sf::FloatRect diffBounds = difficultyTitle.getLocalBounds();
    difficultyTitle.setPosition(panelX + (panelWidth - diffBounds.width) / 2.f, currentY);
    currentY += diffBounds.height + smallSpacing;

    float diffBtnHeight = windowSize.y * 0.05f;
    diffBtnHeight = std::max(40.f, std::min(diffBtnHeight, 44.f));
    
    float diffSpacing = std::max(8.f, panelWidth * 0.02f);
    float diffRowWidth = panelWidth - 2.f * panelPadding;
    float diffBtnWidth = (diffRowWidth - 2.f * diffSpacing) / 3.f;
    diffBtnWidth = std::max(120.f, std::min(diffBtnWidth, 165.f));
    
    float diffStartX = panelX + (panelWidth - (diffBtnWidth * 3.f + diffSpacing * 2.f)) / 2.f;

    btnEasy.getRect().setSize(sf::Vector2f(diffBtnWidth, diffBtnHeight));
    btnMedium.getRect().setSize(sf::Vector2f(diffBtnWidth, diffBtnHeight));
    btnHard.getRect().setSize(sf::Vector2f(diffBtnWidth, diffBtnHeight));
    btnEasy.setPosition(sf::Vector2f(diffStartX, currentY));
    btnMedium.setPosition(sf::Vector2f(diffStartX + diffBtnWidth + diffSpacing, currentY));
    btnHard.setPosition(sf::Vector2f(diffStartX + 2.f * (diffBtnWidth + diffSpacing), currentY));
    unsigned int diffFontSize = static_cast<unsigned int>(UIHelpers::scaleFont(14.f, windowSize));
    btnEasy.getText().setCharacterSize(diffFontSize);
    btnMedium.getText().setCharacterSize(diffFontSize);
    btnHard.getText().setCharacterSize(diffFontSize);

    currentY += diffBtnHeight + verticalSpacing;

    // Play button avec largeur réduite
    float playBtnWidth = panelWidth * 0.50f; // RÉDUIT de 0.6f à 0.50f
    playBtnWidth = std::max(panelWidth * 0.48f, std::min(playBtnWidth, panelWidth * 0.52f));
    
    float playBtnHeight = windowSize.y * 0.065f;
    playBtnHeight = std::max(48.f, std::min(playBtnHeight, 52.f));
    
    float playBtnX = panelX + (panelWidth - playBtnWidth) / 2.f;
    float playBtnY = currentY;

    btnPlayNow.getRect().setSize(sf::Vector2f(playBtnWidth, playBtnHeight));
    btnPlayNow.setPosition(sf::Vector2f(playBtnX, playBtnY));
    btnPlayNow.getText().setCharacterSize(UIHelpers::scaleFont(18.f, windowSize));

    float panelHeight = (playBtnY + playBtnHeight + panelPadding) - panelY;
    mainContainer.setSize(sf::Vector2f(panelWidth, panelHeight));
    mainContainer.setPosition(panelX, panelY);
    mainContainer.setOutlineThickness(UIHelpers::scaleSize(sf::Vector2f(2.f, 2.f), windowSize).x);

    // Glow effect réduit
    containerGlow.setSize(sf::Vector2f(panelWidth + 8.f, panelHeight + 8.f));
    containerGlow.setPosition(panelX - 4.f, panelY - 4.f);
    containerGlow.setOutlineThickness(UIHelpers::scaleSize(sf::Vector2f(3.f, 3.f), windowSize).x);

    containerShadow.setSize(mainContainer.getSize());
    containerShadow.setPosition(mainContainer.getPosition() + sf::Vector2f(shadowOffset, shadowOffset));

    // Piece set container avec espacement réduit
    float sectionGap = verticalSpacing * 1.2f; // Réduits de 3.f à 1.5f
    float pieceWidth = windowSize.x * 0.52f;
    pieceWidth = std::max(420.f, std::min(pieceWidth, windowSize.x * 0.55f));
    
    float pieceHeight = windowSize.y * 0.20f; // Augmenté de 0.18f à 0.20f pour éviter le chevauchement
    pieceHeight = std::max(160.f, std::min(pieceHeight, windowSize.y * 0.24f));
    
    float pieceX = (windowSize.x - pieceWidth) / 2.f;
    float pieceY = panelY + panelHeight + sectionGap;

    pieceSetContainer.setSize(sf::Vector2f(pieceWidth, pieceHeight));
    pieceSetContainer.setPosition(pieceX, pieceY);
    pieceSetContainer.setOutlineThickness(UIHelpers::scaleSize(sf::Vector2f(1.f, 1.f), windowSize).x);

    pieceSetContainerShadow.setSize(pieceSetContainer.getSize());
    pieceSetContainerShadow.setPosition(pieceSetContainer.getPosition() + sf::Vector2f(shadowOffset, shadowOffset));

    float piecePadding = std::max(14.f, pieceWidth * 0.04f);
    pieceSetTitle.setCharacterSize(UIHelpers::scaleFont(18.f, windowSize));
    pieceSetTitle.setPosition(pieceX + piecePadding, pieceY + piecePadding * 0.8f);

    pieceSetSubtitle.setCharacterSize(UIHelpers::scaleFont(14.f, windowSize));
    pieceSetSubtitle.setPosition(pieceSetTitle.getPosition().x, 
        pieceSetTitle.getPosition().y + pieceSetTitle.getLocalBounds().height + smallSpacing * 1.5f);

    if (interRegular) {
        createPieceSetCards(*interRegular, windowSize);
    }

    // Theme container avec hauteur augmentée pour tout afficher
    float themeWidth = pieceWidth * 0.95f;
    float themeHeight = 220.f;
    float themeX = (windowSize.x - themeWidth) / 2.f;
    float themeY = pieceY + pieceHeight + verticalSpacing * 0.8f; // Espacement réduit

    float availableThemeHeight = windowSize.y - margin - themeY;
    if (availableThemeHeight < 0.f) {
        availableThemeHeight = 0.f;
    }
    if (themeHeight > availableThemeHeight && availableThemeHeight > 0.f) {
        themeHeight = availableThemeHeight;
    }
    float minThemeHeight = 200.f; // AUGMENTÉ de 150f à 200f
    if (themeHeight > 0.f && themeHeight < minThemeHeight && availableThemeHeight >= minThemeHeight) {
        themeHeight = minThemeHeight;
    }

    themeContainer.setSize(sf::Vector2f(themeWidth, themeHeight));
    themeContainer.setPosition(themeX, themeY);
    themeContainer.setOutlineThickness(UIHelpers::scaleSize(sf::Vector2f(1.f, 1.f), windowSize).x);

    themeContainerShadow.setSize(themeContainer.getSize());
    themeContainerShadow.setPosition(themeContainer.getPosition() + sf::Vector2f(shadowOffset, shadowOffset));

    float themePadding = std::max(12.f, themeWidth * 0.04f); // Réduit le padding
    themeTitle.setCharacterSize(UIHelpers::scaleFont(18.f, windowSize));
    sf::FloatRect themeTitleBounds = themeTitle.getLocalBounds();
    themeTitle.setPosition(themeX + themePadding, themeY + themePadding * 0.7f); // Padding top réduit

    themeSubtitle.setCharacterSize(UIHelpers::scaleFont(14.f, windowSize));
    themeSubtitle.setPosition(themeTitle.getPosition().x, 
        themeTitle.getPosition().y + themeTitleBounds.height + smallSpacing * 1.2f); // Espacement réduit

    if (interRegular) {
        createMiniBoards(*interRegular, windowSize);
    }
}

void MainMenuScreen::createPieceSetCards(sf::Font& font, const sf::Vector2f& windowSize) {
    float padding = std::max(14.f, pieceSetContainer.getSize().x * 0.04f);
    float cardSpacing = std::max(15.f, pieceSetContainer.getSize().x * 0.045f);
    float availableWidth = pieceSetContainer.getSize().x - 2.f * padding;
    float cardWidth = (availableWidth - cardSpacing) / 2.f;
    
    // AUGMENTÉ la hauteur de base de 0.14f à 0.16f (hauteur +14%)
    float baseCardHeight = windowSize.y * 0.16f;
    baseCardHeight = std::max(120.f, std::min(baseCardHeight, pieceSetContainer.getSize().y - padding * 1.2f));

    std::string labels[2] = { "Classic", "Themed" };

    float totalCardsWidth = cardWidth * 2.f + cardSpacing;
    float startX = pieceSetContainer.getPosition().x + (pieceSetContainer.getSize().x - totalCardsWidth) / 2.f;
    
    float cardsY = pieceSetSubtitle.getPosition().y + pieceSetSubtitle.getLocalBounds().height + padding * 0.9f;

    float maxAvailableHeight = pieceSetContainer.getPosition().y + pieceSetContainer.getSize().y - cardsY - padding * 0.6f;
    float cardHeight = std::min(baseCardHeight, maxAvailableHeight);

    // RÉDUIT la largeur des cartes de 15% pour plus d'espace
    cardWidth *= 0.85f;
    
    // Recalculer le positionnement avec la nouvelle largeur
    totalCardsWidth = cardWidth * 2.f + cardSpacing;
    startX = pieceSetContainer.getPosition().x + (pieceSetContainer.getSize().x - totalCardsWidth) / 2.f;

    // Dimensions des previews de pièces
    float previewSize = 28.f;
    float previewSpacing = std::max(18.f, previewSize * 0.6f);
    float previewRowWidth = previewSize * 3.f + previewSpacing * 2.f;

    for (int i = 0; i < 2; ++i) {
        auto& card = pieceSetCards[i];
        float x = startX + i * (cardWidth + cardSpacing);
        float previewStartX = x + (cardWidth - previewRowWidth) / 2.f;
        
        // Position des pièces avec plus d'espace vertical grâce à la hauteur augmentée
        float previewY = cardsY + 18.f; // Augmenté de 15f à 18f pour centrer verticalement

        // Style de carte
        card.frame.setSize(sf::Vector2f(cardWidth, cardHeight));
        card.frame.setPosition(x, cardsY);
        card.frame.setFillColor(sf::Color(50, 38, 30));

        card.border.setSize(sf::Vector2f(cardWidth, cardHeight));
        card.border.setPosition(x, cardsY);
        card.border.setFillColor(sf::Color::Transparent);
        card.border.setOutlineThickness(UIHelpers::scaleSize(sf::Vector2f(2.f, 2.f), windowSize).x);
        card.border.setOutlineColor(sf::Color(140, 110, 90));

        // Label - positionné plus bas grâce à la hauteur augmentée
        card.label.setFont(font);
        card.label.setString(labels[i]);
        card.label.setCharacterSize(UIHelpers::scaleFont(14.f, windowSize));
        card.label.setFillColor(sf::Color(255, 255, 255));
        sf::FloatRect textBounds = card.label.getLocalBounds();
        card.label.setPosition(
            x + (cardWidth - textBounds.width) / 2.f,
            cardsY + cardHeight - 24.f // Ajusté pour la nouvelle hauteur
        );

        card.clickArea = sf::FloatRect(x, cardsY, cardWidth, cardHeight);

        // Effet de sélection
        card.selectionGlow.setSize(sf::Vector2f(cardWidth + 4.f, cardHeight + 4.f));
        card.selectionGlow.setPosition(x - 2.f, cardsY - 2.f);
        card.selectionGlow.setFillColor(sf::Color::Transparent);
        card.selectionGlow.setOutlineThickness(UIHelpers::scaleSize(sf::Vector2f(3.f, 3.f), windowSize).x);
        card.selectionGlow.setOutlineColor(sf::Color::Transparent);

        // Slots transparents pour les pièces
        for (int j = 0; j < 3; ++j) {
            float slotX = previewStartX + j * (previewSize + previewSpacing);
            
            card.previewSlots[j].setSize(sf::Vector2f(previewSize, previewSize));
            card.previewSlots[j].setPosition(slotX, previewY);
            card.previewSlots[j].setFillColor(sf::Color::Transparent);
            card.previewSlots[j].setOutlineThickness(0.f);
            card.previewSlots[j].setOutlineColor(sf::Color::Transparent);
            
            // Position des sprites de pièces
            const sf::Texture* tex = card.previewSprites[j].getTexture();
            if (tex) {
                float texW = static_cast<float>(tex->getSize().x);
                float texH = static_cast<float>(tex->getSize().y);
                float scale = previewSize / std::max(texW, texH);
                card.previewSprites[j].setScale(scale, scale);
                card.previewSprites[j].setPosition(
                    slotX + (previewSize - texW * scale) / 2.f,
                    previewY + (previewSize - texH * scale) / 2.f
                );
                card.previewSprites[j].setColor(sf::Color(255, 255, 255, 255));
            }

            card.previewGlyphs[j].setString("");
        }
    }
}

void MainMenuScreen::loadPieceSetPreviews() {
    TextureManager* texManager = screenManager ? screenManager->getTextureManager() : nullptr;
    if (!texManager) return;
    
    // ===== CLASSIC SET (Index 0) =====
    // Utilise les pièces BLANCHES standard dans assets/
    sf::Texture* wKing = texManager->getTexture("w_king");
    sf::Texture* wQueen = texManager->getTexture("w_queen");
    sf::Texture* wPawn = texManager->getTexture("w_pawn");
    
    if (wKing) {
        pieceSetCards[0].previewSprites[0].setTexture(*wKing);
        pieceSetCards[0].previewSprites[0].setColor(sf::Color(255, 255, 255, 255));
    }
    if (wQueen) {
        pieceSetCards[0].previewSprites[1].setTexture(*wQueen);
        pieceSetCards[0].previewSprites[1].setColor(sf::Color(255, 255, 255, 255));
    }
    if (wPawn) {
        pieceSetCards[0].previewSprites[2].setTexture(*wPawn);
        pieceSetCards[0].previewSprites[2].setColor(sf::Color(255, 255, 255, 255));
    }

    // ===== THEMED SET (Index 1) =====
    // TextureManager ajoute "assets/" automatiquement, donc on utilise "../pieces/" pour remonter
    
    // Déterminer le dossier du thème pour les pièces noires
    std::string themedFolder;
    switch (selectedTheme) {
        case BoardTheme::Wooden:
            themedFolder = "wooden";
            break;
        case BoardTheme::RedWine:
            themedFolder = "redwine";
            break;
        case BoardTheme::BlueSky:
            themedFolder = "bluesky";
            break;
        default:
            themedFolder = "wooden";
            break;
    }
    
    // TextureManager préfixe avec "assets/", donc "../pieces/" devient "assets/../pieces/" = "pieces/"
    std::string whiteKingPath = "../pieces/player white/king.png";
    std::string whiteQueenPath = "../pieces/player white/queen.png";
    std::string whitePawnPath = "../pieces/player white/pawn.png";
    
    std::string blackKingPath = "../pieces/" + themedFolder + "/king.png";
    std::string blackQueenPath = "../pieces/" + themedFolder + "/queen.png";
    std::string blackPawnPath = "../pieces/" + themedFolder + "/pawn.png";
    
    // Noms uniques pour le texture manager
    std::string whiteKingName = "themed_white_king";
    std::string whiteQueenName = "themed_white_queen";
    std::string whitePawnName = "themed_white_pawn";
    std::string blackKingName = "themed_" + themedFolder + "_king";
    std::string blackQueenName = "themed_" + themedFolder + "_queen";
    std::string blackPawnName = "themed_" + themedFolder + "_pawn";
    
    // Charger les textures si pas déjà chargées
    if (!texManager->hasTexture(whiteKingName)) {
        texManager->loadTexture(whiteKingName, whiteKingPath);
    }
    if (!texManager->hasTexture(whiteQueenName)) {
        texManager->loadTexture(whiteQueenName, whiteQueenPath);
    }
    if (!texManager->hasTexture(whitePawnName)) {
        texManager->loadTexture(whitePawnName, whitePawnPath);
    }
    if (!texManager->hasTexture(blackKingName)) {
        texManager->loadTexture(blackKingName, blackKingPath);
    }
    if (!texManager->hasTexture(blackQueenName)) {
        texManager->loadTexture(blackQueenName, blackQueenPath);
    }
    if (!texManager->hasTexture(blackPawnName)) {
        texManager->loadTexture(blackPawnName, blackPawnPath);
    }
    
    // AFFICHER un MIX des deux couleurs pour montrer la variété :
    // Roi blanc, Dame noire, Pion blanc (pour montrer que ça varie)
    sf::Texture* themedKing = texManager->getTexture(whiteKingName);  // Blanc
    sf::Texture* themedQueen = texManager->getTexture(blackQueenName); // Noir (varie selon thème)
    sf::Texture* themedPawn = texManager->getTexture(whitePawnName);  // Blanc
    
    if (themedKing) {
        pieceSetCards[1].previewSprites[0].setTexture(*themedKing);
        pieceSetCards[1].previewSprites[0].setColor(sf::Color(255, 255, 255, 255));
    }
    if (themedQueen) {
        pieceSetCards[1].previewSprites[1].setTexture(*themedQueen);
        pieceSetCards[1].previewSprites[1].setColor(sf::Color(255, 255, 255, 255));
    }
    if (themedPawn) {
        pieceSetCards[1].previewSprites[2].setTexture(*themedPawn);
        pieceSetCards[1].previewSprites[2].setColor(sf::Color(255, 255, 255, 255));
    }
    
    // Debug: vérifier quelles textures ont été chargées
    std::cout << "[MainMenuScreen] Piece Set Previews Loaded:" << std::endl;
    std::cout << "  Classic set (WHITE): King=" << (wKing ? "OK" : "Missing") 
              << ", Queen=" << (wQueen ? "OK" : "Missing") 
              << ", Pawn=" << (wPawn ? "OK" : "Missing") << std::endl;
    std::cout << "  Themed set (" << themedFolder << "): WKing=" << (themedKing ? "OK" : "Missing") 
              << ", BQueen=" << (themedQueen ? "OK" : "Missing") 
              << ", WPawn=" << (themedPawn ? "OK" : "Missing") << std::endl;
}

void MainMenuScreen::createMiniBoards(sf::Font& font, const sf::Vector2f& windowSize) {
    if (themeContainer.getSize().y <= 0.f || themeContainer.getSize().x <= 0.f) {
        return;
    }
    float padding = std::max(12.f, themeContainer.getSize().x * 0.04f); // Padding réduit
    float availableWidth = themeContainer.getSize().x - 2.f * padding;
    float boardSpacing = std::max(15.f, availableWidth * 0.04f); // Espacement optimisé
    float availableForBoards = availableWidth - 2.f * boardSpacing;
    if (availableForBoards < 0.f) {
        boardSpacing = std::max(12.f, availableWidth * 0.03f);
        availableForBoards = std::max(0.f, availableWidth - 2.f * boardSpacing);
    }
    
    // Taille optimisée pour les mini-plateaux - AUGMENTÉ pour meilleure visibilité
    float boardSize = 95.f; // Augmenté de 90f à 95f
    
    float labelSpacing = std::max(10.f, windowSize.y * 0.013f);
    
    float boardsTop = themeSubtitle.getPosition().y + themeSubtitle.getLocalBounds().height + padding * 0.6f; // Espacement réduit
    
    float totalBoardWidth = boardSize * 3.f + boardSpacing * 2.f;
    float startX = themeContainer.getPosition().x + (themeContainer.getSize().x - totalBoardWidth) / 2.f;

    BoardTheme themes[3] = { BoardTheme::Wooden, BoardTheme::RedWine, BoardTheme::BlueSky };
    std::string labels[3] = { "Wooden", "Red Wine", "Blue Sky" };

    // Schémas de couleurs optimisés
    sf::Color lightColors[3] = {
        sf::Color(245, 222, 186), // Wooden light
        sf::Color(235, 189, 183), // RedWine light
        sf::Color(183, 226, 240)  // BlueSky light
    };
    sf::Color darkColors[3] = {
        sf::Color(176, 131, 94),  // Wooden dark
        sf::Color(149, 79, 79),   // RedWine dark
        sf::Color(80, 140, 190)   // BlueSky dark
    };

    for (int t = 0; t < 3; ++t) {
        auto& preview = themePreviews[t];
        float x = startX + t * (boardSize + boardSpacing);

        // Cadre du plateau
        preview.frame.setSize(sf::Vector2f(boardSize, boardSize));
        preview.frame.setPosition(x, boardsTop);
        preview.frame.setFillColor(sf::Color::Transparent);

        preview.border.setSize(sf::Vector2f(boardSize, boardSize));
        preview.border.setPosition(x, boardsTop);
        preview.border.setFillColor(sf::Color::Transparent);
        preview.border.setOutlineThickness(UIHelpers::scaleSize(sf::Vector2f(2.f, 2.f), windowSize).x);
        preview.border.setOutlineColor(sf::Color(100, 80, 60));

        float squareSize = boardSize / 8.f;
        
        // Pattern d'échiquier
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                bool isLight = (row + col) % 2 == 0;
                preview.squares[row][col].setSize(sf::Vector2f(squareSize, squareSize));
                preview.squares[row][col].setPosition(x + col * squareSize, boardsTop + row * squareSize);
                preview.squares[row][col].setFillColor(isLight ? lightColors[t] : darkColors[t]);
                preview.squares[row][col].setOutlineThickness(0.2f); // Bordures très fines
                preview.squares[row][col].setOutlineColor(sf::Color(0, 0, 0, 15));
            }
        }

        // Pièces cachées
        for (int col = 0; col < 8; ++col) {
            preview.pieceSprites[col].setColor(sf::Color::Transparent);
        }

        // Label avec espacement optimisé
        preview.label.setFont(font);
        preview.label.setString(labels[t]);
        preview.label.setCharacterSize(UIHelpers::scaleFont(14.f, windowSize)); // Légèrement augmenté
        preview.label.setFillColor(sf::Color(255, 255, 255));
        sf::FloatRect textBounds = preview.label.getLocalBounds();
        preview.label.setPosition(
            x + (boardSize - textBounds.width) / 2.f,
            boardsTop + boardSize + 10.f // Espacement optimisé
        );

        preview.clickArea = sf::FloatRect(x, boardsTop, boardSize, boardSize);

        // Effet de sélection
        preview.selectionGlow.setSize(sf::Vector2f(boardSize + 6.f, boardSize + 6.f));
        preview.selectionGlow.setPosition(x - 3.f, boardsTop - 3.f);
        preview.selectionGlow.setFillColor(sf::Color::Transparent);
        preview.selectionGlow.setOutlineThickness(UIHelpers::scaleSize(sf::Vector2f(3.f, 3.f), windowSize).x);
        preview.selectionGlow.setOutlineColor(sf::Color::Transparent);
    }
}

void MainMenuScreen::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) {
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        
        // King selection avec feedback amélioré
        if (btnWhiteKing.clicked(mousePos)) {
            whiteSelected = true;
            blackSelected = false;
            updateVisualFeedback();
            std::cout << "[MainMenuScreen] Selected White pieces" << std::endl;
        }
        else if (btnBlackKing.clicked(mousePos)) {
            whiteSelected = false;
            blackSelected = true;
            updateVisualFeedback();
            std::cout << "[MainMenuScreen] Selected Black pieces" << std::endl;
        }

        // Difficulty selection avec feedback amélioré
        else if (btnEasy.clicked(mousePos)) {
            selectedDifficulty = 1;
            updateVisualFeedback();
            std::cout << "[MainMenuScreen] Selected Easy difficulty" << std::endl;
        }
        else if (btnMedium.clicked(mousePos)) {
            selectedDifficulty = 2;
            updateVisualFeedback();
            std::cout << "[MainMenuScreen] Selected Medium difficulty" << std::endl;
        }
        else if (btnHard.clicked(mousePos)) {
            selectedDifficulty = 3;
            updateVisualFeedback();
            std::cout << "[MainMenuScreen] Selected Hard difficulty" << std::endl;
        }

        // Piece Set selection
        else if (checkPieceSetClicks(mousePos)) {
            // Handled in checkPieceSetClicks avec feedback console
        }

        // Theme selection
        else if (checkThemeClicks(mousePos)) {
            // Handled in checkThemeClicks avec feedback console
        }

        // Play button avec validation améliorée
        else if (btnPlayNow.clicked(mousePos)) {
            std::cout << "[MainMenuScreen] Starting game with settings:" << std::endl;
            std::cout << "  Color: " << (whiteSelected ? "White" : "Black") << std::endl;
            std::cout << "  Difficulty: " << selectedDifficulty << std::endl;
            std::cout << "  Piece Set: " << static_cast<int>(selectedPieceSet) << std::endl;
            std::cout << "  Board Theme: " << static_cast<int>(selectedTheme) << std::endl;
            
            UserData& userData = screenManager->getUserData();
            userData.setSelectedColor(whiteSelected ? "White" : "Black");
            userData.setSelectedDifficulty(selectedDifficulty);
            userData.setSelectedBoardTheme(selectedTheme);
            userData.setSelectedPieceSet(selectedPieceSet);
            screenManager->changeState(STATE_GAME_BOARD);
        }

        // Statistics button
        else if (btnStatistics.clicked(mousePos)) {
            std::cout << "[MainMenuScreen] Opening Statistics screen" << std::endl;
            screenManager->changeState(STATE_STATISTICS);
        }

        // Logout button
        else if (btnLogout.clicked(mousePos)) {
            std::cout << "[MainMenuScreen] Logging out user" << std::endl;
            screenManager->getUserData().reset();
            screenManager->changeState(STATE_LOGIN);
        }
    }
    
    // Gestion du survol pour améliorer l'UX
    else if (event.type == sf::Event::MouseMoved) {
        // Mise à jour du survol pour tous les boutons
        btnWhiteKing.updateHover(mousePos);
        btnBlackKing.updateHover(mousePos);
        btnEasy.updateHover(mousePos);
        btnMedium.updateHover(mousePos);
        btnHard.updateHover(mousePos);
        btnPlayNow.updateHover(mousePos);
        btnStatistics.updateHover(mousePos);
        btnLogout.updateHover(mousePos);
    }
}

bool MainMenuScreen::checkPieceSetClicks(const sf::Vector2i& mousePos) {
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // Vérification améliorée des clics avec feedback
    if (pieceSetCards[0].clickArea.contains(mousePosF)) {
        selectedPieceSet = PieceSetType::CLASSIC;
        updateVisualFeedback();
        std::cout << "[MainMenuScreen] Selected Classic piece set" << std::endl;
        return true;
    }
    else if (pieceSetCards[1].clickArea.contains(mousePosF)) {
        selectedPieceSet = PieceSetType::THEMED;
        updateVisualFeedback();
        std::cout << "[MainMenuScreen] Selected Themed piece set" << std::endl;
        return true;
    }
    return false;
}

bool MainMenuScreen::checkThemeClicks(const sf::Vector2i& mousePos) {
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    BoardTheme themes[3] = { BoardTheme::Wooden, BoardTheme::RedWine, BoardTheme::BlueSky };
    std::string themeNames[3] = { "Wooden", "RedWine", "BlueSky" };

    for (int i = 0; i < 3; ++i) {
        if (themePreviews[i].clickArea.contains(mousePosF)) {
            selectedTheme = themes[i];
            updateVisualFeedback();
            std::cout << "[MainMenuScreen] Selected " << themeNames[i] << " theme" << std::endl;
            return true;
        }
    }
    return false;
}

void MainMenuScreen::updateVisualFeedback() {
    sf::Vector2f windowSize = currentWindowSize;
    
    // Update king frames avec couleurs améliorées
    float thinOutline = UIHelpers::scaleSize(sf::Vector2f(3.f, 3.f), windowSize).x; // Augmenté
    float thickOutline = UIHelpers::scaleSize(sf::Vector2f(5.f, 5.f), windowSize).x; // Augmenté

    if (whiteSelected) {
        whiteKingFrame.setOutlineColor(sf::Color(173, 216, 230, 255)); // #ADD8E6 light blue
        whiteKingFrame.setOutlineThickness(thickOutline);
        whiteKingFrame.setFillColor(sf::Color::Transparent); // Keep transparent background
    }
    else {
        whiteKingFrame.setOutlineColor(sf::Color(140, 100, 70));
        whiteKingFrame.setOutlineThickness(thinOutline);
        whiteKingFrame.setFillColor(sf::Color::Transparent); // Keep transparent background
    }

    if (blackSelected) {
        blackKingFrame.setOutlineColor(sf::Color(173, 216, 230, 255)); // #ADD8E6 light blue
        blackKingFrame.setOutlineThickness(thickOutline);
        blackKingFrame.setFillColor(sf::Color::Transparent); // Keep transparent background
    }
    else {
        blackKingFrame.setOutlineColor(sf::Color(140, 100, 70));
        blackKingFrame.setOutlineThickness(thinOutline);
        blackKingFrame.setFillColor(sf::Color::Transparent); // Keep transparent background
    }

    // Update difficulty buttons avec couleurs améliorées
    sf::Color idleColor(85, 65, 50); // Harmonisé avec la nouvelle couleur
    sf::Color selectedColor(120, 95, 75); // Couleur de sélection plus contrastée
    sf::Color hoverColor(100, 80, 65); // Couleur intermédiaire pour l'effet hover

    btnEasy.setFillColor(selectedDifficulty == 1 ? selectedColor : idleColor);
    btnMedium.setFillColor(selectedDifficulty == 2 ? selectedColor : idleColor);
    btnHard.setFillColor(selectedDifficulty == 3 ? selectedColor : idleColor);

    // Update piece set cards avec effet bleu clair #ADD8E6
    for (int i = 0; i < 2; ++i) {
        PieceSetType cardType = (i == 0) ? PieceSetType::CLASSIC : PieceSetType::THEMED;
        if (cardType == selectedPieceSet) {
            pieceSetCards[i].selectionGlow.setOutlineColor(sf::Color(173, 216, 230, 220)); // #ADD8E6 light blue
            pieceSetCards[i].border.setOutlineColor(sf::Color(173, 216, 230, 180)); // #ADD8E6 light blue border
        }
        else {
            pieceSetCards[i].selectionGlow.setOutlineColor(sf::Color::Transparent);
            pieceSetCards[i].border.setOutlineColor(sf::Color(140, 110, 90)); // Couleur normale
        }
    }

    // Update theme previews avec effet bleu clair #ADD8E6
    BoardTheme themes[3] = { BoardTheme::Wooden, BoardTheme::RedWine, BoardTheme::BlueSky };
    for (int i = 0; i < 3; ++i) {
        if (themes[i] == selectedTheme) {
            themePreviews[i].selectionGlow.setOutlineColor(sf::Color(173, 216, 230, 220)); // #ADD8E6 light blue
            themePreviews[i].border.setOutlineColor(sf::Color(173, 216, 230, 180)); // #ADD8E6 light blue border
        }
        else {
            themePreviews[i].selectionGlow.setOutlineColor(sf::Color::Transparent);
            themePreviews[i].border.setOutlineColor(sf::Color(120, 100, 80)); // Couleur normale
        }
    }
}

void MainMenuScreen::update(float deltaTime) {
    // Animate Play button pulse timing
    playButtonPulseTime += deltaTime * playButtonPulseDirection * 1.5f;
    
    if (playButtonPulseTime >= 1.0f) {
        playButtonPulseTime = 1.0f;
        playButtonPulseDirection = -1.0f;
    }
    else if (playButtonPulseTime <= 0.0f) {
        playButtonPulseTime = 0.0f;
        playButtonPulseDirection = 1.0f;
    }
    
    // La couleur est appliquée dans draw() APRÈS applyResponsiveLayout()
}

void MainMenuScreen::draw(sf::RenderWindow& window) {
    // Appeler applyResponsiveLayout pour maintenir le layout
    applyResponsiveLayout(window.getSize());
    updateVisualFeedback();

    // Appliquer l'animation du bouton APRÈS applyResponsiveLayout
    float pulseIntensity = 0.85f + (playButtonPulseTime * 0.15f);
    sf::Color baseColor(74, 144, 226, 240);
    sf::Color pulsedColor(
        static_cast<sf::Uint8>(baseColor.r * pulseIntensity),
        static_cast<sf::Uint8>(baseColor.g * pulseIntensity),
        static_cast<sf::Uint8>(baseColor.b * pulseIntensity),
        baseColor.a
    );
    btnPlayNow.setFillColor(pulsedColor);

    // Background
    window.draw(background);

    // Header avec tous les éléments textuels
    window.draw(welcomeText);
    window.draw(chessGameSetupText);
    window.draw(levelText);
    btnStatistics.draw(window);
    btnLogout.draw(window);

    // Main container avec glow et ombre (ordre d'affichage important)
    window.draw(containerShadow);     // Ombre en arrière-plan
    window.draw(containerGlow);       // Glow autour du container
    window.draw(mainContainer);       // Container principal

    // Play as section avec icônes
    window.draw(playAsTitle);
    window.draw(whiteKingFrame);
    window.draw(blackKingFrame);
    if (whiteKingIcon.getTexture()) {
        window.draw(whiteKingIcon);
    }
    if (blackKingIcon.getTexture()) {
        window.draw(blackKingIcon);
    }

    // Difficulty section avec boutons
    window.draw(difficultyTitle);
    btnEasy.draw(window);
    btnMedium.draw(window);
    btnHard.draw(window);

    // Play button principal
    btnPlayNow.draw(window);

    // Piece Set Section avec ombre et container
    window.draw(pieceSetContainerShadow);
    window.draw(pieceSetContainer);
    window.draw(pieceSetTitle);
    window.draw(pieceSetSubtitle);

    // Cartes de jeux de pièces avec ordre d'affichage optimisé
    for (int i = 0; i < 2; ++i) {
        auto& card = pieceSetCards[i];
        
        // Dessiner l'effet de sélection en premier (arrière-plan)
        window.draw(card.selectionGlow);
        
        // Dessiner le cadre de la carte
        window.draw(card.frame);
        window.draw(card.border);
        
        // *** NE PAS DESSINER les slots d'arrière-plan (ils sont transparents) ***
        // Les previewSlots sont maintenant transparents et ne doivent pas être dessinés
        
        // Dessiner SEULEMENT les sprites de pièces (sans arrière-plan)
        for (int j = 0; j < 3; ++j) {
            if (card.previewSprites[j].getTexture()) {
                window.draw(card.previewSprites[j]);
            }
        }
        
        // Dessiner le label en dernier pour qu'il soit toujours visible
        window.draw(card.label);
    }

    // Board Theme Section avec ombre et container
    window.draw(themeContainerShadow);
    window.draw(themeContainer);
    window.draw(themeTitle);
    window.draw(themeSubtitle);

    // Mini-plateaux de thèmes avec ordre d'affichage optimisé
    for (int t = 0; t < 3; ++t) {
        auto& preview = themePreviews[t];
        
        // Dessiner l'effet de sélection amélioré
        window.draw(preview.selectionGlow);
        
        // Dessiner les cases de l'échiquier
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                window.draw(preview.squares[row][col]);
            }
        }
        
        // Dessiner la bordure du plateau
        window.draw(preview.border);
        
        // Dessiner le label du thème
        window.draw(preview.label);
    }
}

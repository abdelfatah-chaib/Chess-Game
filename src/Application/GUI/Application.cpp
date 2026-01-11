#include "Application.h"
#include "AppState.h"
#include "TitleScreen.h"
#include "GetStartedScreen.h"
#include "CreateAccountScreen.h"
#include "EmailPasswordScreen.h"
#include "UsernameScreen.h"         
#include "MainMenuScreen.h"
#include "GameBoardScreen.h"
#include "ConfirmationScreen.h"
#include "LoginScreen.h"
#include "GUI/StatisticsScreen.h"
#include "ForgotPasswordScreen.h"
#include <iostream>
#include <filesystem>

// Windows-specific
#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

Application::Application()
    : window(sf::VideoMode(1200, 800), "ChessMaster UI", sf::Style::Default) { 
    
    std::cout << "[Application] Creating window: " << 1200 << "x" << 800 << std::endl;
    
    // Permettre le redimensionnement
    window.setFramerateLimit(60);

    // Use relative assets path
    std::string basePath = "assets/";

    try {
        fs::path cwd = fs::current_path();
        fs::path resolvedAssets = fs::absolute(cwd / basePath);
        std::cout << "[Application] Current working directory: " << cwd.string() << std::endl;
        std::cout << "[Application] Asset base path (relative): " << basePath << std::endl;
        std::cout << "[Application] Resolved asset directory: " << resolvedAssets.string() << std::endl;
        if (!fs::exists(resolvedAssets)) {
            std::cout << "[Application] Warning: Resolved asset directory does not exist." << std::endl;
            std::cout << "[Application] Ensure 'assets' folder exists and contains required files." << std::endl;
        } else {
            std::cout << "[Application] ✓ Assets directory found!" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "[Application] Filesystem diagnostic failed: " << e.what() << std::endl;
    }

    std::cout << "[Application] Creating managers..." << std::endl;
    textureManager = std::make_unique<TextureManager>(basePath);
    fontManager = std::make_unique<FontManager>(basePath);
    gameController = std::make_unique<GameController>();

    std::cout << "[Application] Loading resources..." << std::endl;
    loadResources();

    std::cout << "[Application] Creating ScreenManager..." << std::endl;
    screenManager = std::make_unique<ScreenManager>(textureManager.get(), fontManager.get());
    
    // Initialize database
    std::cout << "[Application] Initializing database..." << std::endl;
    screenManager->initDatabase("chessgame.db");
    
    std::cout << "[Application] Initializing screens..." << std::endl;
    initializeScreens();
    
    // Check for auto-login
    checkAutoLogin();
    
    std::cout << "[Application] Initialization complete!" << std::endl;
}

void Application::loadResources() {
    // Load fonts Professional Typography System
    std::cout << "[Resources] Loading typography system..." << std::endl;
    
    // Inter (Primary UI Font) 3 weights
    fontManager->loadFont("inter-regular", "Inter-Regular.ttf");
    fontManager->loadFont("inter-medium", "Inter-Medium.ttf");
    fontManager->loadFont("inter-semibold", "Inter-SemiBold.ttf");
    
    // Cinzel (Branding & Titles)  2 weights
    fontManager->loadFont("cinzel-regular", "Cinzel-Regular.ttf");
    fontManager->loadFont("cinzel-bold", "Cinzel-Bold.ttf");
    
    // JetBrains Mono (Monospace for numbers/timers)
    fontManager->loadFont("jetbrains-mono", "JetBrainsMono-Regular.ttf");

    // Load textures
    std::cout << "[Resources] Loading textures..." << std::endl;
    textureManager->loadTexture("bg_title", "bg_title.jpg");
    textureManager->loadTexture("bg_getstarted", "bg_getstarted.png");
    textureManager->loadTexture("bg_create", "bgk.jpg");
    textureManager->loadTexture("bg_email", "bgk.jpg");
    textureManager->loadTexture("bg_username", "bgk.jpg");
    textureManager->loadTexture("bg_main_menu", "dramatic-chess-piece.jpg");
    textureManager->loadTexture("dramatic_chess", "dramatic-chess-piece.jpg");
    textureManager->loadTexture("white_pawn", "white_pawn.png");
    textureManager->loadTexture("black_pawn", "black_pawn.png");
    textureManager->loadTexture("bgk", "bgk.jpg");
    
    // Load background for login and signup screens
    std::cout << "[Resources] Loading login/signup background..." << std::endl;
    textureManager->loadTexture("bgg", "bg_title.jpg");
    
    // Load ALL white chess piece icons for Main Menu previews
    std::cout << "[Resources] Loading white chess pieces for Main Menu..." << std::endl;
    textureManager->loadTexture("w_king", "w_king.png");
    textureManager->loadTexture("w_queen", "w_queen.png");
    textureManager->loadTexture("w_bishop", "w_bishop.png");
    textureManager->loadTexture("w_knight", "w_knight.png");
    textureManager->loadTexture("w_rook", "w_rook.png");
    textureManager->loadTexture("w_pawn", "w_pawn.png");
    
    // Load black chess piece icons for Main Menu
    std::cout << "[Resources] Loading black chess pieces for Main Menu..." << std::endl;
    textureManager->loadTexture("b_king", "b_king.png");
    textureManager->loadTexture("b_queen", "b_queen.png");
    textureManager->loadTexture("b_bishop", "b_bishop.png");
    textureManager->loadTexture("b_knight", "b_knight.png");
    textureManager->loadTexture("b_rook", "b_rook.png");
    textureManager->loadTexture("b_pawn", "b_pawn.png");
    
    std::cout << "[Resources] ✓ All resources loaded successfully!" << std::endl;
}

void Application::initializeScreens() {
    // Register all screens
    std::cout << "[Screens] Registering TitleScreen..." << std::endl;
    screenManager->registerScreen(STATE_TITLE,
        std::make_unique<TitleScreen>(screenManager.get()));

    std::cout << "[Screens] Registering GetStartedScreen..." << std::endl;
    screenManager->registerScreen(STATE_GETSTARTED,
        std::make_unique<GetStartedScreen>(screenManager.get()));

    std::cout << "[Screens] Registering LoginScreen..." << std::endl;
    screenManager->registerScreen(STATE_LOGIN,
        std::make_unique<LoginScreen>(screenManager.get()));

    std::cout << "[Screens] Registering ForgotPasswordScreen..." << std::endl;
    screenManager->registerScreen(STATE_FORGOT_PASSWORD,
        std::make_unique<ForgotPasswordScreen>(screenManager.get()));

    std::cout << "[Screens] Registering CreateAccountScreen..." << std::endl;
    screenManager->registerScreen(STATE_CREATE_ACCOUNT,
        std::make_unique<CreateAccountScreen>(screenManager.get()));

    std::cout << "[Screens] Registering EmailPasswordScreen..." << std::endl;
    screenManager->registerScreen(STATE_EMAIL_PASSWORD,
        std::make_unique<EmailPasswordScreen>(screenManager.get()));

    std::cout << "[Screens] Registering UsernameScreen..." << std::endl;
    screenManager->registerScreen(STATE_USERNAME,
        std::make_unique<UsernameScreen>(screenManager.get()));

    std::cout << "[Screens] Registering MainMenuScreen..." << std::endl;
    screenManager->registerScreen(STATE_MAIN_MENU,
        std::make_unique<MainMenuScreen>(screenManager.get()));

    std::cout << "[Screens] Registering StatisticsScreen..." << std::endl;
    screenManager->registerScreen(STATE_STATISTICS,
        std::make_unique<StatisticsScreen>(screenManager.get()));

    std::cout << "[Screens] Registering GameBoardScreen..." << std::endl;
    screenManager->registerScreen(STATE_GAME_BOARD,
        std::make_unique<GameBoardScreen>(screenManager.get(), gameController.get()));

    // Success confirmation (new design used after account creation)
    std::cout << "[Screens] Registering ConfirmationScreen..." << std::endl;
    screenManager->registerScreen(STATE_CONFIRMATION,
        std::make_unique<ConfirmationScreen>(screenManager.get()));

    // Start at title screen (will be overridden by auto-login if applicable)
    std::cout << "[Screens] Changing to TitleScreen (STATE_TITLE)..." << std::endl;
    screenManager->changeState(STATE_TITLE);

    std::cout << "All screens registered successfully!" << std::endl;
}

void Application::checkAutoLogin() {
    std::cout << "[Application] Checking for auto-login..." << std::endl;
    
    SQLiteManager& db = screenManager->getDatabaseManager();
    int userId = db.getLastStayLoggedInUserId();
    
    if (userId > 0) {
        std::string username = db.getUsernameById(userId);
        
        if (!username.empty()) {
            std::cout << "[Application] ✓ Auto-login successful for user: " << username << " (ID: " << userId << ")" << std::endl;
            
            // Set user data
            UserData& user = screenManager->getUserData();
            user.id = userId;
            user.setUsername(username);
            user.stayLoggedIn = true;
            
            // Go directly to main menu
            screenManager->changeState(STATE_MAIN_MENU);
        } else {
            std::cout << "[Application] ✗ Failed to retrieve username for user ID: " << userId << std::endl;
        }
    } else {
        std::cout << "[Application] No stay logged in user found" << std::endl;
    }
}

void Application::processEvents() {
    sf::Event event;
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            std::cout << "[Application] Window close requested" << std::endl;
            window.close();
        }
        
        // Gérer le redimensionnement de la fenêtre
        if (event.type == sf::Event::Resized) {
            // Mettre à jour la vue pour correspondre à la nouvelle taille
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            window.setView(sf::View(visibleArea));
        }

        screenManager->handleEvent(event, mousePos);
    }
}

void Application::update() {
    float deltaTime = deltaClock.restart().asSeconds();
    screenManager->update(deltaTime);

    // Only update game controller when in game board state
    if (screenManager->getCurrentState() == STATE_GAME_BOARD) {
        gameController->update(deltaTime);
    }
}

void Application::render() {
    window.clear(sf::Color(8, 12, 20));
    screenManager->draw(window);
    window.display();
}

void Application::run() {
    std::cout << "ChessMaster UI started!" << std::endl;
    std::cout << "Navigate through screens to play chess." << std::endl;
    std::cout << "[Application] Entering main loop..." << std::endl;

    int frameCount = 0;
    while (window.isOpen()) {
        processEvents();
        update();
        render();
        
        // Log first 3 frames to verify rendering
        if (frameCount < 3) {
            std::cout << "[Application] Frame " << frameCount << " rendered" << std::endl;
            frameCount++;
        }
    }

    std::cout << "ChessMaster UI closed." << std::endl;
}

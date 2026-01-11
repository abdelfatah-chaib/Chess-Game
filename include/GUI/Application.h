#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "ScreenManager.h"
#include "TextureManager.h"
#include "FontManager.h"
#include "GameController.h"

class Application {
private:
    sf::RenderWindow window;
    std::unique_ptr<TextureManager> textureManager;
    std::unique_ptr<FontManager> fontManager;
    std::unique_ptr<ScreenManager> screenManager;
    std::unique_ptr<GameController> gameController;
    
    sf::Clock deltaClock;
    
    const unsigned WINDOW_W = 900;
    const unsigned WINDOW_H = 520;

    void loadResources();
    void initializeScreens();
    void checkAutoLogin();
    void processEvents();
    void update();
    void render();

public:
    Application();
    ~Application() = default;

    void run();
};

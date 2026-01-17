#include "Services/SoundManager.h"
#include <iostream>

SoundManager::SoundManager() : soundsLoaded(false), assetsPath("assets/") {
    std::cout << "[SoundManager] Initializing sound system..." << std::endl;
}

bool SoundManager::loadSounds(const std::string& basePath) {
    assetsPath = basePath;
    std::cout << "[SoundManager] Loading sounds from: " << assetsPath << std::endl;
    
    bool allLoaded = true;
    
    // Load move sound
    if (!moveBuffer.loadFromFile(assetsPath + "move.wav")) {
        std::cerr << "[SoundManager] ERROR: Failed to load move.wav" << std::endl;
        allLoaded = false;
    } else {
        moveSound.setBuffer(moveBuffer);
        std::cout << "[SoundManager] ✓ Loaded move.wav" << std::endl;
    }
    
    // Load check sound
    if (!checkBuffer.loadFromFile(assetsPath + "check.wav")) {
        std::cerr << "[SoundManager] ERROR: Failed to load check.wav" << std::endl;
        allLoaded = false;
    } else {
        checkSound.setBuffer(checkBuffer);
        std::cout << "[SoundManager] ✓ Loaded check.wav" << std::endl;
    }
    
    // Load game over sound
    if (!gameOverBuffer.loadFromFile(assetsPath + "gameover.wav")) {
        std::cerr << "[SoundManager] ERROR: Failed to load gameover.wav" << std::endl;
        allLoaded = false;
    } else {
        gameOverSound.setBuffer(gameOverBuffer);
        std::cout << "[SoundManager] ✓ Loaded gameover.wav" << std::endl;
    }
    
    // Load time warning sound
    if (!timeWarningBuffer.loadFromFile(assetsPath + "time_warning.wav")) {
        std::cerr << "[SoundManager] ERROR: Failed to load time_warning.wav" << std::endl;
        allLoaded = false;
    } else {
        timeWarningSound.setBuffer(timeWarningBuffer);
        std::cout << "[SoundManager] ✓ Loaded time_warning.wav" << std::endl;
    }
    
    soundsLoaded = allLoaded;
    
    if (soundsLoaded) {
        std::cout << "[SoundManager] All sounds loaded successfully!" << std::endl;
    } else {
        std::cerr << "[SoundManager] WARNING: Some sounds failed to load" << std::endl;
    }
    
    return soundsLoaded;
}

void SoundManager::playMove() {
    if (!soundsLoaded) return;
    
    // Stop previous move sound if still playing (prevents overlap)
    if (moveSound.getStatus() == sf::Sound::Playing) {
        moveSound.stop();
    }
    
    moveSound.play();
    std::cout << "[SoundManager] Playing move sound" << std::endl;
}

void SoundManager::playCheck() {
    if (!soundsLoaded) return;
    
    // Stop previous check sound if still playing
    if (checkSound.getStatus() == sf::Sound::Playing) {
        checkSound.stop();
    }
    
    checkSound.play();
    std::cout << "[SoundManager] Playing check sound" << std::endl;
}

void SoundManager::playGameOver() {
    if (!soundsLoaded) return;
    
    // Stop previous game over sound if still playing
    if (gameOverSound.getStatus() == sf::Sound::Playing) {
        gameOverSound.stop();
    }
    
    gameOverSound.play();
    std::cout << "[SoundManager] Playing game over sound" << std::endl;
}

void SoundManager::playTimeWarning() {
    if (!soundsLoaded) return;
    
    // Stop previous time warning sound if still playing
    if (timeWarningSound.getStatus() == sf::Sound::Playing) {
        timeWarningSound.stop();
    }
    
    timeWarningSound.play();
    std::cout << "[SoundManager] Playing time warning sound" << std::endl;
}

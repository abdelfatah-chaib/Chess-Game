#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <memory>

class SoundManager {
private:
    // Sound buffers (loaded once)
    sf::SoundBuffer moveBuffer;
    sf::SoundBuffer checkBuffer;
    sf::SoundBuffer gameOverBuffer;
    sf::SoundBuffer timeWarningBuffer;
    
    // Sound objects (for playback)
    sf::Sound moveSound;
    sf::Sound checkSound;
    sf::Sound gameOverSound;
    sf::Sound timeWarningSound;
    
    bool soundsLoaded;
    std::string assetsPath;

public:
    SoundManager();
    ~SoundManager() = default;
    
    // Load all sound files
    bool loadSounds(const std::string& basePath = "assets/");
    
    // Play sounds (non-blocking, event-driven)
    void playMove();
    void playCheck();
    void playGameOver();
    void playTimeWarning();
    
    // Getters
    bool areSoundsLoaded() const { return soundsLoaded; }
};

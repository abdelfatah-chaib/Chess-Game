#pragma once
#include <string>

// Player clock structure
struct PlayerClock {
    float remainingSeconds;  // Time remaining in seconds
    bool isRunning;          // Is this clock currently running?
    
    PlayerClock() : remainingSeconds(600.0f), isRunning(false) {}
    
    // Get formatted time as MM:SS
    std::string getFormattedTime() const;
    
    // Check if time is running out (warning states)
    bool isCritical() const { return remainingSeconds <= 60.0f; }      // Red: <= 1 minute
    bool isWarning() const { return remainingSeconds <= 180.0f; }      // Yellow: <= 3 minutes
    bool isVeryLow() const { return remainingSeconds <= 10.0f; }       // Flashing red: <= 10 seconds
    bool isTimeOut() const { return remainingSeconds <= 0.0f; }
};

// Chess clock manager - handles both player clocks
class ChessClock {
private:
    PlayerClock whiteClock;
    PlayerClock blackClock;
    bool isPaused;

public:
    ChessClock();
    
    // Clock control
    void startWhiteClock();
    void startBlackClock();
    void stopWhiteClock();
    void stopBlackClock();
    void pauseAll();
    void resumeAll();
    void reset();
    
    // Update (call every frame with delta time)
    void update(float deltaTime);
    
    // Getters
    const PlayerClock& getWhiteClock() const { return whiteClock; }
    const PlayerClock& getBlackClock() const { return blackClock; }
    bool isWhiteTimeOut() const { return whiteClock.isTimeOut(); }
    bool isBlackTimeOut() const { return blackClock.isTimeOut(); }
    bool isPausedState() const { return isPaused; }
    
    // Switch clock on turn change
    void switchToWhite();
    void switchToBlack();
};

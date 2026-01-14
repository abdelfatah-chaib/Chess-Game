#include "Services/ChessClock.h"
#include <sstream>
#include <iomanip>
#include <iostream>

// PlayerClock implementation
std::string PlayerClock::getFormattedTime() const {
    // Ensure we don't show negative time
    float displaySeconds = remainingSeconds < 0.0f ? 0.0f : remainingSeconds;
    
    int totalSeconds = static_cast<int>(displaySeconds);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds;
    return oss.str();
}

// ChessClock implementation
ChessClock::ChessClock() : isPaused(false) {
    // Clocks are initialized with 600 seconds (10 minutes) by default in PlayerClock constructor
}

void ChessClock::startWhiteClock() {
    if (!isPaused && !whiteClock.isTimeOut()) {
        whiteClock.isRunning = true;
        blackClock.isRunning = false;
        std::cout << "[ChessClock] White clock started" << std::endl;
    }
}

void ChessClock::startBlackClock() {
    if (!isPaused && !blackClock.isTimeOut()) {
        blackClock.isRunning = true;
        whiteClock.isRunning = false;
        std::cout << "[ChessClock] Black clock started" << std::endl;
    }
}

void ChessClock::stopWhiteClock() {
    whiteClock.isRunning = false;
    std::cout << "[ChessClock] White clock stopped" << std::endl;
}

void ChessClock::stopBlackClock() {
    blackClock.isRunning = false;
    std::cout << "[ChessClock] Black clock stopped" << std::endl;
}

void ChessClock::pauseAll() {
    isPaused = true;
    whiteClock.isRunning = false;
    blackClock.isRunning = false;
    std::cout << "[ChessClock] All clocks paused" << std::endl;
}

void ChessClock::resumeAll() {
    isPaused = false;
    std::cout << "[ChessClock] Clocks resumed (manual start required)" << std::endl;
}

void ChessClock::reset() {
    whiteClock = PlayerClock();  // Reset to 600 seconds
    blackClock = PlayerClock();  // Reset to 600 seconds
    isPaused = false;
    std::cout << "[ChessClock] Clocks reset to 10:00" << std::endl;
}

void ChessClock::update(float deltaTime) {
    // Only update if not paused
    if (isPaused) {
        return;
    }
    
    // Update white clock if running
    if (whiteClock.isRunning && !whiteClock.isTimeOut()) {
        whiteClock.remainingSeconds -= deltaTime;
        
        // Log timeout
        if (whiteClock.remainingSeconds <= 0.0f) {
            whiteClock.remainingSeconds = 0.0f;
            whiteClock.isRunning = false;
            std::cout << "[ChessClock] WHITE TIME OUT! Remaining: " << whiteClock.getFormattedTime() << std::endl;
        }
    }
    
    // Update black clock if running
    if (blackClock.isRunning && !blackClock.isTimeOut()) {
        blackClock.remainingSeconds -= deltaTime;
        
        // Log timeout
        if (blackClock.remainingSeconds <= 0.0f) {
            blackClock.remainingSeconds = 0.0f;
            blackClock.isRunning = false;
            std::cout << "[ChessClock] BLACK TIME OUT! Remaining: " << blackClock.getFormattedTime() << std::endl;
        }
    }
}

void ChessClock::switchToWhite() {
    stopBlackClock();
    startWhiteClock();
}

void ChessClock::switchToBlack() {
    stopWhiteClock();
    startBlackClock();
}

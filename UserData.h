#pragma once
#include <string>
#include "BoardTheme.h"
#include "PieceSetType.h"

class UserData {
public:
    int id = -1;
    std::string username;
    std::string email;
    std::string password;
    bool stayLoggedIn = false;
    int level = 0;
    std::string selectedColor;
    int selectedDifficulty = 1;
    BoardTheme selectedBoardTheme = BoardTheme::Wooden;
    PieceSetType selectedPieceSet = PieceSetType::CLASSIC;

    UserData() = default;

    // Getters
    const std::string& getUsername() const { return username; }
    const std::string& getEmail() const { return email; }
    const std::string& getPassword() const { return password; }
    const std::string& getSelectedColor() const { return selectedColor; }
    int getSelectedDifficulty() const { return selectedDifficulty; }
    int getLevel() const { return level; }
    BoardTheme getSelectedBoardTheme() const { return selectedBoardTheme; }
    PieceSetType getSelectedPieceSet() const { return selectedPieceSet; }

    // Setters
    void setUsername(const std::string& val) { username = val; }
    void setEmail(const std::string& val) { email = val; }
    void setPassword(const std::string& val) { password = val; }
    void setSelectedColor(const std::string& val) { selectedColor = val; }
    void setSelectedDifficulty(int val) { selectedDifficulty = val; }
    void setLevel(int val) { level = val; }
    void setSelectedBoardTheme(BoardTheme val) { selectedBoardTheme = val; }
    void setSelectedPieceSet(PieceSetType val) { selectedPieceSet = val; }

    void reset() {
        id = -1;
        username.clear();
        email.clear();
        password.clear();
        stayLoggedIn = false;
        level = 0;
        selectedColor.clear();
        selectedDifficulty = 1;
        selectedBoardTheme = BoardTheme::Wooden;
        selectedPieceSet = PieceSetType::CLASSIC;
    }
};

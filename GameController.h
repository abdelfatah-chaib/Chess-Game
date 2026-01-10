#pragma once
#include <SFML/Graphics.hpp>
#include "ChessBoard.h"
#include "MoveValidator.h"
#include "GameEndEvaluator.h"
#include "PieceSetType.h"
#include <string>
#include <vector>
#include <utility>
#include <memory>

// Forward declarations
class ScoreSystem;
class AIEngine;

class GameController {
private:
    ChessBoard chessBoard;
    bool boardInitialized;
    bool whiteTurn;
    bool gamePaused;
    
    int selectedPieceRow;
    int selectedPieceCol;
    bool pieceSelected;
    std::vector<std::pair<int, int>> legalMoves; // Stocker les mouvements légals

    std::string selectedColor;
    int selectedDifficulty;
    
    // Score System Integration
    ScoreSystem* scoreSystem;
    std::string player1Name;
    std::string player2Name;
    bool gameEnded;
    
    // Game End Evaluation
    std::unique_ptr<MoveValidator> moveValidator;
    std::unique_ptr<GameEndEvaluator> gameEndEvaluator;
    GameResult currentGameResult;
    GameEndReason currentEndReason;
    
    // Move tracking
    int moveCount;

    // IA System
    std::unique_ptr<AIEngine> aiEngine;
    bool aiEnabled;
    std::string aiColor;  // Couleur de l'IA ("white" ou "black")
    bool aiThinking;      // Flagpour indiquer que l'IA réfléchit
    float aiThinkingTimer; // Timer pour simuler le temps de réflexion

public:
    GameController();
    ~GameController();

    void initializeBoard(float x, float y, float size);
    void initializeBoard(float x, float y, float size, const ThemeColors& theme);
    void initializeBoard(float x, float y, float size, const ThemeColors& theme, PieceSetType pieceSet);  // New overload
    void resetGame();
    void update(float deltaTime);
    
    bool selectPiece(int row, int col);
    bool movePiece(int toRow, int toCol);
    bool undoMove();
    
    void pauseGame();
    void resumeGame();
    
    // Game state checking methods
    bool isCurrentPlayerInCheck() const;
    bool isCurrentPlayerInCheckmate() const;
    bool isStalemate() const;
    
    // AI Management
    void setDifficulty(int difficulty);
    void enableAI(bool enable, const std::string& color = "black");
    void disableAI();
    bool isAITurn() const;
    void startAIThinking();  // Démarre explicitement la réflexion IA
    void processAIMove();
    
    // Getters
    ChessBoard& getBoard() { return chessBoard; }
    bool isWhiteTurn() const { return whiteTurn; }
    bool isPaused() const { return gamePaused; }
    bool isPieceSelected() const { return pieceSelected; }
    int getSelectedRow() const { return selectedPieceRow; }
    int getSelectedCol() const { return selectedPieceCol; }
    const std::vector<std::pair<int, int>>& getLegalMoves() const { return legalMoves; }
    bool isAIEnabled() const { return aiEnabled; }
    bool isAIThinking() const { return aiThinking; }
    const std::string& getAIColor() const { return aiColor; }
    float getAIThinkingTimeRemaining() const { return aiThinkingTimer; }
    
    // Game End Status Getters
    bool isGameEnded() const { return gameEnded; }
    GameResult getGameResult() const { return currentGameResult; }
    GameEndReason getEndReason() const { return currentEndReason; }
    std::string getGameResultDescription() const;
    int getMoveCount() const { return moveCount; }
    
    // Setters
    void setSelectedColor(const std::string& color) { selectedColor = color; }
    void setPlayerNames(const std::string& white, const std::string& black);
    
    // Score System Methods  
    void updateGameScore();
    std::string getCurrentScoreString() const;
    void recordGameResult(const std::string& winner, const std::string& loser);
    std::string getPositionEvaluation();

private:
    void calculateLegalMoves(int row, int col);
    void clearLegalMoves();
    void initializeAI();
    void executeAIMove(const Move& move);
    void evaluateGameEnd();
    void handleGameEnd(const std::string& winnerName, const std::string& loserName);
};

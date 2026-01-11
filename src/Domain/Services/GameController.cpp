#include "Services/GameController.h"
#include "Entities/ChessPiece.h"
#include "Rules/MoveValidator.h"
#include "Services/GameEndEvaluator.h"
#include "Services/ScoreSystem.h"
#include "AIEngine.h"
#include "BoardTheme.h"
#include <iostream>
#include <cstdlib>  // Pour rand()
#include <ctime>    // Pour initialiser le générateur aléatoire

GameController::GameController()
    : boardInitialized(false), whiteTurn(true), gamePaused(false),
      selectedPieceRow(-1), selectedPieceCol(-1), pieceSelected(false),
      selectedDifficulty(1), gameEnded(false), moveCount(0),
      player1Name("White"), player2Name("Black"),
      scoreSystem(new ScoreSystem()),
      currentGameResult(GameResult::ONGOING),
      currentEndReason(GameEndReason::NONE),
      aiEnabled(false), aiColor("black"), aiThinking(false), aiThinkingTimer(0.0f) {
    
    // Initialiser le générateur aléatoire pour les délais IA
    srand(static_cast<unsigned int>(time(nullptr)));
}

GameController::~GameController() {
    delete scoreSystem;
}

void GameController::initializeBoard(float x, float y, float size) {
    if (!boardInitialized) {
        chessBoard.loadTextures();
        boardInitialized = true;
    }
    chessBoard.initialize(x, y, size);
    whiteTurn = true;
    pieceSelected = false;
    selectedPieceRow = -1;
    selectedPieceCol = -1;
    clearLegalMoves();
    gamePaused = false;
    gameEnded = false;
    aiThinking = false;
    aiThinkingTimer = 0.0f;
    moveCount = 0;
    currentGameResult = GameResult::ONGOING;
    currentEndReason = GameEndReason::NONE;
    
    // Initialiser MoveValidator et GameEndEvaluator
    moveValidator = std::make_unique<MoveValidator>(&chessBoard);
    gameEndEvaluator = std::make_unique<GameEndEvaluator>(&chessBoard, moveValidator.get());
    
    // Initialize score
    updateGameScore();
    
    // Initialiser l'IA si nécessaire
    if (aiEnabled) {
        initializeAI();
    }
}

void GameController::initializeBoard(float x, float y, float size, const ThemeColors& theme) {
    if (!boardInitialized) {
        chessBoard.loadTextures();
        boardInitialized = true;
    }
    chessBoard.initialize(x, y, size, theme);
    whiteTurn = true;
    pieceSelected = false;
    selectedPieceRow = -1;
    selectedPieceCol = -1;
    clearLegalMoves();
    gamePaused = false;
    gameEnded = false;
    aiThinking = false;
    aiThinkingTimer = 0.0f;
    moveCount = 0;
    currentGameResult = GameResult::ONGOING;
    currentEndReason = GameEndReason::NONE;
    
    // Initialiser MoveValidator et GameEndEvaluator
    moveValidator = std::make_unique<MoveValidator>(&chessBoard);
    gameEndEvaluator = std::make_unique<GameEndEvaluator>(&chessBoard, moveValidator.get());
    
    // Initialize score
    updateGameScore();
    
    // Initialiser l'IA si nécessaire
    if (aiEnabled) {
        initializeAI();
    }
}

void GameController::initializeBoard(float x, float y, float size, const ThemeColors& theme, PieceSetType pieceSet) {
    if (!boardInitialized) {
        // Load textures with the selected piece set
        BoardTheme currentTheme = BoardThemeManager::getCurrentTheme();
        chessBoard.loadTextures(pieceSet, currentTheme);
        boardInitialized = true;
    }
    chessBoard.initialize(x, y, size, theme, pieceSet);
    whiteTurn = true;
    pieceSelected = false;
    selectedPieceRow = -1;
    selectedPieceCol = -1;
    clearLegalMoves();
    gamePaused = false;
    gameEnded = false;
    aiThinking = false;
    aiThinkingTimer = 0.0f;
    moveCount = 0;
    currentGameResult = GameResult::ONGOING;
    currentEndReason = GameEndReason::NONE;
    
    // Initialiser MoveValidator et GameEndEvaluator
    moveValidator = std::make_unique<MoveValidator>(&chessBoard);
    gameEndEvaluator = std::make_unique<GameEndEvaluator>(&chessBoard, moveValidator.get());
    
    // Initialize score
    updateGameScore();
    
    // Initialiser l'IA si nécessaire
    if (aiEnabled) {
        initializeAI();
    }
}

void GameController::resetGame() {
    // Reset ALL board pieces to initial positions
    chessBoard.reset();  // This will reset pieces to starting positions
    
    // Reset game logic state
    whiteTurn = true;
    pieceSelected = false;
    selectedPieceRow = -1;
    selectedPieceCol = -1;
    gamePaused = false;
    gameEnded = false;
    moveCount = 0;
    clearLegalMoves();
    aiThinking = false;
    aiThinkingTimer = 0.0f;
    currentGameResult = GameResult::ONGOING;
    currentEndReason = GameEndReason::NONE;
    
    // Réinitialiser GameEndEvaluator
    if (gameEndEvaluator) {
        gameEndEvaluator->reset();
    }
    
    // Réinitialiser MoveValidator avec le nouveau plateau
    moveValidator = std::make_unique<MoveValidator>(&chessBoard);
    gameEndEvaluator = std::make_unique<GameEndEvaluator>(&chessBoard, moveValidator.get());
    
    updateGameScore();
    
    std::cout << "[GameController] Game reset complete - all pieces returned to starting positions" << std::endl;
    
    // Relancer l'IA si elle joue les blancs et est enabled
    if (aiEnabled && aiColor == "white") {
        startAIThinking();
    }
}

void GameController::update(float deltaTime) {
    // Update score every frame to reflect current board state
    if (!gameEnded) {
        updateGameScore();
    }
    
    // Mise à jour du système IA - SEULEMENT si l'IA réfléchit déjà
    if (aiThinking && aiEnabled) {
        aiThinkingTimer -= deltaTime;
        if (aiThinkingTimer <= 0.0f) {
            // L'IA a fini de réfléchir
            processAIMove();
            aiThinking = false;
        }
    }
}

void GameController::calculateLegalMoves(int row, int col) {
    legalMoves.clear();
    ChessPiece* piece = chessBoard.getPieceAt(row, col);
    
    if (!piece || piece->type.empty()) {
        return;
    }

    // Calcul silencieux des mouvements légaux
    MoveValidator validator(&chessBoard);
    std::vector<Move> validMoves = validator.getValidMovesForPiece(row, col);
    
    for (const Move& move : validMoves) {
        legalMoves.push_back({move.toRow, move.toCol});
    }
    
    // LOG OPTIMISÉ : Une seule ligne résumé
    std::cout << "[Selection] " << piece->color << " " << piece->type 
              << " at (" << row << "," << col << ") → " << legalMoves.size() << " legal moves" << std::endl;
}

void GameController::clearLegalMoves() {
    legalMoves.clear();
}

bool GameController::selectPiece(int row, int col) {
    // Ne pas permettre la sélection pendant le tour de l'IA
    if (isAITurn() || aiThinking) {
        return false;
    }
    
    if (gameEnded) return false;
    
    ChessPiece* piece = chessBoard.getPieceAt(row, col);
    std::string currentPlayerColor = whiteTurn ? "white" : "black";
    
    if (pieceSelected) {
        // Vérifier si c'est un mouvement légal
        bool isLegalMove = false;
        for (const auto& move : legalMoves) {
            if (move.first == row && move.second == col) {
                isLegalMove = true;
                break;
            }
        }
        
        if (isLegalMove) {
            // Exécuter le mouvement
            Move moveToValidate(selectedPieceRow, selectedPieceCol, row, col);
            const ChessPiece* targetPiece = chessBoard.getPieceAt(row, col);
            if (targetPiece && !targetPiece->type.empty()) {
                moveToValidate.capturedType = targetPiece->type;
                moveToValidate.capturedColor = targetPiece->color;
            }
            
            MoveValidator validator(&chessBoard);
            if (validator.isMoveLegal(moveToValidate) && 
                chessBoard.movePiece(selectedPieceRow, selectedPieceCol, row, col)) {
                
                // LOG OPTIMISÉ : Mouvement réussi
                std::cout << "[Move] " << currentPlayerColor << " moved (" 
                          << selectedPieceRow << "," << selectedPieceCol 
                          << ") → (" << row << "," << col << ")";
                if (!moveToValidate.capturedType.empty()) {
                    std::cout << " [captured " << moveToValidate.capturedType << "]";
                }
                std::cout << std::endl;
                
                whiteTurn = !whiteTurn;
                moveCount++;  // Increment move count
                
                // Évaluer l'état de fin de partie
                evaluateGameEnd();

                pieceSelected = false;
                selectedPieceRow = -1;
                selectedPieceCol = -1;
                clearLegalMoves();
                
                // Update score after move
                updateGameScore();
                
                // Démarrer l'IA si c'est son tour
                if (aiEnabled && isAITurn() && !gamePaused && !gameEnded) {
                    startAIThinking();
                }
                
                return true;
            }
        }
        
        // Sélectionner nouvelle pièce ou désélectionner
        if (piece && !piece->type.empty() && 
            ((whiteTurn && piece->color == "white") || (!whiteTurn && piece->color == "black"))) {
            pieceSelected = true;
            selectedPieceRow = row;
            selectedPieceCol = col;
            calculateLegalMoves(row, col);
            return true;
        } else {
            pieceSelected = false;
            selectedPieceRow = -1;
            selectedPieceCol = -1;
            clearLegalMoves();
            return false;
        }
    }
    
    // Première sélection
    if (piece && !piece->type.empty()) {
        if ((whiteTurn && piece->color == "white") || (!whiteTurn && piece->color == "black")) {
            pieceSelected = true;
            selectedPieceRow = row;
            selectedPieceCol = col;
            calculateLegalMoves(row, col);
            return true;
        }
    }
    
    return false;
}

bool GameController::movePiece(int toRow, int toCol) {
    if (!pieceSelected || gameEnded) return false;
    
    Move moveToValidate(selectedPieceRow, selectedPieceCol, toRow, toCol);
    const ChessPiece* targetPiece = chessBoard.getPieceAt(toRow, toCol);
    if (targetPiece && !targetPiece->type.empty()) {
        moveToValidate.capturedType = targetPiece->type;
        moveToValidate.capturedColor = targetPiece->color;
    }
    
    MoveValidator validator(&chessBoard);
    if (!validator.isMoveLegal(moveToValidate)) {
        return false;
    }
    
    if (chessBoard.movePiece(selectedPieceRow, selectedPieceCol, toRow, toCol)) {
        whiteTurn = !whiteTurn;
        moveCount++;  // Increment move count
        pieceSelected = false;
        selectedPieceRow = -1;
        selectedPieceCol = -1;
        clearLegalMoves();
        
        // Update score after successful move
        updateGameScore();
        
        // Démarrer l'IA si c'est son tour
        if (aiEnabled && isAITurn() && !gamePaused) {
            startAIThinking();
        }
        
        return true;
    }
    return false;
}

bool GameController::undoMove() {
    if (chessBoard.undoMove()) {
        whiteTurn = !whiteTurn;
        pieceSelected = false;
        selectedPieceRow = -1;
        selectedPieceCol = -1;
        clearLegalMoves();
        gameEnded = false;
        aiThinking = false;
        aiThinkingTimer = 0.0f;
        
        // Update score after undo
        updateGameScore();
        
        std::cout << "[Action] Move undone" << std::endl;
        return true;
    }
    return false;
}

bool GameController::isCurrentPlayerInCheck() const {
    MoveValidator validator(&chessBoard);
    std::string currentColor = whiteTurn ? "white" : "black";
    return validator.isInCheck(currentColor);
}

bool GameController::isCurrentPlayerInCheckmate() const {
    MoveValidator validator(&chessBoard);
    std::string currentColor = whiteTurn ? "white" : "black";
    return validator.isCheckmate(currentColor);
}

bool GameController::isStalemate() const {
    MoveValidator validator(&chessBoard);
    std::string currentColor = whiteTurn ? "white" : "black";
    return validator.isStalemate(currentColor);
}

void GameController::pauseGame() {
    gamePaused = true;
    aiThinking = false;
}

void GameController::resumeGame() {
    gamePaused = false;
}

// ================================
// AI Management Methods
// ================================

void GameController::setDifficulty(int difficulty) {
    std::cout << "[AI] Difficulty set to level " << difficulty << std::endl;
    selectedDifficulty = difficulty;
    
    if (aiEnabled) {
        initializeAI();
    }
}

void GameController::enableAI(bool enable, const std::string& color) {
    std::cout << "[AI] " << (enable ? "Enabled" : "Disabled") << " - plays as " << color << std::endl;
    aiEnabled = enable;
    aiColor = color;
    aiThinking = false;
    aiThinkingTimer = 0.0f;
    
    if (enable) {
        initializeAI();
        if (isAITurn() && !gamePaused) {
            startAIThinking();
        }
    }
}

void GameController::disableAI() {
    enableAI(false);
}

bool GameController::isAITurn() const {
    if (!aiEnabled) return false;
    std::string currentColor = whiteTurn ? "white" : "black";
    return (currentColor == aiColor);
}

void GameController::startAIThinking() {
    if (!aiEnabled || aiThinking) return;
    
    // Délais variables selon le niveau de difficulté
    float thinkingTime = 0.5f;  // Délai par défaut
    
    switch (selectedDifficulty) {
        case 1: // Niveau facile - 4-7 secondes comme demandé
            thinkingTime = 4.0f + (rand() % 4);  // 4-7 secondes aléatoires
            std::cout << "[AI] " << aiColor << " (Beginner) thinking carefully for " << thinkingTime << " seconds..." << std::endl;
            break;
        case 2: // Niveau moyen - 2-4 secondes
            thinkingTime = 2.0f + (rand() % 3);  // 2-4 secondes aléatoires
            std::cout << "[AI] " << aiColor << " (Intermediate) analyzing position for " << thinkingTime << " seconds..." << std::endl;
            break;
        case 3: // Niveau difficile - 1-3 secondes (plus rapide car plus intelligent)
            thinkingTime = 1.0f + (rand() % 3);  // 1-3 secondes aléatoires
            std::cout << "[AI] " << aiColor << " (Expert) calculating best move for " << thinkingTime << " seconds..." << std::endl;
            break;
        default:
            thinkingTime = 2.0f;
            break;
    }
    
    aiThinking = true;
    aiThinkingTimer = thinkingTime;
}

void GameController::processAIMove() {
    if (!aiEngine) {
        std::cout << "[AI] ERROR: Engine not initialized!" << std::endl;
        return;
    }
    
    std::string currentColor = whiteTurn ? "white" : "black";
    
    try {
        Move aiMove = aiEngine->chooseMove(chessBoard, currentColor);
        
        if (aiMove.fromRow >= 0 && aiMove.fromCol >= 0 && aiMove.toRow >= 0 && aiMove.toCol >= 0) {
            executeAIMove(aiMove);
        } else {
            std::cout << "[AI] No valid move found" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "[AI] Error: " << e.what() << std::endl;
    }
}

void GameController::initializeAI() {
    std::cout << "[AI] Initializing engine (difficulty " << selectedDifficulty << ")" << std::endl;
    aiEngine = createAIEngine(selectedDifficulty);
    
    if (!aiEngine) {
        std::cout << "[AI] ERROR: Failed to create engine!" << std::endl;
        aiEnabled = false;
    }
}

void GameController::executeAIMove(const Move& move) {
    MoveValidator validator(&chessBoard);
    if (!validator.isMoveLegal(move)) {
        std::cout << "[AI] ERROR: Illegal move suggested!" << std::endl;
        return;
    }
    
    if (chessBoard.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol)) {
        std::cout << "[AI] Move executed successfully" << std::endl;
        
        whiteTurn = !whiteTurn;
        moveCount++;  // Increment move count
        
        // Évaluer l'état de fin de partie
        evaluateGameEnd();
        
        pieceSelected = false;
        selectedPieceRow = -1;
        selectedPieceCol = -1;
        clearLegalMoves();
        
        // Update score after AI move
        updateGameScore();
        
    } else {
        std::cout << "[AI] ERROR: Move execution failed" << std::endl;
    }
}

// ================================
// Score System Implementation
// ================================

void GameController::setPlayerNames(const std::string& white, const std::string& black) {
    player1Name = white;
    player2Name = black;
}

void GameController::updateGameScore() {
    if (!scoreSystem) return;
    // Score is calculated on demand, no need to store
}

std::string GameController::getCurrentScoreString() const {
    if (!scoreSystem) return "Score: 0-0";
    
    ScoreSystem::GameScore currentScore = scoreSystem->calculateCurrentScore(chessBoard);
    return "White: " + std::to_string(currentScore.whiteScore) + 
           ", Black: " + std::to_string(currentScore.blackScore) +
           ", Diff: " + std::to_string(currentScore.getDifference());
}

void GameController::recordGameResult(const std::string& winner, const std::string& loser) {
    if (!scoreSystem || winner.empty() || loser.empty()) return;
    
    ScoreSystem::GameScore currentScore = scoreSystem->calculateCurrentScore(chessBoard);
    int winnerBonus = currentScore.getDifference() > 0 ? 
                     (winner == player1Name ? currentScore.whiteScore : currentScore.blackScore) : 0;
    int loserBonus = std::max(0, winnerBonus / 4); // Loser gets 25% of winner's score as consolation
    
    scoreSystem->addGameResult(winner, true, winnerBonus, loser, true);
    scoreSystem->addGameResult(loser, false, loserBonus, winner, true);
    
    std::cout << "[Score] Game recorded - Winner: " << winner 
              << " (+" << winnerBonus << "), Loser: " << loser 
              << " (+" << loserBonus << ")" << std::endl;
}

std::string GameController::getPositionEvaluation() {
    if (!scoreSystem) return "Evaluation unavailable";
    return scoreSystem->evaluatePosition(chessBoard);
}

void GameController::evaluateGameEnd() {
    if (!gameEndEvaluator) return;
    
    std::string currentPlayerColor = whiteTurn ? "white" : "black";
    currentGameResult = gameEndEvaluator->evaluateGameState(currentPlayerColor);
    currentEndReason = gameEndEvaluator->getEndReason();
    
    if (currentGameResult != GameResult::ONGOING) {
        gameEnded = true;
        std::cout << "[GAME] " << gameEndEvaluator->getResultDescription() << std::endl;
        
        // Enregistrer le résultat dans le système de score
        if (currentGameResult == GameResult::WHITE_WIN) {
            handleGameEnd(player1Name, player2Name);
        } else if (currentGameResult == GameResult::BLACK_WIN) {
            handleGameEnd(player2Name, player1Name);
        } else if (currentGameResult == GameResult::DRAW) {
            // En cas de match nul, on peut enregistrer comme draw pour les deux
            std::cout << "[Score] Draw recorded for both players" << std::endl;
        }
    }
}

void GameController::handleGameEnd(const std::string& winnerName, const std::string& loserName) {
    recordGameResult(winnerName, loserName);
}

std::string GameController::getGameResultDescription() const {
    if (gameEndEvaluator) {
        return gameEndEvaluator->getResultDescription();
    }
    return "Game in progress";
}

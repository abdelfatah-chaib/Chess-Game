#include "Services/GameController.h"
#include "Entities/ChessPiece.h"
#include "Rules/MoveValidator.h"
#include "Services/GameEndEvaluator.h"
#include "Services/ScoreSystem.h"
#include "Services/ChessClock.h"  // Add chess clock include
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
      aiEnabled(false), aiColor("black"), aiThinking(false), aiThinkingTimer(0.0f),
      currentGameState(GameState::PLAYING),
      pendingPromotionRow(-1), pendingPromotionCol(-1), pendingPromotionColor("") {
    
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
    
    // Check for king danger at start (should be none in standard position)
    updateKingDangerStatus();
    
    // Initialiser l'IA si nécessaire
    if (aiEnabled) {
        initializeAI();
    }

    // Initialize chess clock and start white's clock
    chessClock.reset();
    chessClock.startWhiteClock();
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
    
    // Check for king danger at start (should be none in standard position)
    updateKingDangerStatus();
    
    // Initialiser l'IA si nécessaire
    if (aiEnabled) {
        initializeAI();
    }

    // Initialize chess clock and start white's clock
    chessClock.reset();
    chessClock.startWhiteClock();
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
    
    // Check for king danger at start (should be none in standard position)
    updateKingDangerStatus();
    
    // Initialize chess clock and start white's clock
    chessClock.reset();
    chessClock.startWhiteClock();
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
    
    // Reset game state
    currentGameState = GameState::PLAYING;
    pendingPromotionRow = -1;
    pendingPromotionCol = -1;
    pendingPromotionColor = "";
    
    // Reset GameEndEvaluator
    if (gameEndEvaluator) {
        gameEndEvaluator->reset();
    }
    
    // Reset MoveValidator with new board
    moveValidator = std::make_unique<MoveValidator>(&chessBoard);
    gameEndEvaluator = std::make_unique<GameEndEvaluator>(&chessBoard, moveValidator.get());
    
    // Clear king danger status on reset
    chessBoard.clearKingDangerStatus();
    
    // Reset chess clock and start white's clock
    chessClock.reset();
    chessClock.startWhiteClock();
    
    updateGameScore();
    
    std::cout << "[GameController] Game reset complete - all pieces returned to starting positions" << std::endl;
    
    // Restart AI if it plays white
    if (aiEnabled && aiColor == "white") {
        startAIThinking();
    }
}

void GameController::update(float deltaTime) {
    // Update chess clock
    if (!gameEnded && currentGameState != GameState::PAWN_PROMOTION_PENDING) {
        chessClock.update(deltaTime);
        
        // Check for timeout
        if (chessClock.isWhiteTimeOut()) {
            std::cout << "[GameController] WHITE TIME OUT - Black wins!" << std::endl;
            currentGameResult = GameResult::BLACK_WIN;
            currentEndReason = GameEndReason::TIMEOUT;
            gameEnded = true;
            chessClock.pauseAll();
            handleGameEnd(player2Name, player1Name);
            return;
        }
        
        if (chessClock.isBlackTimeOut()) {
            std::cout << "[GameController] BLACK TIME OUT - White wins!" << std::endl;
            currentGameResult = GameResult::WHITE_WIN;
            currentEndReason = GameEndReason::TIMEOUT;
            gameEnded = true;
            chessClock.pauseAll();
            handleGameEnd(player1Name, player2Name);
            return;
        }
    }
    
    // Update AI system - only if AI is thinking
    if (aiThinking && aiEnabled) {
        aiThinkingTimer -= deltaTime;
        if (aiThinkingTimer <= 0.0f) {
            // AI finished thinking
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
    // Ne pas permettre la sélection si promotion en attente
    if (currentGameState == GameState::PAWN_PROMOTION_PENDING) {
        std::cout << "[GameController] Cannot select piece - pawn promotion pending" << std::endl;
        return false;
    }
    
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
            // Vérifier si c'est une promotion de pion
            if (isPawnPromotionRequired(selectedPieceRow, selectedPieceCol, row, col)) {
                // C'est une promotion - bloquer le jeu et attendre la sélection du joueur
                std::cout << "[GameController] Pawn promotion required at (" << row << "," << col << ")" << std::endl;
                
                ChessPiece* pawn = chessBoard.getPieceAt(selectedPieceRow, selectedPieceCol);
                
                // Exécuter le mouvement du pion (sans promotion)
                Move moveToValidate(selectedPieceRow, selectedPieceCol, row, col);
                const ChessPiece* targetPiece = chessBoard.getPieceAt(row, col);
                if (targetPiece && !targetPiece->type.empty()) {
                    moveToValidate.capturedType = targetPiece->type;
                    moveToValidate.capturedColor = targetPiece->color;
                }
                
                MoveValidator validator(&chessBoard);
                if (validator.isMoveLegal(moveToValidate)) {
                    // Exécuter le mouvement SANS promotion automatique
                    // On utilise un flag temporaire pour empêcher la promotion dans movePiece
                    if (chessBoard.movePiece(selectedPieceRow, selectedPieceCol, row, col, "")) {
                        // Entrer dans l'état de promotion en attente
                        currentGameState = GameState::PAWN_PROMOTION_PENDING;
                        pendingPromotionRow = row;
                        pendingPromotionCol = col;
                        pendingPromotionColor = pawn->color;
                        
                        // PAUSE CHESS CLOCK during promotion selection
                        chessClock.pauseAll();
                        
                        // NE PAS changer de tour - on attend la sélection
                        pieceSelected = false;
                        selectedPieceRow = -1;
                        selectedPieceCol = -1;
                        clearLegalMoves();
                        
                        std::cout << "[GameController] Entering PAWN_PROMOTION_PENDING state - clock paused" << std::endl;
                        return true;
                    }
                }
                return false;
            }
            
            // Mouvement normal (pas de promotion)
            Move moveToValidate(selectedPieceRow, selectedPieceCol, row, col);
            const ChessPiece* targetPiece = chessBoard.getPieceAt(row, col);
            if (targetPiece && !targetPiece->type.empty()) {
                moveToValidate.capturedType = targetPiece->type;
                moveToValidate.capturedColor = targetPiece->color;
            }
            
            MoveValidator validator(&chessBoard);
            if (validator.isMoveLegal(moveToValidate) && 
                chessBoard.movePiece(selectedPieceRow, selectedPieceCol, row, col)) {
                
                // LOG: Successful move
                std::cout << "[Move] " << currentPlayerColor << " moved (" 
                          << selectedPieceRow << "," << selectedPieceCol 
                          << ") → (" << row << "," << col << ")"
                          << " [captured: " << moveToValidate.capturedType << "]"
                          << std::endl;
                
                whiteTurn = !whiteTurn;
                moveCount++;
                
                // Switch chess clock to next player
                if (whiteTurn) {
                    chessClock.switchToWhite();
                } else {
                    chessClock.switchToBlack();
                }
                
                // Update king danger status for BOTH kings after each move
                updateKingDangerStatus();
                
                // Evaluate game end
                evaluateGameEnd();

                pieceSelected = false;
                selectedPieceRow = -1;
                selectedPieceCol = -1;
                clearLegalMoves();
                
                // Update score after move
                updateGameScore();
                
                // Start AI if it's AI's turn
                if (aiEnabled && isAITurn() && !gamePaused && !gameEnded) {
                    startAIThinking();
                }
                
                return true;
            } else {
                std::cout << "[Move] " << currentPlayerColor << " attempted illegal move" << std::endl;
            }
        } else {
            std::cout << "[Selection] Illegal move targeted: (" << row << "," << col << ")" << std::endl;
        }
    } else {
        // Sélectionner une pièce
        if (piece && piece->color == currentPlayerColor) {
            selectedPieceRow = row;
            selectedPieceCol = col;
            pieceSelected = true;
            
            // Calculer les mouvements légaux pour la pièce sélectionnée
            calculateLegalMoves(row, col);
            
            std::cout << "[Selection] " << currentPlayerColor << " selected " << piece->type 
                      << " at (" << row << "," << col << ")" << std::endl;
        } else {
            std::cout << "[Selection] Invalid piece selection" << std::endl;
        }
    }
    
    return false;
}

bool GameController::isPawnPromotionRequired(int fromRow, int fromCol, int toRow, int toCol) const {
    const ChessPiece* pawn = chessBoard.getPieceAt(fromRow, fromCol);
    if (pawn && pawn->type == "pawn") {
        if (pawn->color == "white" && toRow == 0) {
            return true;
        } else if (pawn->color == "black" && toRow == 7) {
            return true;
        }
    }
    return false;
}

void GameController::executePawnPromotion(const std::string& promotionPiece) {
    if (currentGameState != GameState::PAWN_PROMOTION_PENDING) {
        std::cout << "[GameController] ERROR: executePawnPromotion called but no promotion pending" << std::endl;
        return;
    }
    
    std::cout << "[GameController] Executing pawn promotion to " << promotionPiece 
              << " at (" << pendingPromotionRow << "," << pendingPromotionCol << ")" << std::endl;
    
    // Promote the pawn
    chessBoard.promotePawn(pendingPromotionRow, pendingPromotionCol, promotionPiece);
    
    // Switch turn NOW (after promotion is complete)
    whiteTurn = !whiteTurn;
    moveCount++;
    
    // Resume chess clock and switch to next player
    chessClock.resumeAll();
    if (whiteTurn) {
        chessClock.switchToWhite();
    } else {
        chessClock.switchToBlack();
    }
    
    // Update score after promotion
    updateGameScore();
    
    // Update king danger status after promotion
    updateKingDangerStatus();
    
    // Evaluate game end
    evaluateGameEnd();
    
    // Clear promotion state
    currentGameState = GameState::PLAYING;
    pendingPromotionRow = -1;
    pendingPromotionCol = -1;
    pendingPromotionColor = "";
    
    std::cout << "[GameController] Promotion complete - game resumed, clock switched" << std::endl;
    
    // Start AI thinking if it's AI's turn
    if (aiEnabled && isAITurn() && !gamePaused && !gameEnded) {
        startAIThinking();
    }
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
        
        // Switch chess clock back to current player after undo
        if (whiteTurn) {
            chessClock.switchToWhite();
        } else {
            chessClock.switchToBlack();
        }
        
        // Update king danger status after undo
        updateKingDangerStatus();
        
        // Update score after undo
        updateGameScore();
        
        std::cout << "[Action] Move undone - clock switched to " 
                  << (whiteTurn ? "white" : "black") << std::endl;
        return true;
    }
    return false;
}

void GameController::pauseGame() {
    gamePaused = true;
    aiThinking = false;
    chessClock.pauseAll();
    std::cout << "[GameController] Game paused - clocks stopped" << std::endl;
}

void GameController::resumeGame() {
    gamePaused = false;
    chessClock.resumeAll();
    
    // Restart the clock for current player
    if (whiteTurn) {
        chessClock.startWhiteClock();
    } else {
        chessClock.startBlackClock();
    }
    
    std::cout << "[GameController] Game resumed - clock restarted for " 
              << (whiteTurn ? "white" : "black") << std::endl;
}

void GameController::updateGameScore() {
    if (!scoreSystem) return;
    
    // Use the new ScoreSystem calculateCurrentScore method
    ScoreSystem::GameScore score = scoreSystem->calculateCurrentScore(chessBoard);
}

std::string GameController::getCurrentScoreString() const {
    if (!scoreSystem) return "White: 0, Black: 0, Diff: 0";
    
    ScoreSystem::GameScore score = scoreSystem->calculateCurrentScore(chessBoard);
    return score.toString();
}

void GameController::setPlayerNames(const std::string& white, const std::string& black) {
    player1Name = white;
    player2Name = black;
    std::cout << "[GameController] Player names set: " << white << " vs " << black << std::endl;
}

std::string GameController::getGameResultDescription() const {
    if (!gameEndEvaluator) {
        return "Game in progress";
    }
    return gameEndEvaluator->getResultDescription();
}

void GameController::updateKingDangerStatus() {
    if (!moveValidator) return;
    
    // Update danger status for both kings
    chessBoard.updateKingDangerStatus("white", moveValidator.get());
    chessBoard.updateKingDangerStatus("black", moveValidator.get());
}

void GameController::evaluateGameEnd() {
    if (gameEnded || !gameEndEvaluator) return;
    
    std::string currentColor = whiteTurn ? "white" : "black";
    GameResult result = gameEndEvaluator->evaluateGameState(currentColor);
    
    if (result != GameResult::ONGOING) {
        currentGameResult = result;
        currentEndReason = gameEndEvaluator->getEndReason();
        gameEnded = true;
        chessClock.pauseAll();
        
        std::cout << "[GameController] Game ended: " << gameEndEvaluator->getResultDescription() << std::endl;
        
        // Determine winner and loser for score tracking
        if (result == GameResult::WHITE_WIN) {
            handleGameEnd(player1Name, player2Name);
        } else if (result == GameResult::BLACK_WIN) {
            handleGameEnd(player2Name, player1Name);
        } else {
            handleGameEnd("", ""); // Draw
        }
    }
}

void GameController::handleGameEnd(const std::string& winnerName, const std::string& loserName) {
    std::cout << "[GameController] Handling game end - Winner: " << winnerName << ", Loser: " << loserName << std::endl;
    
    // Record game result if scoreSystem exists
    if (scoreSystem && !winnerName.empty()) {
        recordGameResult(winnerName, loserName);
    }
}

void GameController::recordGameResult(const std::string& winner, const std::string& loser) {
    // This method is called by handleGameEnd to record the result
    std::cout << "[GameController] Recording game result: " << winner << " defeated " << loser << std::endl;
}

std::string GameController::getPositionEvaluation() {
    if (!scoreSystem) return "";
    return scoreSystem->evaluatePosition(chessBoard);
}

bool GameController::isAITurn() const {
    if (!aiEnabled) return false;
    
    std::string currentColor = whiteTurn ? "white" : "black";
    return (currentColor == aiColor);
}

void GameController::startAIThinking() {
    if (!aiEnabled || !isAITurn()) {
        return;
    }
    
    aiThinking = true;
    aiThinkingTimer = static_cast<float>(rand() % 3 + 2); // 2-5 seconds
    
    std::cout << "[AI] Starting to think for " << aiThinkingTimer << " seconds..." << std::endl;
}

void GameController::processAIMove() {
    if (!aiEngine) {
        std::cout << "[AI] ERROR: AI engine not initialized" << std::endl;
        return;
    }
    
    std::cout << "[AI] Processing move..." << std::endl;
    
    // Get AI move from engine using chooseMove
    Move aiMove = aiEngine->chooseMove(chessBoard, aiColor);
    
    if (aiMove.fromRow == -1) {
        std::cout << "[AI] No valid moves available!" << std::endl;
        return;
    }
    
    // Execute the AI move
    executeAIMove(aiMove);
}

void GameController::executeAIMove(const Move& move) {
    MoveValidator validator(&chessBoard);
    if (!validator.isMoveLegal(move)) {
        std::cout << "[AI] ERROR: Illegal move suggested!" << std::endl;
        return;
    }
    
    if (chessBoard.movePiece(move.fromRow, move.fromCol, move.toRow, move.toCol)) {
        std::cout << "[AI] Move executed successfully: (" << move.fromRow << "," << move.fromCol 
                  << ") -> (" << move.toRow << "," << move.toCol << ")" << std::endl;
        
        whiteTurn = !whiteTurn;
        moveCount++;
        
        // Switch chess clock to next player
        if (whiteTurn) {
            chessClock.switchToWhite();
        } else {
            chessClock.switchToBlack();
        }
        
        // Update king danger status for BOTH kings after AI move
        updateKingDangerStatus();
        
        // Evaluate game end
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

void GameController::setDifficulty(int difficulty) {
    selectedDifficulty = difficulty;
    std::cout << "[GameController] AI difficulty set to " << difficulty << std::endl;
}

void GameController::enableAI(bool enable, const std::string& color) {
    aiEnabled = enable;
    aiColor = color;
    
    if (enable) {
        std::cout << "[GameController] AI enabled for " << color << std::endl;
        initializeAI();
    } else {
        std::cout << "[GameController] AI disabled" << std::endl;
        aiEngine.reset();
    }
}

void GameController::disableAI() {
    aiEnabled = false;
    aiEngine.reset();
    std::cout << "[GameController] AI disabled" << std::endl;
}

void GameController::initializeAI() {
    if (!aiEnabled) return;
    
    // Use the factory function to create AI engine based on difficulty
    aiEngine = createAIEngine(selectedDifficulty);
    std::cout << "[GameController] AI initialized with difficulty " << selectedDifficulty << std::endl;
    
    // If AI plays white, start thinking immediately
    if (aiColor == "white" && whiteTurn) {
        startAIThinking();
    }
}

bool GameController::isCurrentPlayerInCheck() const {
    if (!moveValidator) return false;
    
    std::string currentColor = whiteTurn ? "white" : "black";
    return moveValidator->isInCheck(currentColor);
}

bool GameController::isCurrentPlayerInCheckmate() const {
    if (!moveValidator) return false;
    
    std::string currentColor = whiteTurn ? "white" : "black";
    return moveValidator->isCheckmate(currentColor);
}

bool GameController::isStalemate() const {
    if (!moveValidator) return false;
    
    std::string currentColor = whiteTurn ? "white" : "black";
    return moveValidator->isStalemate(currentColor);
}


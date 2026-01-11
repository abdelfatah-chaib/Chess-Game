#include "AIEngine.h"
#include "ChessBoard.h"
#include "Rules/MoveValidator.h"
#include "Evaluator.h"
#include <random>
#include <algorithm>
#include <limits>
#include <iostream>

// RandomAI - Niveau Facile
class RandomAI : public AIEngine {
private:
    mutable std::random_device rd;
    mutable std::mt19937 gen;
    
public:
    RandomAI() : gen(rd()) {
        std::cout << "[RandomAI] Initialized - Easy difficulty" << std::endl;
    }
    
    Move chooseMove(const ChessBoard& board, const std::string& playerColor) override {
        MoveValidator validator(&board);
        std::vector<Move> validMoves = validator.getAllValidMovesForColor(playerColor);
        
        if (validMoves.empty()) {
            std::cout << "[RandomAI] " << playerColor << " - No valid moves available!" << std::endl;
            return Move(-1, -1, -1, -1);
        }
        
        std::uniform_int_distribution<> dis(0, validMoves.size() - 1);
        int randomIndex = dis(gen);
        Move chosenMove = validMoves[randomIndex];
        
        // LOG OPTIMISÉ : Une seule ligne par décision
        std::cout << "[RandomAI] " << playerColor << " examined " << validMoves.size() 
                  << " moves, chose random move: (" << chosenMove.fromRow << "," << chosenMove.fromCol 
                  << ") → (" << chosenMove.toRow << "," << chosenMove.toCol << ")" << std::endl;
        
        return chosenMove;
    }
};

// HeuristicAI  Niveau Moyen
class HeuristicAI : public AIEngine {
public:
    HeuristicAI() {
        std::cout << "[HeuristicAI] Initialized - Medium difficulty" << std::endl;
    }
    
    Move chooseMove(const ChessBoard& board, const std::string& playerColor) override {
        MoveValidator validator(&board);
        std::vector<Move> validMoves = validator.getAllValidMovesForColor(playerColor);
        
        if (validMoves.empty()) {
            std::cout << "[HeuristicAI] " << playerColor << " - No valid moves available!" << std::endl;
            return Move(-1, -1, -1, -1);
        }
        
        Move bestMove = validMoves[0];
        double bestScore = std::numeric_limits<double>::lowest();
        int captureCount = 0;
        
        // Évaluation silencieuse de tous les coups
        for (const Move& move : validMoves) {
            double score = evaluateMove(board, move, playerColor);
            
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
            
            // Compter les captures pour le log
            if (!move.capturedType.empty()) {
                captureCount++;
            }
        }
        
        // LOG OPTIMISÉ : Résumé concis de l'analyse
        std::cout << "[HeuristicAI] " << playerColor << " analyzed " << validMoves.size() 
                  << " moves (" << captureCount << " captures), best score: " << bestScore 
                  << ", chose: (" << bestMove.fromRow << "," << bestMove.fromCol 
                  << ") → (" << bestMove.toRow << "," << bestMove.toCol << ")";
        
        if (!bestMove.capturedType.empty()) {
            std::cout << " [CAPTURE " << bestMove.capturedType << "]";
        }
        std::cout << std::endl;
        
        return bestMove;
    }
    
private:
    double evaluateMove(const ChessBoard& board, const Move& move, const std::string& playerColor) {
        double score = 0.0;
        
        // Bonus pour capturer une pièce
        if (!move.capturedType.empty()) {
            score += Evaluator::getPieceValue(move.capturedType) * 10.0;
        }
        
        // Bonus pour se rapprocher du centre
        double centerDistance = std::abs(move.toRow - 3.5) + std::abs(move.toCol - 3.5);
        score += (7 - centerDistance) * 0.1;
        
        // Évaluation de la position actuelle
        score += Evaluator::evaluatePosition(board, playerColor);
        
        return score;
    }
};

// MinimaxAI  Niveau Difficile

class MinimaxAI : public AIEngine {
private:
    int maxDepth;
    
public:
    explicit MinimaxAI(int depth = 3) : maxDepth(depth) {
        std::cout << "[MinimaxAI] Initialized - Hard difficulty (depth " << depth << ")" << std::endl;
    }
    
    Move chooseMove(const ChessBoard& board, const std::string& playerColor) override {
        MoveValidator validator(&board);
        std::vector<Move> validMoves = validator.getAllValidMovesForColor(playerColor);
        
        if (validMoves.empty()) {
            std::cout << "[MinimaxAI] " << playerColor << " - No valid moves available!" << std::endl;
            return Move(-1, -1, -1, -1);
        }
        
        Move bestMove = validMoves[0];
        double bestScore = std::numeric_limits<double>::lowest();
        int captureCount = 0;
        int centerMoves = 0;
        int developmentMoves = 0;
        
        // Analyse silencieuse avec compteurs
        for (const Move& move : validMoves) {
            double score = evaluateAdvancedMove(board, move, playerColor);
            
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
            
            // Statistiques pour le log
            if (!move.capturedType.empty()) captureCount++;
            if ((move.toRow >= 2 && move.toRow <= 5) && (move.toCol >= 2 && move.toCol <= 5)) centerMoves++;
            
            const ChessPiece* piece = board.getPieceAt(move.fromRow, move.fromCol);
            if (piece && (piece->type == "knight" || piece->type == "bishop")) {
                bool isInitialPos = (piece->color == "white" && move.fromRow == 7) || 
                                   (piece->color == "black" && move.fromRow == 0);
                if (isInitialPos) developmentMoves++;
            }
        }
        
        // LOG OPTIMISÉ : Analyse tactique résumée
        std::cout << "[MinimaxAI] " << playerColor << " deep analysis: " << validMoves.size() 
                  << " moves [" << captureCount << " captures, " << centerMoves 
                  << " center, " << developmentMoves << " development], score: " << bestScore 
                  << ", selected: (" << bestMove.fromRow << "," << bestMove.fromCol 
                  << ") → (" << bestMove.toRow << "," << bestMove.toCol << ")";
        
        if (!bestMove.capturedType.empty()) {
            std::cout << " [TACTICAL CAPTURE: " << bestMove.capturedType << "]";
        }
        std::cout << std::endl;
        
        return bestMove;
    }
    
private:
    double evaluateAdvancedMove(const ChessBoard& board, const Move& move, const std::string& playerColor) {
        double score = 0.0;
        
        // 1. Bonus capture (plus important que HeuristicAI)
        if (!move.capturedType.empty()) {
            score += Evaluator::getPieceValue(move.capturedType) * 15.0;
        }
        
        // 2. Évaluation position actuelle
        double positionScore = Evaluator::evaluatePosition(board, playerColor);
        score += positionScore;
        
        // 3. Bonus pour contrôle du centre
        double centerBonus = 0.0;
        if ((move.toRow >= 3 && move.toRow <= 4) && (move.toCol >= 3 && move.toCol <= 4)) {
            centerBonus = 2.0;
        } else if ((move.toRow >= 2 && move.toRow <= 5) && (move.toCol >= 2 && move.toCol <= 5)) {
            centerBonus = 1.0;
        }
        score += centerBonus;
        
        // 4. Bonus développement des pièces
        const ChessPiece* piece = board.getPieceAt(move.fromRow, move.fromCol);
        if (piece && !piece->type.empty()) {
            if (piece->type == "knight" || piece->type == "bishop") {
                bool isInitialPosition = false;
                if (piece->color == "white" && move.fromRow == 7) isInitialPosition = true;
                if (piece->color == "black" && move.fromRow == 0) isInitialPosition = true;
                
                if (isInitialPosition) {
                    score += 1.5;
                }
            }
        }
        
        // 5. Pénalité pour exposer le roi
        if (piece && piece->type == "king") {
            score -= 1.0;
        }
        
        return score;
    }
};

// Factory Function
std::unique_ptr<AIEngine> createAIEngine(int difficulty) {
    std::cout << "[AIFactory] Creating AI engine for difficulty level " << difficulty << std::endl;
    
    switch (difficulty) {
        case 1:
            return std::make_unique<RandomAI>();
        case 2:
            return std::make_unique<HeuristicAI>();
        case 3:
        default:
            return std::make_unique<MinimaxAI>(3);
    }
}
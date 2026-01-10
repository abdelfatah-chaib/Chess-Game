#include "GameEndEvaluator.h"
#include "ChessBoard.h"
#include "MoveValidator.h"
#include "ChessPiece.h"
#include <sstream>
#include <iostream>

GameEndEvaluator::GameEndEvaluator(const ChessBoard* board, const MoveValidator* validator)
    : m_board(board)
    , m_validator(validator)
    , m_currentResult(GameResult::ONGOING)
    , m_endReason(GameEndReason::NONE)
    , m_halfMoveClock(0) {
}

GameResult GameEndEvaluator::evaluateGameState(const std::string& currentPlayerColor) {
    // Vérifier d'abord l'échec et mat (priorité la plus haute)
    if (isCheckmate(currentPlayerColor)) {
        m_currentResult = (currentPlayerColor == "white") ? GameResult::BLACK_WIN : GameResult::WHITE_WIN;
        m_endReason = GameEndReason::CHECKMATE;
        return m_currentResult;
    }
    
    // Vérifier le pat
    if (isStalemate(currentPlayerColor)) {
        m_currentResult = GameResult::DRAW;
        m_endReason = GameEndReason::STALEMATE;
        return m_currentResult;
    }
    
    // Vérifier la règle des 50 coups
    if (isFiftyMoveRule()) {
        m_currentResult = GameResult::DRAW;
        m_endReason = GameEndReason::FIFTY_MOVE_RULE;
        return m_currentResult;
    }
    
    // Vérifier la triple répétition
    if (isThreefoldRepetition()) {
        m_currentResult = GameResult::DRAW;
        m_endReason = GameEndReason::THREEFOLD_REPETITION;
        return m_currentResult;
    }
    
    // Vérifier le matériel insuffisant
    if (hasInsufficientMaterial()) {
        m_currentResult = GameResult::DRAW;
        m_endReason = GameEndReason::INSUFFICIENT_MATERIAL;
        return m_currentResult;
    }
    
    // La partie continue
    m_currentResult = GameResult::ONGOING;
    m_endReason = GameEndReason::NONE;
    return m_currentResult;
}

bool GameEndEvaluator::isCheckmate(const std::string& playerColor) const {
    if (!m_validator) return false;
    return m_validator->isCheckmate(playerColor);
}

bool GameEndEvaluator::isStalemate(const std::string& playerColor) const {
    if (!m_validator) return false;
    return m_validator->isStalemate(playerColor);
}

bool GameEndEvaluator::hasInsufficientMaterial() const {
    if (!m_board) return false;
    
    // Roi vs Roi
    if (hasOnlyKings()) {
        return true;
    }
    
    // Roi + Fou vs Roi
    if (hasKingVsKingBishop()) {
        return true;
    }
    
    // Roi + Cavalier vs Roi
    if (hasKingVsKingKnight()) {
        return true;
    }
    
    // Roi + Fou vs Roi + Fou (même couleur de case)
    if (hasKingBishopVsKingBishop()) {
        return true;
    }
    
    return false;
}

bool GameEndEvaluator::isFiftyMoveRule() const {
    // La règle des 50 coups signifie 100 demi-coups (50 coups complets)
    return m_halfMoveClock >= 100;
}

bool GameEndEvaluator::isThreefoldRepetition() const {
    std::string currentHash = generatePositionHash();
    
    auto it = m_positionHistory.find(currentHash);
    if (it != m_positionHistory.end()) {
        return it->second >= 3;
    }
    
    return false;
}

std::string GameEndEvaluator::getResultDescription() const {
    switch (m_currentResult) {
        case GameResult::WHITE_WIN:
            switch (m_endReason) {
                case GameEndReason::CHECKMATE:
                    return "White wins by checkmate!";
                case GameEndReason::RESIGNATION:
                    return "White wins by resignation!";
                case GameEndReason::TIMEOUT:
                    return "White wins on time!";
                default:
                    return "White wins!";
            }
            
        case GameResult::BLACK_WIN:
            switch (m_endReason) {
                case GameEndReason::CHECKMATE:
                    return "Black wins by checkmate!";
                case GameEndReason::RESIGNATION:
                    return "Black wins by resignation!";
                case GameEndReason::TIMEOUT:
                    return "Black wins on time!";
                default:
                    return "Black wins!";
            }
            
        case GameResult::DRAW:
            switch (m_endReason) {
                case GameEndReason::STALEMATE:
                    return "Draw by stalemate";
                case GameEndReason::INSUFFICIENT_MATERIAL:
                    return "Draw by insufficient material";
                case GameEndReason::FIFTY_MOVE_RULE:
                    return "Draw by fifty-move rule";
                case GameEndReason::THREEFOLD_REPETITION:
                    return "Draw by threefold repetition";
                default:
                    return "Draw";
            }
            
        case GameResult::ONGOING:
        default:
            return "Game in progress";
    }
}

void GameEndEvaluator::updateAfterMove(bool isPawnMove, bool isCapture) {
    // Réinitialiser le compteur si c'est un mouvement de pion ou une capture
    if (isPawnMove || isCapture) {
        m_halfMoveClock = 0;
    } else {
        m_halfMoveClock++;
    }
}

void GameEndEvaluator::recordCurrentPosition() {
    std::string positionHash = generatePositionHash();
    m_positionHistory[positionHash]++;
}

void GameEndEvaluator::reset() {
    m_currentResult = GameResult::ONGOING;
    m_endReason = GameEndReason::NONE;
    m_halfMoveClock = 0;
    m_positionHistory.clear();
}

std::string GameEndEvaluator::generatePositionHash() const {
    if (!m_board) return "";
    
    std::stringstream ss;
    
    // Encoder la position de toutes les pièces
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            const ChessPiece* piece = m_board->getPieceAt(row, col);
            if (piece && !piece->type.empty()) {
                ss << row << col << piece->type[0] << piece->color[0] << "|";
            }
        }
    }
    
    // Note: Pour une implémentation complète, il faudrait aussi encoder:
    // - Le joueur actif
    // - Les droits de roque
    // - La possibilité de prise en passant
    // Pour le moment, on se concentre sur la position des pièces
    
    return ss.str();
}

bool GameEndEvaluator::hasOnlyKings() const {
    int totalPieces = 0;
    int kingCount = 0;
    
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            const ChessPiece* piece = m_board->getPieceAt(row, col);
            if (piece && !piece->type.empty()) {
                totalPieces++;
                if (piece->type == "king") {
                    kingCount++;
                }
            }
        }
    }
    
    return totalPieces == 2 && kingCount == 2;
}

bool GameEndEvaluator::hasKingVsKingBishop() const {
    int whiteKing = countPieces("king", "white");
    int blackKing = countPieces("king", "black");
    
    int whiteBishop = countPieces("bishop", "white");
    int blackBishop = countPieces("bishop", "black");
    
    int whiteOther = 0;
    int blackOther = 0;
    
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            const ChessPiece* piece = m_board->getPieceAt(row, col);
            if (piece && !piece->type.empty() && piece->type != "king" && piece->type != "bishop") {
                if (piece->color == "white") whiteOther++;
                else blackOther++;
            }
        }
    }
    
    // Roi + Fou vs Roi
    if (whiteKing == 1 && blackKing == 1 && whiteOther == 0 && blackOther == 0) {
        return (whiteBishop == 1 && blackBishop == 0) || (whiteBishop == 0 && blackBishop == 1);
    }
    
    return false;
}

bool GameEndEvaluator::hasKingVsKingKnight() const {
    int whiteKing = countPieces("king", "white");
    int blackKing = countPieces("king", "black");
    
    int whiteKnight = countPieces("knight", "white");
    int blackKnight = countPieces("knight", "black");
    
    int whiteOther = 0;
    int blackOther = 0;
    
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            const ChessPiece* piece = m_board->getPieceAt(row, col);
            if (piece && !piece->type.empty() && piece->type != "king" && piece->type != "knight") {
                if (piece->color == "white") whiteOther++;
                else blackOther++;
            }
        }
    }
    
    // Roi + Cavalier vs Roi
    if (whiteKing == 1 && blackKing == 1 && whiteOther == 0 && blackOther == 0) {
        return (whiteKnight == 1 && blackKnight == 0) || (whiteKnight == 0 && blackKnight == 1);
    }
    
    return false;
}

bool GameEndEvaluator::hasKingBishopVsKingBishop() const {
    int whiteKing = countPieces("king", "white");
    int blackKing = countPieces("king", "black");
    
    int whiteBishop = countPieces("bishop", "white");
    int blackBishop = countPieces("bishop", "black");
    
    // Vérifier qu'il n'y a que des rois et fous
    int totalPieces = 0;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            const ChessPiece* piece = m_board->getPieceAt(row, col);
            if (piece && !piece->type.empty()) {
                totalPieces++;
                if (piece->type != "king" && piece->type != "bishop") {
                    return false;
                }
            }
        }
    }
    
    // Roi + Fou vs Roi + Fou (exactement 4 pièces)
    if (whiteKing == 1 && blackKing == 1 && whiteBishop == 1 && blackBishop == 1) {
        // Vérifier si les deux fous sont sur la même couleur de case
        bool whiteBishopOnLight = false;
        bool blackBishopOnLight = false;
        
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                const ChessPiece* piece = m_board->getPieceAt(row, col);
                if (piece && piece->type == "bishop") {
                    bool isLightSquare = ((row + col) % 2 == 0);
                    if (piece->color == "white") {
                        whiteBishopOnLight = isLightSquare;
                    } else {
                        blackBishopOnLight = isLightSquare;
                    }
                }
            }
        }
        
        return whiteBishopOnLight == blackBishopOnLight;
    }
    
    return false;
}

int GameEndEvaluator::countPieces(const std::string& type, const std::string& color) const {
    int count = 0;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            const ChessPiece* piece = m_board->getPieceAt(row, col);
            if (piece && piece->type == type && piece->color == color) {
                count++;
            }
        }
    }
    return count;
}

bool GameEndEvaluator::allBishopsOnSameColorSquares(const std::string& color) const {
    bool hasLightSquareBishop = false;
    bool hasDarkSquareBishop = false;
    
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            const ChessPiece* piece = m_board->getPieceAt(row, col);
            if (piece && piece->type == "bishop" && piece->color == color) {
                if ((row + col) % 2 == 0) {
                    hasLightSquareBishop = true;
                } else {
                    hasDarkSquareBishop = true;
                }
            }
        }
    }
    
    // Tous sur la même couleur si on n'a qu'un seul type
    return hasLightSquareBishop != hasDarkSquareBishop;
}

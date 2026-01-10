#include "SaveLoadManager.h"
#include "ChessBoard.h"
#include "ChessPiece.h"
#include "Color.h"
#include "Logger.h"
#include <fstream>
#include <unordered_map>

// Simple custom format:
// 8x8 tokens row-major. Each cell is two chars: color + type (e.g., wP, bK), or ".." for empty.
// Color: 'w' = white, 'b' = black
// Type char mapping: P=Pawn, R=Rook, N=Knight, B=Bishop, Q=Queen, K=King

static char typeStringToChar(const std::string& type) {
    if (type == "pawn") return 'P';
    if (type == "rook") return 'R';
    if (type == "knight") return 'N';
    if (type == "bishop") return 'B';
    if (type == "queen") return 'Q';
    if (type == "king") return 'K';
    return '?';
}

static std::string typeCharToString(char c) {
    switch (c) {
    case 'P': case 'p': return "pawn";
    case 'R': case 'r': return "rook";
    case 'N': case 'n': return "knight";
    case 'B': case 'b': return "bishop";
    case 'Q': case 'q': return "queen";
    case 'K': case 'k': return "king";
    default: return "";
    }
}

bool SaveLoadManager::saveToFile(const std::string& filePath, const ChessBoard& board) {
    std::ofstream out(filePath);
    if (!out.is_open()) {
        Logger::getInstance().logError("Failed to open file for saving: " + filePath);
        return false;
    }           

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            const ChessPiece* p = board.getPieceAt(r, c);
            if (p && !p->type.empty()) {
                char colorChar = (p->color == "white") ? 'w' : 'b';
                char typeChar = typeStringToChar(p->type);
                out << colorChar << typeChar;
            } else {
                out << "..";
            }
            if (c < 7) out << ' ';
        }
        out << '\n';
    }

    Logger::getInstance().logMessage("Saved board to " + filePath);
    return true;
}

bool SaveLoadManager::loadFromFile(const std::string& filePath, ChessBoard& board) {
    std::ifstream in(filePath);
    if (!in.is_open()) {
        Logger::getInstance().logError("Failed to open file for loading: " + filePath);
        return false;
    }

    // Clear existing pieces
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            board.removePiece(r, c);
        }
    }

    // Parse tokens row by row
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            std::string token;
            if (!(in >> token)) {
                Logger::getInstance().logError("Invalid save format: unexpected EOF at row " + std::to_string(r));
                return false;
            }
            if (token == "..") {
                // already cleared above, but ensure slot is empty
                board.removePiece(r, c);
            } else if (token.size() == 2) {
                char colorChar = token[0];
                char typeChar = token[1];

                std::string color;
                if (colorChar == 'w') color = "white";
                else if (colorChar == 'b') color = "black";
                else {
                    Logger::getInstance().logError("Invalid color in token: " + token);
                    return false;
                }

                std::string type = typeCharToString(typeChar);
                if (type.empty()) {
                    Logger::getInstance().logError("Invalid piece type in token: " + token);
                    return false;
                }

                board.setPieceByNames(r, c, type, color);
            } else {
                Logger::getInstance().logError("Invalid token size: " + token);
                return false;
            }
        }
    }

    Logger::getInstance().logMessage("Loaded board from " + filePath);
    return true;
}

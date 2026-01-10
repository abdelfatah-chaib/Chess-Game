#pragma once

#include <string>

enum class SpecialMoveType {
    None,
    Castling,
    EnPassant,
    PawnPromotion
};

struct Move {
    int fromRow, fromCol, toRow, toCol;
    std::string capturedType;
    std::string capturedColor;
    bool wasFirstMove;
    SpecialMoveType specialMove;
    std::string promotionPiece; // For pawn promotion: "queen", "rook", "bishop", "knight"
    
    Move(int fr, int fc, int tr, int tc, const std::string& ct = "",
         const std::string& cc = "", bool firstMove = false, 
         SpecialMoveType special = SpecialMoveType::None,
         const std::string& promotion = "");
};

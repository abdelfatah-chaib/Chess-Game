#include "Entities/ChessPiece.h"
#include "Move.h"

ChessPiece::ChessPiece() : hasMoved(false), row(-1), col(-1) {}

Move::Move(int fr, int fc, int tr, int tc, const std::string& ct,
    const std::string& cc, bool firstMove, SpecialMoveType special,
    const std::string& promotion)
    : fromRow(fr), fromCol(fc), toRow(tr), toCol(tc),
    capturedType(ct), capturedColor(cc), wasFirstMove(firstMove),
    specialMove(special), promotionPiece(promotion) {
}

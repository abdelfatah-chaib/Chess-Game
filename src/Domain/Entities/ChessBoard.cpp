#include "ChessBoard.h"
#include "Entities/ChessPiece.h"
#include "BoardTheme.h"
#include "Rules/MoveValidator.h"
#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;

ChessBoard::ChessBoard() : texturesLoaded(false), squareSize(50.f), boardX(50.f), boardY(50.f), halfMoveClock(0), currentPieceSet(PieceSetType::CLASSIC),
    whiteKingDangerPos(-1, -1), blackKingDangerPos(-1, -1) {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            pieces[row][col] = nullptr;
        }
    }
    
    // Set default theme
    currentTheme = BoardThemeManager::getThemeColors(BoardTheme::Wooden);
}

ChessBoard::~ChessBoard() {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (pieces[row][col]) {
                delete pieces[row][col];
                pieces[row][col] = nullptr;
            }
        }
    }
}

bool ChessBoard::createBlackPieceFromWhite(sf::Texture& blackTexture, const std::string& whitePieceName) {
    std::string basePath = "C:/Users/abdel/OneDrive - uit.ac.ma/Bureau/M1-IAOC/Conception and Programing CPP/ChessMasterUIT-Project/ChessMasterUIT/assets/";
    std::string whitePath = basePath + whitePieceName + ".png";

    sf::Image whiteImage;
    if (!whiteImage.loadFromFile(whitePath)) {
        cout<< "Impossible de charger: " << whitePath << std::endl;
        return false;
    }

    sf::Image blackImage;
    blackImage.create(whiteImage.getSize().x, whiteImage.getSize().y, sf::Color::Transparent);

    for (unsigned int x = 0; x < whiteImage.getSize().x; ++x) {
        for (unsigned int y = 0; y < whiteImage.getSize().y; ++y) {
            sf::Color pixel = whiteImage.getPixel(x, y);

            if (pixel.a > 0) {
                if (pixel.r > 200 && pixel.g > 200 && pixel.b > 200) {
                    blackImage.setPixel(x, y, sf::Color(50, 50, 50, pixel.a));
                }
                else if (pixel.r > 150 || pixel.g > 150 || pixel.b > 150) {
                    blackImage.setPixel(x, y, sf::Color(80, 80, 80, pixel.a));
                }
                else {
                    blackImage.setPixel(x, y, sf::Color(
                        std::max(0, pixel.r - 100),
                        std::max(0, pixel.g - 100),
                        std::max(0, pixel.b - 100),
                        pixel.a
                    ));
                }
            }
        }
    }

    return blackTexture.loadFromImage(blackImage);
}

bool ChessBoard::loadTextures() {
    std::string pieceTypes[] = { "pawn", "rook", "knight", "bishop", "queen", "king" };
    std::string basePath = "C:/Users/abdel/OneDrive - uit.ac.ma/Bureau/M1-IAOC/Conception and Programing CPP/ChessMasterUIT-Project/ChessMasterUIT/assets/";

    int index = 0;

    for (const auto& type : pieceTypes) {
        std::string filename = basePath + "w_" + type + ".png";
        if (!pieceTextures[index].loadFromFile(filename)) {
            std::cout << "Impossible de charger la pièce blanche: " << filename << std::endl;
            sf::Image img;
            img.create(64, 64, sf::Color(200, 200, 200, 128));
            pieceTextures[index].loadFromImage(img);
        }
        index++;
    }

    for (const auto& type : pieceTypes) {
        if (!createBlackPieceFromWhite(pieceTextures[index], "w_" + type)) {
            std::cout << "Impossible de créer la pièce noire pour: " << type << std::endl;
            sf::Image img;
            img.create(64, 64, sf::Color(50, 50, 50, 128));
            pieceTextures[index].loadFromImage(img);
        }
        index++;
    }

    texturesLoaded = true;
    std::cout << "Textures chargées: " << index << " pièces (6 blanches, 6 noires générées)" << std::endl;
    return true;
}

void ChessBoard::initialize(float x, float y, float size) {
    // Use current theme or default
    initialize(x, y, size, currentTheme);
}

void ChessBoard::initialize(float x, float y, float size, const ThemeColors& theme) {
    std::cout << "[ChessBoard] initialize called: " << x << "," << y << " size=" << size << std::endl;
    
    boardX = x;
    boardY = y;
    squareSize = size;
    currentTheme = theme;

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            boardSquares[row][col].setSize(sf::Vector2f(squareSize, squareSize));
            boardSquares[row][col].setPosition(boardX + col * squareSize, boardY + row * squareSize);

            if ((row + col) % 2 == 0) {
                boardSquares[row][col].setFillColor(theme.LIGHT);
            }
            else {
                boardSquares[row][col].setFillColor(theme.DARK);
            }
        }
    }

    // Only initialize pieces if they don't exist yet
    bool piecesExist = false;
    for (int row = 0; row < 8 && !piecesExist; row++) {
        for (int col = 0; col < 8 && !piecesExist; col++) {
            if (pieces[row][col] != nullptr) {
                piecesExist = true;
            }
        }
    }

    if (!piecesExist) {
        std::cout << "[ChessBoard] Setting up initial pieces" << std::endl;
        
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                if (pieces[row][col]) {
                    delete pieces[row][col];
                }
                pieces[row][col] = nullptr;
            }
        }

        for (int col = 0; col < 8; col++) {
            setupPiece(1, col, "pawn", "black");
            setupPiece(6, col, "pawn", "white");
        }

        std::string backRow[] = { "rook", "knight", "bishop", "queen", "king", "bishop", "knight", "rook" };
        for (int col = 0; col < 8; col++) {
            setupPiece(0, col, backRow[col], "black");
            setupPiece(7, col, backRow[col], "white");
        }

        while (!moveHistory.empty()) moveHistory.pop();
        
        // Réinitialiser le tracking de fin de partie
        halfMoveClock = 0;
        positionHistory.clear();
        recordCurrentPosition();
    } else {
        std::cout << "[ChessBoard] Updating existing piece positions for new geometry" << std::endl;
        // Update sprite positions for existing pieces
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                if (pieces[row][col] && !pieces[row][col]->type.empty()) {
                    pieces[row][col]->sprite.setPosition(
                        boardX + col * squareSize + squareSize / 2,
                        boardY + row * squareSize + squareSize / 2
                    );
                }
            }
        }
    }
}

void ChessBoard::setupPiece(int row, int col, const std::string& type, const std::string& color) {
    if (!texturesLoaded) {
        // still create a minimal ChessPiece to avoid null checks elsewhere
        if (pieces[row][col]) delete pieces[row][col];
        pieces[row][col] = new ChessPiece();
        pieces[row][col]->type = type;
        pieces[row][col]->color = color;
        pieces[row][col]->row = row;
        pieces[row][col]->col = col;
        pieces[row][col]->hasMoved = false;
        return;
    }

    int textureIndex = (color == "white" ? 0 : 6);
    if (type == "pawn") textureIndex += 0;
    else if (type == "rook") textureIndex += 1;
    else if (type == "knight") textureIndex += 2;
    else if (type == "bishop") textureIndex += 3;
    else if (type == "queen") textureIndex += 4;
    else if (type == "king") textureIndex += 5;

    if (pieces[row][col]) {
        delete pieces[row][col];
        pieces[row][col] = nullptr;
    }

    pieces[row][col] = new ChessPiece();
    pieces[row][col]->sprite.setTexture(pieceTextures[textureIndex]);
    pieces[row][col]->type = type;
    pieces[row][col]->color = color;
    pieces[row][col]->row = row;
    pieces[row][col]->col = col;
    pieces[row][col]->hasMoved = false;

    float scale = squareSize / std::max(pieceTextures[textureIndex].getSize().x,
        pieceTextures[textureIndex].getSize().y) * 0.8f;
    pieces[row][col]->sprite.setScale(scale, scale);

    sf::FloatRect bounds = pieces[row][col]->sprite.getLocalBounds();
    pieces[row][col]->sprite.setOrigin(bounds.width / 2, bounds.height / 2);
    pieces[row][col]->sprite.setPosition(
        boardX + col * squareSize + squareSize / 2,
        boardY + row * squareSize + squareSize / 2
    );
}

void ChessBoard::draw(sf::RenderWindow& window) {
    // 1. Dessiner les cases de l'échiquier
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            window.draw(boardSquares[row][col]);
        }
    }
    
    // 2. Dessiner le surlignage du dernier coup (vert) AVANT le danger du roi
    drawLastMoveHighlight(window);
    
    // 3. Dessiner la surbrillance de danger du roi (rouge) par-dessus le coup vert
    // Le rouge a priorité plus élevée et couvrira le vert si la case est la même
    highlightKingDanger(window);

    // 4. Dessiner les pièces par-dessus tout
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (pieces[row][col] && !pieces[row][col]->type.empty()) {
                window.draw(pieces[row][col]->sprite);
            }
        }
    }
}

void ChessBoard::highlightSquare(int row, int col, sf::RenderWindow& window, sf::Color color) {
    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
        sf::RectangleShape highlight(sf::Vector2f(squareSize, squareSize));
        highlight.setFillColor(color);
        highlight.setPosition(boardX + col * squareSize, boardY + row * squareSize);
        window.draw(highlight);
    }
}

void ChessBoard::drawLegalMoveIndicator(int row, int col, sf::RenderWindow& window) {
    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
        float centerX = boardX + col * squareSize + squareSize / 2;
        float centerY = boardY + row * squareSize + squareSize / 2;
        
        sf::CircleShape indicator(squareSize * 0.15f); // Point vert relativement petit
        indicator.setRadius(squareSize * 0.15f);
        indicator.setFillColor(sf::Color(0, 255, 0, 180)); // Vert semi-transparent
        indicator.setOutlineColor(sf::Color(0, 200, 0, 255)); // Contour vert plus foncé
        indicator.setOutlineThickness(2.0f);
        
        // Centrer le cercle
        indicator.setOrigin(indicator.getRadius(), indicator.getRadius());
        indicator.setPosition(centerX, centerY);
        
        window.draw(indicator);
    }
}

ChessPiece* ChessBoard::getPieceAt(int row, int col) {
    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
        return pieces[row][col];
    }
    return nullptr;
}

const ChessPiece* ChessBoard::getPieceAt(int row, int col) const {
    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
        return pieces[row][col];
    }
    return nullptr;
}

void ChessBoard::removePiece(int row, int col) {
    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
        if (pieces[row][col]) {
            delete pieces[row][col];
            pieces[row][col] = nullptr;
        }
    }
}

void ChessBoard::setPieceByNames(int row, int col, const std::string& type, const std::string& color) {
    if (row < 0 || row >= 8 || col < 0 || col >= 8) return;

    if (pieces[row][col]) {
        delete pieces[row][col];
        pieces[row][col] = nullptr;
    }

    // setupPiece will handle textures and positioning
    setupPiece(row, col, type, color);
}

// Integrated move validation logic from extracted code
bool ChessBoard::isValidPawnMove(int fromRow, int fromCol, int toRow, int toCol, const std::string& color) {
    int direction = (color == "white") ? -1 : 1;
    ChessPiece* target = getPieceAt(toRow, toCol);
    ChessPiece* fromPiece = getPieceAt(fromRow, fromCol);

    // Forward movement
    if (fromCol == toCol) {
        if (target && !target->type.empty()) return false; // must be empty
        
        // One square forward
        if (toRow == fromRow + direction) return true;
        
        // Two squares on first move
        if (fromPiece && !fromPiece->hasMoved && toRow == fromRow + 2 * direction) {
            ChessPiece* intermediate = getPieceAt(fromRow + direction, fromCol);
            return (!intermediate || intermediate->type.empty());
        }
    }
    // Diagonal capture
    else if (abs(fromCol - toCol) == 1 && toRow == fromRow + direction) {
        return (target && !target->type.empty() && target->color != color);
    }

    return false;
}

bool ChessBoard::isValidRookMove(int fromRow, int fromCol, int toRow, int toCol) {
    if (fromRow != toRow && fromCol != toCol) return false;

    // Check path is clear
    if (fromRow == toRow) { // horizontal
        int step = (toCol > fromCol) ? 1 : -1;
        for (int col = fromCol + step; col != toCol; col += step) {
            if (getPieceAt(fromRow, col) && !getPieceAt(fromRow, col)->type.empty()) {
                return false;
            }
        }
    } else { // vertical
        int step = (toRow > fromRow) ? 1 : -1;
        for (int row = fromRow + step; row != toRow; row += step) {
            if (getPieceAt(row, fromCol) && !getPieceAt(row, fromCol)->type.empty()) {
                return false;
            }
        }
    }
    return true;
}

bool ChessBoard::isValidKnightMove(int fromRow, int fromCol, int toRow, int toCol) {
    int rowDiff = abs(fromRow - toRow);
    int colDiff = abs(fromCol - toCol);
    return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
}

bool ChessBoard::isValidBishopMove(int fromRow, int fromCol, int toRow, int toCol) {
    if (abs(fromRow - toRow) != abs(fromCol - toCol)) return false;

    int rowStep = (toRow > fromRow) ? 1 : -1;
    int colStep = (toCol > fromCol) ? 1 : -1;
    int steps = abs(fromRow - toRow);
    
    for (int i = 1; i < steps; i++) {
        int checkRow = fromRow + i * rowStep;
        int checkCol = fromCol + i * colStep;
        if (getPieceAt(checkRow, checkCol) && !getPieceAt(checkRow, checkCol)->type.empty()) {
            return false;
        }
    }
    return true;
}

bool ChessBoard::isValidQueenMove(int fromRow, int fromCol, int toRow, int toCol) {
    return isValidRookMove(fromRow, fromCol, toRow, toCol) ||
           isValidBishopMove(fromRow, fromCol, toRow, toCol);
}

bool ChessBoard::isValidKingMove(int fromRow, int fromCol, int toRow, int toCol) {
    int rowDiff = abs(fromRow - toRow);
    int colDiff = abs(fromCol - toCol);
    return (rowDiff <= 1 && colDiff <= 1);
}

bool ChessBoard::isValidMove(int fromRow, int fromCol, int toRow, int toCol) {
    ChessPiece* fromPiece = getPieceAt(fromRow, fromCol);
    ChessPiece* toPiece = getPieceAt(toRow, toCol);

    if (!fromPiece || fromPiece->type.empty()) return false;
    if (toPiece && !toPiece->type.empty() && toPiece->color == fromPiece->color) return false;

    const std::string& type = fromPiece->type;
    if (type == "pawn") return isValidPawnMove(fromRow, fromCol, toRow, toCol, fromPiece->color);
    else if (type == "rook") return isValidRookMove(fromRow, fromCol, toRow, toCol);
    else if (type == "knight") return isValidKnightMove(fromRow, fromCol, toRow, toCol);
    else if (type == "bishop") return isValidBishopMove(fromRow, fromCol, toRow, toCol);
    else if (type == "queen") return isValidQueenMove(fromRow, fromCol, toRow, toCol);
    else if (type == "king") return isValidKingMove(fromRow, fromCol, toRow, toCol);

    return false;
}

bool ChessBoard::movePiece(int fromRow, int fromCol, int toRow, int toCol, const std::string& promotionPiece) {
    std::cout << "[ChessBoard] movePiece called: (" << fromRow << "," << fromCol << ") -> (" << toRow << "," << toCol << ")" << std::endl;
    
    if (fromRow < 0 || fromRow >= 8 || fromCol < 0 || fromCol >= 8 ||
        toRow < 0 || toRow >= 8 || toCol < 0 || toCol >= 8) {
        std::cout << "[ChessBoard] Move out of bounds" << std::endl;
        return false;
    }

    // Validate move using integrated logic
    if (!isValidMove(fromRow, fromCol, toRow, toCol)) {
        std::cout << "[ChessBoard] Move validation failed" << std::endl;
        return false;
    }

    ChessPiece* fromPiece = pieces[fromRow][fromCol];
    ChessPiece* toPiece = pieces[toRow][toCol];

    if (!fromPiece || fromPiece->type.empty()) {
        std::cout << "[ChessBoard] No piece to move" << std::endl;
        return false;
    }

    std::cout << "[ChessBoard] Moving " << fromPiece->type << " " << fromPiece->color << std::endl;

    std::string capturedType = toPiece ? toPiece->type : "";
    std::string capturedColor = toPiece ? toPiece->color : "";
    bool wasFirstMove = fromPiece->hasMoved;
    
    // Mettre à jour le compteur de demi-coups pour la règle des 50 coups
    bool isPawnMove = (fromPiece->type == "pawn");
    bool isCapture = (toPiece && !toPiece->type.empty());
    
    if (isPawnMove || isCapture) {
        halfMoveClock = 0;
    } else {
        halfMoveClock++;
    }

    moveHistory.push(Move(fromRow, fromCol, toRow, toCol, capturedType, capturedColor, wasFirstMove));

    if (toPiece) {
        std::cout << "[ChessBoard] Capturing " << toPiece->type << " " << toPiece->color << std::endl;
        delete toPiece;
    }

    pieces[toRow][toCol] = fromPiece;
    pieces[fromRow][fromCol] = nullptr;

    fromPiece->row = toRow;
    fromPiece->col = toCol;
    fromPiece->hasMoved = true;

    // Recalculate position based on current board geometry
    fromPiece->sprite.setPosition(
        boardX + toCol * squareSize + squareSize / 2,
        boardY + toRow * squareSize + squareSize / 2
    );
    
    // Record last move for visual highlighting
    setLastMove(fromRow, fromCol, toRow, toCol);
    
    // Check for pawn promotion
    if (isPawnMove) {
        bool isWhite = (fromPiece->color == "white");
        int promotionRow = isWhite ? 0 : 7;
        
        if (toRow == promotionRow) {
            // Only auto-promote if a piece is explicitly specified
            if (!promotionPiece.empty()) {
                std::cout << "[ChessBoard] Pawn reached promotion row! Promoting to " << promotionPiece << std::endl;
                promotePawn(toRow, toCol, promotionPiece);
            } else {
                std::cout << "[ChessBoard] Pawn reached promotion row - waiting for piece selection" << std::endl;
            }
        }
    }
    
    // Enregistrer la nouvelle position pour la détection de répétition
    recordCurrentPosition();

    std::cout << "[ChessBoard] Move completed successfully" << std::endl;
    return true;
}

// Overload for backward compatibility (automatic queen promotion for AI)
bool ChessBoard::movePiece(int fromRow, int fromCol, int toRow, int toCol) {
    return movePiece(fromRow, fromCol, toRow, toCol, "queen");
}

bool ChessBoard::undoMove() {
    if (moveHistory.empty()) return false;

    Move lastMove = moveHistory.top();
    moveHistory.pop();

    ChessPiece* movedPiece = pieces[lastMove.toRow][lastMove.toCol];

    pieces[lastMove.fromRow][lastMove.fromCol] = movedPiece;
    pieces[lastMove.toRow][lastMove.toCol] = nullptr;

    if (!lastMove.capturedType.empty()) {
        setupPiece(lastMove.toRow, lastMove.toCol, lastMove.capturedType, lastMove.capturedColor);
    }

    if (movedPiece) {
        movedPiece->row = lastMove.fromRow;
        movedPiece->col = lastMove.fromCol;
        movedPiece->hasMoved = lastMove.wasFirstMove;

        movedPiece->sprite.setPosition(
            boardX + lastMove.fromCol * squareSize + squareSize / 2,
            boardY + lastMove.fromRow * squareSize + squareSize / 2
        );
    }
    
    // Clear last move highlight on undo
    clearLastMove();
    
    // Retirer la dernière position de l'historique
    if (!positionHistory.empty()) {
        positionHistory.pop_back();
    }
    
    // Réduire le compteur de demi-coups (approximatif, pas parfait pour undo)
    if (halfMoveClock > 0) {
        halfMoveClock--;
    }

    return true;
}

// ================================
// King Danger Detection & Visual Alert System
// ================================

std::pair<int, int> ChessBoard::getKingPosition(const std::string& color) const {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            const ChessPiece* piece = pieces[row][col];
            if (piece && piece->type == "king" && piece->color == color) {
                return {row, col};
            }
        }
    }
    return {-1, -1}; // King not found (should never happen in valid game)
}

bool ChessBoard::isKingInDanger(const std::string& color) const {
    // Cette méthode délègue au MoveValidator::isInCheck
    // Elle sera appelée depuis GameController qui a accès au validator
    // Ici on retourne juste si la position de danger est définie
    if (color == "white") {
        return whiteKingDangerPos.first != -1 && whiteKingDangerPos.second != -1;
    } else {
        return blackKingDangerPos.first != -1 && blackKingDangerPos.second != -1;
    }
}

void ChessBoard::updateKingDangerStatus(const std::string& color, const MoveValidator* validator) {
    if (!validator) {
        clearKingDangerStatus();
        return;
    }
    
    // Vérifier si le roi est en échec
    bool inCheck = validator->isInCheck(color);
    
    if (inCheck) {
        // Trouver la position du roi
        std::pair<int, int> kingPos = getKingPosition(color);
        
        if (kingPos.first != -1) {
            std::cout << "[ChessBoard] " << color << " king is IN DANGER at (" 
                      << kingPos.first << "," << kingPos.second << ")" << std::endl;
            
            // Enregistrer la position de danger
            if (color == "white") {
                whiteKingDangerPos = kingPos;
            } else {
                blackKingDangerPos = kingPos;
            }
        }
    } else {
        // Le roi n'est pas en danger, effacer le statut
        if (color == "white") {
            whiteKingDangerPos = {-1, -1};
        } else {
            blackKingDangerPos = {-1, -1};
        }
    }
}

void ChessBoard::clearKingDangerStatus() {
    whiteKingDangerPos = {-1, -1};
    blackKingDangerPos = {-1, -1};
}

void ChessBoard::highlightKingDanger(sf::RenderWindow& window) {
    // Dessiner la surbrillance rouge pour le roi blanc si en danger
    if (whiteKingDangerPos.first != -1 && whiteKingDangerPos.second != -1) {
        sf::RectangleShape dangerHighlight(sf::Vector2f(squareSize, squareSize));
        dangerHighlight.setFillColor(sf::Color(255, 0, 0, 120)); // Rouge semi-transparent
        dangerHighlight.setPosition(
            boardX + whiteKingDangerPos.second * squareSize,
            boardY + whiteKingDangerPos.first * squareSize
        );
        window.draw(dangerHighlight);
    }
    
    // Dessiner la surbrillance rouge pour le roi noir si en danger
    if (blackKingDangerPos.first != -1 && blackKingDangerPos.second != -1) {
        sf::RectangleShape dangerHighlight(sf::Vector2f(squareSize, squareSize));
        dangerHighlight.setFillColor(sf::Color(255, 0, 0, 120)); // Rouge semi-transparent
        dangerHighlight.setPosition(
            boardX + blackKingDangerPos.second * squareSize,
            boardY + blackKingDangerPos.first * squareSize
        );
        window.draw(dangerHighlight);
    }
}

void ChessBoard::promotePawn(int row, int col, const std::string& promotionPiece) {
    ChessPiece* pawn = pieces[row][col];
    if (!pawn || pawn->type != "pawn") {
        std::cout << "[ChessBoard] ERROR: No pawn at position (" << row << "," << col << ") to promote" << std::endl;
        return;
    }
    
    std::string color = pawn->color;
    std::cout << "[ChessBoard] Promoting " << color << " pawn at (" << row << "," << col << ") to " << promotionPiece << std::endl;
    
    // Delete the pawn
    delete pawn;
    pieces[row][col] = nullptr;
    
    // Create the new promoted piece
    setupPiece(row, col, promotionPiece, color);
    
    std::cout << "[ChessBoard] Pawn promotion complete" << std::endl;
}

sf::FloatRect ChessBoard::getBounds() const {
    return sf::FloatRect(boardX, boardY, squareSize * 8, squareSize * 8);
}

sf::Vector2f ChessBoard::getSquarePosition(int row, int col) const {
    return sf::Vector2f(boardX + col * squareSize, boardY + row * squareSize);
}

std::pair<int, int> ChessBoard::getSquareAtPosition(float x, float y) const {
    if (x < boardX || y < boardY || x >= boardX + 8 * squareSize || y >= boardY + 8 * squareSize) {
        return { -1, -1 };
    }

    int col = static_cast<int>((x - boardX) / squareSize);
    int row = static_cast<int>((y - boardY) / squareSize);

    return { row, col };        
}

bool ChessBoard::isInsideBoard(int row, int col) const {
    return row >= 0 && row < 8 && col >= 0 && col < 8;
}

void ChessBoard::recordCurrentPosition() {
    std::string hash = generatePositionHash();
    positionHistory.push_back(hash);
}

std::string ChessBoard::generatePositionHash() const {
    std::string hash;
    
    // Encoder la position de toutes les pièces
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            const ChessPiece* piece = getPieceAt(row, col);
            if (piece && !piece->type.empty()) {
                hash += std::to_string(row) + std::to_string(col);
                hash += piece->type.substr(0, 1);
                hash += piece->color.substr(0, 1);
                hash += "|";
            }
        }
    }
    
    return hash;
}

void ChessBoard::setTheme(const ThemeColors& theme) {
    currentTheme = theme;
    
    // Update square colors
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if ((row + col) % 2 == 0) {
                boardSquares[row][col].setFillColor(theme.LIGHT);
            }
            else {
                boardSquares[row][col].setFillColor(theme.DARK);
            }
        }
    }
}

void ChessBoard::reset() {
    std::cout << "[ChessBoard] Resetting board to initial state" << std::endl;
    
    // Clear all existing pieces
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (pieces[row][col]) {
                delete pieces[row][col];
                pieces[row][col] = nullptr;
            }
        }
    }
    
    // Setup pawns
    for (int col = 0; col < 8; col++) {
        setupPiece(1, col, "pawn", "black");
        setupPiece(6, col, "pawn", "white");
    }
    
    // Setup back row pieces
    std::string backRow[] = { "rook", "knight", "bishop", "queen", "king", "bishop", "knight", "rook" };
    for (int col = 0; col < 8; col++) {
        setupPiece(0, col, backRow[col], "black");
        setupPiece(7, col, backRow[col], "white");
    }
    
    // Clear move history
    while (!moveHistory.empty()) {
        moveHistory.pop();
    }
    
    // Clear last move highlight on reset
    clearLastMove();
    
    // Reset game end tracking
    halfMoveClock = 0;
    positionHistory.clear();
    recordCurrentPosition();
    
    std::cout << "[ChessBoard] Board reset complete - all pieces in starting positions" << std::endl;
}

bool ChessBoard::loadThemedPiece(sf::Texture& texture, const std::string& pieceName, const std::string& color, const BoardTheme& theme) {
    std::string basePath = "C:/Users/abdel/OneDrive - uit.ac.ma/Bureau/M1-IAOC/Conception and Programing CPP/ChessMasterUIT-Project/ChessMasterUIT/pieces/";
    
    std::string folderName;
    if (color == "white") {
        folderName = "player white";
    } else {
        // Black pieces vary by theme
        switch (theme) {
            case BoardTheme::Wooden:
                folderName = "wooden";
                break;
            case BoardTheme::RedWine:
                folderName = "redwine";
                break;
            case BoardTheme::BlueSky:
                folderName = "bluesky";
                break;
            default:
                folderName = "wooden";
                break;
        }
    }
    
    std::string fullPath = basePath + folderName + "/" + pieceName + ".png";
    
    if (texture.loadFromFile(fullPath)) {
        std::cout << "[ChessBoard] Loaded themed piece: " << fullPath << std::endl;
        return true;
    } else {
        std::cout << "[ChessBoard] Failed to load themed piece: " << fullPath << std::endl;
        return false;
    }
}

bool ChessBoard::loadTextures(PieceSetType pieceSet, BoardTheme theme) {
    std::cout << "[ChessBoard] Loading textures for piece set: " << (pieceSet == PieceSetType::CLASSIC ? "CLASSIC" : "THEMED") << std::endl;
    
    currentPieceSet = pieceSet;
    
    if (pieceSet == PieceSetType::CLASSIC) {
        // Load default classic pieces
        return loadTextures();
    } else {
        // Load themed pieces
        std::string pieceTypes[] = { "pawn", "rook", "knight", "bishop", "queen", "king" };
        int index = 0;
        
        // Load white themed pieces
        for (const auto& type : pieceTypes) {
            if (!loadThemedPiece(pieceTextures[index], type, "white", theme)) {
                std::cout << "[ChessBoard] Fallback to generating white piece for: " << type << std::endl;
                // Fallback to classic
                std::string filename = "C:/Users/abdel/OneDrive - uit.ac.ma/Bureau/M1-IAOC/Conception and Programing CPP/ChessMasterUIT-Project/ChessMasterUIT/assets/w_" + type + ".png";
                if (!pieceTextures[index].loadFromFile(filename)) {
                    sf::Image img;
                    img.create(64, 64, sf::Color(200, 200, 200, 128));
                    pieceTextures[index].loadFromImage(img);
                }
            }
            index++;
        }
        
        // Load black themed pieces
        for (const auto& type : pieceTypes) {
            if (!loadThemedPiece(pieceTextures[index], type, "black", theme)) {
                std::cout << "[ChessBoard] Fallback to generating black piece for: " << type << std::endl;
                // Fallback: create from white
                std::string whitePieceName = "w_" + type;
                if (!createBlackPieceFromWhite(pieceTextures[index], whitePieceName)) {
                    sf::Image img;
                    img.create(64, 64, sf::Color(50, 50, 50, 128));
                    pieceTextures[index].loadFromImage(img);
                }
            }
            index++;
        }
        
        texturesLoaded = true;
        std::cout << "[ChessBoard] Themed textures loaded: " << index << " pieces" << std::endl;
        return true;
    }
}

void ChessBoard::initialize(float x, float y, float size, const ThemeColors& theme, PieceSetType pieceSet) {
    std::cout << "[ChessBoard] initialize called with piece set: " << (pieceSet == PieceSetType::CLASSIC ? "CLASSIC" : "THEMED") << std::endl;
    
    // Reload textures if piece set changed
    if (pieceSet != currentPieceSet || !texturesLoaded) {
        BoardTheme currentBoardTheme = BoardThemeManager::getCurrentTheme();
        loadTextures(pieceSet, currentBoardTheme);
    }
    
    // Call original initialize
    initialize(x, y, size, theme);
}

void ChessBoard::setPieceSet(PieceSetType pieceSet, BoardTheme theme) {
    std::cout << "[ChessBoard] Changing piece set to: " << (pieceSet == PieceSetType::CLASSIC ? "CLASSIC" : "THEMED") << std::endl;
    
    if (pieceSet != currentPieceSet) {
        currentPieceSet = pieceSet;
        loadTextures(pieceSet, theme);
        
        // Re-setup all existing pieces with new textures
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                if (pieces[row][col] && !pieces[row][col]->type.empty()) {
                    std::string type = pieces[row][col]->type;
                    std::string color = pieces[row][col]->color;
                    setupPiece(row, col, type, color);
                }
            }
        }
    }
}

// ================================
// Last Move Tracking & Highlighting
// ================================

void ChessBoard::setLastMove(int fromRow, int fromCol, int toRow, int toCol) {
    lastMove = LastMove(fromRow, fromCol, toRow, toCol);
    std::cout << "[ChessBoard] Last move set: (" << fromRow << "," << fromCol 
              << ") -> (" << toRow << "," << toCol << ")" << std::endl;
}

void ChessBoard::drawLastMoveHighlight(sf::RenderWindow& window) {
    if (!lastMove.has_value()) {
        return;
    }
    
    const LastMove& move = lastMove.value();
    
    // Green semi-transparent highlight (as specified: sf::Color(0, 200, 0, 120))
    sf::Color greenHighlight(0, 200, 0, 120);
    
    // Highlight FROM square
    sf::RectangleShape fromHighlight(sf::Vector2f(squareSize, squareSize));
    fromHighlight.setFillColor(greenHighlight);
    fromHighlight.setPosition(
        boardX + move.fromCol * squareSize,
        boardY + move.fromRow * squareSize
    );
    
    // Highlight TO square
    sf::RectangleShape toHighlight(sf::Vector2f(squareSize, squareSize));
    toHighlight.setFillColor(greenHighlight);
    toHighlight.setPosition(
        boardX + move.toCol * squareSize,
        boardY + move.toRow * squareSize
    );
    
    // Draw both highlights
    window.draw(fromHighlight);
    window.draw(toHighlight);
}

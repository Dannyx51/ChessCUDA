#pragma once
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>

#ifndef CHESS_BOARD_H_
#define CHESS_BOARD_H_

enum Piece {
    NONE,
    PAWN = 1,
    KNIGHT = 2,
    BISHOP = 4,
    ROOK = 8,
    QUEEN = 16,
    KING = 32,
    WHITE = 64,
    BLACK = 128,
};

struct Move {
    int start, end;

    bool isValid();

    friend std::ostream& operator<<(std::ostream& os, Move const& move);
    std::string to_string(bool e);
    
    Move(int s, int e) : start(s), end(e) {};
    Move(std::string move);
};

class ChessBoard {
private:
    const int dirOffsets[8] = { 8, -8, 1, -1, 7, -7, 9, -9 };
    int stepsToEdge[64][8];

    void computeStepsToEdge();

public:
    uint8_t board[64] = { 0 };
    bool playerMove = 0; // false is white | true is black
    bool castle[4] = { 0 }; // KQkq

    uint8_t enPassantTarget = 0;

    int halfMove = 0; // since capture or pawn advance
    int fullMove = 1; // self explanatory lol

    /// Constructors ///

    ChessBoard();
    ChessBoard(std::string FEN);


    /// Board Navigation ///
    std::vector<Move> getMoves(int ind);
    void makeMove(Move move);
    
    /// Helper Methods ///
    std::string toFEN();
    uint8_t getLoc(int x, int y);
    friend std::ostream& operator<<(std::ostream& os, ChessBoard const& board);
};

#endif
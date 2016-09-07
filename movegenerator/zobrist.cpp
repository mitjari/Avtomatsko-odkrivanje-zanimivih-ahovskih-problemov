/// RANDOM RKISS is our pseudo random number generator (PRNG) used to compute hash keys.
/// George Marsaglia invented the RNG-Kiss-family in the early 90's. This is a
/// specific version that Heinz van Saanen derived from some public domain code
/// by Bob Jenkins. Following the feature list, as tested by Heinz.
///
/// - Quite platform independent
/// - Passes ALL dieharder tests! Here *nix sys-rand() e.g. fails miserably:-)
/// - ~12 times faster than my *nix sys-rand()
/// - ~4 times faster than SSE2-version of Mersenne twister
/// - Average cycle length: ~2^126
/// - 64 bit seed
/// - Return doubles with a full 53 bit mantissa
/// - Thread safe
///
/// PRNG Inspired by Stockfish GPL source code

#include <iostream>
#include "zobrist.h"
#include "chessboard.h"
#include "rand64.h"

#include "move.h"

Zobrist::Zobrist() {
    Rand64 rand64;

    //Generate random values for all unique states
    for(int square=0; square<64; square++) {
        for(int side=0; side<2; side++) {
            for(int piece=0; piece<7; piece++) {
                Z_PIECES[side][piece][square] = rand64.rnd();
            }
        }
        Z_ENPASSANT[square] = rand64.rnd();
    }

    for(int i=0; i<4; i++) {
        Z_CASTLING[0][i] = rand64.rnd();
        Z_CASTLING[1][i] = rand64.rnd();
    }

    Z_SIDE = rand64.rnd();
}

uint64_t Zobrist::getKey(const ChessBoard &board) const {
    //compute zobrist key for board
    uint64_t zobristKey = 0;

   if(board.nextMove)
        zobristKey ^= Z_SIDE;

    if(board.castling[(int)Color::White])
        zobristKey ^= Z_CASTLING[Color::White][board.castling[Color::White]];

    if(board.castling[Color::Black])
        zobristKey ^= Z_CASTLING[Color::Black][board.castling[Color::Black]];

    if(board.enPassantTargetIndex)
        zobristKey ^= Z_ENPASSANT[board.enPassantTargetIndex];

    for(int color = 0; color <= 1; color ++) {
        for(int piece = 0; piece < 6; piece ++) {
            bitmask pieces = board.pieces[color][piece];
            while(pieces) zobristKey ^= Z_PIECES[color][piece][BitBoard::bitPop(pieces)];
        }
    }
    return zobristKey;
}

/*
  sachista-chess copyright (C) 2014,2015 dusan.saiko@gmail.com

  sachista-chess is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  sachista-chess is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "move.h"
#include "movearray.h"

const bitmask MAGIC_A8H1[] = {
        0x0ULL,
        0x0ULL,
        0x0101010101010100ULL,
        0x0101010101010100ULL,
        0x0101010101010100ULL,
        0x0101010101010100ULL,
        0x0101010101010100ULL,
        0x0101010101010100ULL,
        0x0080808080808080ULL,
        0x0040404040404040ULL,
        0x0020202020202020ULL,
        0x0010101010101010ULL,
        0x0008080808080808ULL,
        0x0ULL,
        0x0ULL
};

const bitmask MAGIC_A1H8[] = {
        0x0ULL,
        0x0ULL,
        0x0101010101010100ULL,
        0x0101010101010100ULL,
        0x0101010101010100ULL,
        0x0101010101010100ULL,
        0x0101010101010100ULL,
        0x0101010101010100ULL,
        0x8080808080808000ULL,
        0x4040404040400000ULL,
        0x2020202020000000ULL,
        0x1010101000000000ULL,
        0x0808080000000000ULL,
        0x0ULL,
        0x0ULL
};

MoveGeneratorBishop::MoveGeneratorBishop() {

    //for all fields
    for (int i = 0; i < 64; i++) {
        //compute index of diagonal for the field
        A8H1_INDEX[i] = BitBoard::fileIndex(i) + BitBoard::rankIndex(i) % 8;
        A1H8_INDEX[i] = BitBoard::fileIndex(i) + 7 - BitBoard::rankIndex(i) % 8;

        //compute 6-bit diagonal for the field
        MOVE_A8H1_MASK[i] = BitMask::A8H1[A8H1_INDEX[i]] & ~BitMask::FRAME;
        MOVE_A1H8_MASK[i] = BitMask::A1H8[A1H8_INDEX[i]] & ~BitMask::FRAME;

        //index magic multiplier for the field
        MOVE_A8H1_MAGIC[i] = MAGIC_A8H1[A8H1_INDEX[i]];
        MOVE_A1H8_MAGIC[i] = MAGIC_A1H8[A1H8_INDEX[i]];
    }


    //precompute A1H8 moves
    // i is field index
    // n is 6 bit configuration

    //for all fields
    for (int i = 0; i < 64; i++) {
        //for all possible diagonal states
        for (int n = 0; n < 64; n++) {

            //get the diagonal
            bitmask diagonal = BitMask::A1H8[A1H8_INDEX[i]];

            //reconstruct the state (number) into the diagonal

            //get the left/bottom bit - start of diagonal
            bitmask tmp;
            while ((tmp = BitBoard::oneSouthWest(diagonal))) {
                diagonal = tmp;
            }

            bitmask board = 0;

            int m = n;
            //traverse diagonal and set bits according to N
            while (diagonal) {
                //shift up by one
                diagonal = BitBoard::oneNorthEast(diagonal);
                if (m & 1) {
                    board |= diagonal;
                }
                m >>= 1;
            }

            //make it 6-bit only
            board &= ~BitMask::FRAME;

            //compute possible moves
            bitmask moves = 0;

            //set piece to Ith index
            bitmask piece = BitBoard::squareBitmask(i);

            //move in one direction
            while (piece) {
                piece = BitBoard::oneNorthEast(piece);
                moves |= piece;

                //end when there is another piece (either color, own color will have to be stripped out)
                if (piece & board) {
                    piece = 0;
                }
            }

            //set piece back to Ith index
            piece = BitBoard::squareBitmask(i);

            //move in the other direction
            while (piece) {
                piece = BitBoard::oneSouthWest(piece);
                moves |= piece;

                //end when there is another piece (either color, own color will have to be stripped out)
                if (piece & board) {
                    piece = 0;
                }
            }

            //remember the moves for Ith field within Nth occupancy state
            MOVE_A1H8_ATTACKS[i][n] = moves;
        }
    }

    //precompute A8H1 moves
    // i is field index
    // n is 6 bit configuration
    //for all fields
    for (int i = 0; i < 64; i++) {
        //for all possible diagonal states
        for (int n = 0; n < 64; n++) {

            //get the diagonal
            bitmask diagonal = BitMask::A8H1[A8H1_INDEX[i]];

            //get the left/top bit - start of the diagonal
            bitmask tmp;
            while ((tmp = BitBoard::oneNorthWest(diagonal))) {
                diagonal = tmp;
            }

            //traverse diagonal and set bits according to N
            bitmask board = 0;

            int m = n;
            while (diagonal) {
                //shift down by one
                diagonal = BitBoard::oneSouthEast(diagonal);
                if (m & 1) {
                    board |= diagonal;
                }
                m >>= 1;
            }

            //make it 6-bit only
            board &= ~BitMask::FRAME;

            //pre-compute moves
            bitmask moves = 0;

            //set the piece to Ith position
            bitmask piece = BitBoard::squareBitmask(i);

            //move one direction
            while (piece) {
                piece = BitBoard::oneNorthWest(piece);
                moves |= piece;
                //end when there is another piece (either color, own color will have to be stripped out)
                if (piece & board) {
                    piece = 0;
                }
            }

            //set the piece back to Ith position
            piece = BitBoard::squareBitmask(i);
            //move the other direction
            while (piece) {
                piece = BitBoard::oneSouthEast(piece);
                moves |= piece;
                //end when there is another piece (either color, own color will have to be stripped out)
                if (piece & board) {
                    piece = 0;
                }
            }
            MOVE_A8H1_ATTACKS[i][n] = moves;
        }
    }
}


inline bitmask MoveGeneratorBishop::onePieceAttacks(const int sourceIndex, const bitmask allPieces) const {
    const int stateIndexA8H1 = (int) (((allPieces & MOVE_A8H1_MASK[sourceIndex]) * MOVE_A8H1_MAGIC[sourceIndex]) >> 57);
    const int stateIndexA1H8 = (int) (((allPieces & MOVE_A1H8_MASK[sourceIndex]) * MOVE_A1H8_MAGIC[sourceIndex]) >> 57);

    //add attacks
    return MOVE_A8H1_ATTACKS[sourceIndex][stateIndexA8H1] | MOVE_A1H8_ATTACKS[sourceIndex][stateIndexA1H8];
}


bitmask MoveGeneratorBishop::generateAttacks(const ChessBoard &board, const Color color, const ChessBoardStats &stats) const
{
    bitmask pieces = board.pieces[color][Bishop] | board.pieces[color][Queen];
    bitmask attacks = 0;

    //for all bishops
    while (pieces) attacks |= onePieceAttacks(BitBoard::bitPop(pieces), stats.allPieces);
    return attacks;
}

void MoveGeneratorBishop::generateMoves(const ChessBoard &board, const ChessBoardStats &stats, MoveArray &moves) const
{
    bitmask bishop = board.pieces[board.nextMove][Bishop];

    Piece movingPiece = Bishop;

    for(int i=0; i<2; i++) {
        //for all bishops
        while (bishop) {
            int fromIndex = BitBoard::bitPop(bishop);

            //get all moves using precomputed values
            bitmask movesBoard = onePieceAttacks(fromIndex, stats.allPieces) & stats.boardAvaliable;

            //for all moves
            while (movesBoard) {
                int toIndex = BitBoard::bitPop(movesBoard);
                bool isCapture = 0 != (BitBoard::squareBitmask(toIndex) & stats.opponentPieces);

                moves.setNext(movingPiece, fromIndex, toIndex, isCapture);
            }
        }

        //switch to queen
        bishop = board.pieces[board.nextMove][Queen];
        movingPiece = Queen;
    }
}

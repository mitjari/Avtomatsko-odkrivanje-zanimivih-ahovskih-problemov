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

const bitmask MAGIC_FILE[] = {
        0x8040201008040200ULL,
        0x4020100804020100ULL,
        0x2010080402010080ULL,
        0x1008040201008040ULL,
        0x0804020100804020ULL,
        0x0402010080402010ULL,
        0x0201008040201008ULL,
        0x0100804020100804ULL
};


MoveGeneratorRook::MoveGeneratorRook() {
    const bitmask FILE_A_MASK = BitMask::A2 | BitMask::A3 | BitMask::A4 | BitMask::A5 | BitMask::A6 | BitMask::A7;

    for (int i = 0; i < 64; i++) {
        //get 6-bit mask for a rank
        MOVE_RANK_MASK[i] = (126ULL) << (BitBoard::rankIndex(i) << 3);

        //compute needed rank shift
        MOVE_RANK_SHIFT[i] = (BitBoard::rankIndex(i) << 3) + 1;

        //get 6-bit mask for a file
        MOVE_FILE_MASK[i] = FILE_A_MASK << BitBoard::fileIndex(i);

        //index magic number directly fo field
        MOVE_FILE_MAGIC[i] = MAGIC_FILE[BitBoard::fileIndex(i)];
    }

    //precompute rank moves
    //for all pieces
    for (int i = 0; i < 64; i++) {
        //for all occupancy states
        for (int n = 0; n < 64; n++) {
            //reconstruct occupancy state
            const bitmask board = MoveGenerator::shiftBitMask((bitmask) n, BitBoard::rankIndex(i), 1);

            //generate available moves
            bitmask moves = 0;

            //set piece in Ith position
            bitmask piece = BitBoard::squareBitmask(i);

            //move in one direction
            while (piece) {
                piece = BitBoard::oneWest(piece);

                moves |= piece;

                //end when there is another piece on the board (either color, own color will have to be stripped out)
                if (piece & board) {
                    piece = 0;
                }
            }

            //set piece back in Ith position
            piece = BitBoard::squareBitmask(i);

            //move in other direction
            while (piece) {
                piece = BitBoard::oneEast(piece);
                moves |= piece;
                //end when there is another piece on the board (either color, own color will have to be stripped out)
                if (piece & board) {
                    piece = 0;
                }
            }

            //remember the moves
            MOVE_RANK_ATTACKS[i][n] = moves;
        }
    }

    //precompute file moves
    //for all pieces
    for (int i = 0; i < 64; i++) {
        //for all occupancy state indexes
        for (int n = 0; n < 64; n++) {
            //reconstuct the occupancy into file
            const bitmask board = MoveGenerator::shiftBitMask(
                    BitBoard::flipDiagA1H8(BitBoard::mirrorHorizontal(MoveGenerator::shiftBitMask((bitmask) n, 0, 1))),
                    0,
                    BitBoard::fileIndex(i));

            //generate moves
            bitmask moves = 0;

            //set piece to original Ith index
            bitmask piece = BitBoard::squareBitmask(i);

            //move piece in one direction
            while (piece) {
                piece = BitBoard::oneNorth(piece);
                moves |= piece;
                //end when there is another piece on the board (either color, own color will have to be stripped out)
                if (piece & board) {
                    piece = 0;
                }
            }

            //set piece back to original Ith index
            piece = BitBoard::squareBitmask(i);

            //move piece in other direction
            while (piece) {
                piece = BitBoard::oneSouth(piece);
                moves |= piece;
                //end when there is another piece on the board (either color, own color will have to be stripped out)
                if (piece & board) {
                    piece = 0;
                }
            }

            //remember file attacks
            MOVE_FILE_ATTACKS[i][n] = moves;
        }
    }

}

inline bitmask MoveGeneratorRook::onePieceAttacks(const int sourceIndex, const bitmask allPieces) const
{
    //use magic multipliers to get occupancy state index
    const int stateIndexRank = (int) ((allPieces & MOVE_RANK_MASK[sourceIndex]) >> MOVE_RANK_SHIFT[sourceIndex]);
    const int stateIndexFile = (int) (((allPieces & MOVE_FILE_MASK[sourceIndex]) * MOVE_FILE_MAGIC[sourceIndex]) >> 57);

    //get possible attacks for field / occupancy state index
    return MOVE_RANK_ATTACKS[sourceIndex][stateIndexRank] | MOVE_FILE_ATTACKS[sourceIndex][stateIndexFile];

}


bitmask MoveGeneratorRook::generateAttacks(const ChessBoard &board, const Color color, const ChessBoardStats &stats) const
{
    bitmask pieces = board.pieces[color][Rook] | board.pieces[color][Queen];
    bitmask attacks = 0;

    //for all rooks
    while (pieces) attacks |= onePieceAttacks(BitBoard::bitPop(pieces), stats.allPieces);

    return attacks;
}

void MoveGeneratorRook::generateMoves(const ChessBoard &board, const ChessBoardStats &stats, MoveArray &moves) const
{
    bitmask rook = board.pieces[board.nextMove][Rook];
    Piece movingPiece = Rook;

    for(int i=0; i<2; i++) {
        //for all rooks
        while (rook) {
            //get next rook
            int fromIndex = BitBoard::bitPop(rook);

            bitmask movesBoard = onePieceAttacks(fromIndex, stats.allPieces) & stats.boardAvaliable;

            //for all moves
            while (movesBoard) {
                int toIndex = BitBoard::bitPop(movesBoard);
                bool isCapture = 0 != (BitBoard::squareBitmask(toIndex) & stats.opponentPieces);
                moves.setNext(movingPiece, fromIndex, toIndex, isCapture);
            }
        }
        //switch to queen
        rook = board.pieces[board.nextMove][Queen];
        movingPiece = Queen;
    }
}

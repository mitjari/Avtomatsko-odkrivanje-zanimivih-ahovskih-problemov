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

MoveGeneratorKnight::MoveGeneratorKnight() {
    //for all fields
    for (int i = 0; i < 64; i++) {
        //put the piece on the board
        bitmask piece = BitBoard::squareBitmask(i);

        //get moves
        KNIGHT_MOVES[i] =
                MoveGenerator::shiftBitMask(piece,  2,  1)       |
                MoveGenerator::shiftBitMask(piece,  2, -1)       |
                MoveGenerator::shiftBitMask(piece,  1,  2)       |
                MoveGenerator::shiftBitMask(piece, -1,  2)       |
                MoveGenerator::shiftBitMask(piece, -2,  1)       |
                MoveGenerator::shiftBitMask(piece, -2, -1)       |
                MoveGenerator::shiftBitMask(piece, -1, -2)       |
                MoveGenerator::shiftBitMask(piece,  1, -2)
        ;
    }
}

bitmask MoveGeneratorKnight::generateAttacks(const ChessBoard &board, const Color color) const
{
    bitmask pieces = board.pieces[color][Knight];
    bitmask attacks = 0;

    // for all knights on the board
    while (pieces) attacks |= KNIGHT_MOVES[BitBoard::bitPop(pieces)];

    return attacks;
}

void MoveGeneratorKnight::generateMoves(const ChessBoard &board, const ChessBoardStats &stats, MoveArray &moves) const
{
    bitmask knight = board.pieces[board.nextMove][Knight];

    // while there are knight pieces
    while (knight) {
        int fromIndex = BitBoard::bitPop(knight);

        // get possible moves - moves minus my onw color
        bitmask movesBoard = KNIGHT_MOVES[fromIndex] & stats.boardAvaliable;
        // for all moves
        while (movesBoard) {
            int toIndex = BitBoard::bitPop(movesBoard);
            bool isCapture = (BitBoard::squareBitmask(toIndex) & stats.opponentPieces) != 0;
            moves.setNext(Knight, fromIndex, toIndex, isCapture);
        }
    }
}

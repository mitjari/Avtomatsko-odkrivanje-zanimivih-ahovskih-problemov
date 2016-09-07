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

MoveGeneratorPawn::MoveGeneratorPawn() {
    //for all fields
    for (int i = 0; i < 64; i++) {
        //put the piece on the board
        bitmask piece = BitBoard::squareBitmask(i);

        PAWN_MOVES[White][i]         = MoveGenerator::shiftBitMask(piece, 1, 0);
        PAWN_DOUBLE_MOVES[White][i]  = MoveGenerator::shiftBitMask(piece, 2, 0);
        PAWN_ATTACKS[White][i]       = MoveGenerator::shiftBitMask(piece, 1, 1) | MoveGenerator::shiftBitMask(piece, 1, -1);
        PAWN_MOVES[Black][i]         = MoveGenerator::shiftBitMask(piece, -1, 0);
        PAWN_DOUBLE_MOVES[Black][i]  = MoveGenerator::shiftBitMask(piece, -2, 0);
        PAWN_ATTACKS[Black][i]       = MoveGenerator::shiftBitMask(piece, -1, 1) | MoveGenerator::shiftBitMask(piece, -1, -1);
    }
}

bitmask MoveGeneratorPawn::generateAttacks(const ChessBoard &board, const Color color) const
{
    if(color == White) {
        return BitBoard::oneNorthEast(board.pieces[color][Pawn]) | BitBoard::oneNorthWest(board.pieces[color][Pawn]);
    } else {
        return BitBoard::oneSouthEast(board.pieces[color][Pawn]) | BitBoard::oneSouthWest(board.pieces[color][Pawn]);
    }
}

void MoveGeneratorPawn::generateMoves(const ChessBoard &board, const ChessBoardStats &stats, MoveArray &moves) const
{
    const bitmask emptyBoard = ~stats.allPieces;

    int whiteBaseRank;
    int blackBaseRank;

    int whitePromotionRank;
    int blackPromotionRank;

    if(board.nextMove == White) {
        whiteBaseRank = 16;
        blackBaseRank = 999;

        whitePromotionRank = 55;
        blackPromotionRank = 0;
    } else {
        whiteBaseRank = 0;
        blackBaseRank = 47;

        whitePromotionRank = 999;
        blackPromotionRank = 8;
    }

    bitmask pawns = board.pieces[board.nextMove][Pawn];

    //while there are pieces
    while (pawns) {
        //get next piece
        const int sourceIndex = BitBoard::bitPop(pawns);

        //get possible moves - moves minus my onw color
        //one step forward
        bitmask movesBoard = PAWN_MOVES[board.nextMove][sourceIndex] & emptyBoard;

        //if one step forward was successful and we are on base rank, try double move
        if(sourceIndex < whiteBaseRank && movesBoard) {
            movesBoard |=  BitBoard::oneNorth(movesBoard) & emptyBoard;
        } else if(sourceIndex > blackBaseRank && movesBoard) {
            movesBoard |=  BitBoard::oneSouth(movesBoard) & emptyBoard;
        }

        //get attacks, only against opponent pieces
        const bitmask attacks = PAWN_ATTACKS[board.nextMove][sourceIndex];
        movesBoard |=  attacks & stats.opponentPieces;

        //for all moves
        while (movesBoard) {
            //get next move
            const int targetIndex = BitBoard::bitPop(movesBoard);
            bool isCapture = (BitBoard::squareBitmask(targetIndex) & stats.opponentPieces) != 0;

            //promotion?
            if (targetIndex > whitePromotionRank || targetIndex < blackPromotionRank) {
                moves.setNext(Pawn, sourceIndex, targetIndex, isCapture, false, Bishop);
                moves.setNext(Pawn, sourceIndex, targetIndex, isCapture, false, Knight);
                moves.setNext(Pawn, sourceIndex, targetIndex, isCapture, false, Queen);
                moves.setNext(Pawn, sourceIndex, targetIndex, isCapture, false, Rook);
            } else {
                //normal move/capture
                moves.setNext(Pawn, sourceIndex, targetIndex, isCapture);
            }
        }

        //check enpassant capture
        if(board.enPassantTargetIndex) {
            movesBoard = attacks & BitBoard::squareBitmask(board.enPassantTargetIndex);
            if (movesBoard) {
                moves.setNext(Pawn, sourceIndex, BitBoard::bitScan(movesBoard), true, true, NoPiece);
            }
        }

    }
}

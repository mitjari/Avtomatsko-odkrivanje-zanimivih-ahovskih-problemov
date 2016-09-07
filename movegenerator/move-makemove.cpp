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

#include <string>
#include "chessboard.h"
#include "move.h"

#include <iostream>

void Move::applyTo(ChessBoard &board) const
{
    const bitmask source = BitBoard::squareBitmask(sourceIndex);
    const bitmask target = BitBoard::squareBitmask(targetIndex);
    const Color opponentColor = board.nextMove == White ? Black : White;

    bitmask *pieces = board.pieces[board.nextMove];

    board.halfMoveClock++;

    //en passant target
    if(board.enPassantTargetIndex) {
        board.zobristKey ^= ChessBoard::zobrist.Z_ENPASSANT[board.enPassantTargetIndex];
        board.enPassantTargetIndex = 0;
    }

    if(board.castling[White])
        board.zobristKey ^= ChessBoard::zobrist.Z_CASTLING[White][board.castling[White]];

    if(board.castling[Black])
        board.zobristKey ^= ChessBoard::zobrist.Z_CASTLING[Black][board.castling[Black]];

    pieces[piece] ^= source | target;
    board.zobristKey ^= ChessBoard::zobrist.Z_PIECES[board.nextMove][piece][sourceIndex] ^ ChessBoard::zobrist.Z_PIECES[board.nextMove][piece][targetIndex];

    //if(piece ==  Knight)  nothing else to do
    //if(piece ==  Bishop) nothing else to do
    //if(piece == Queen) nothing else to do

    if(piece ==  Rook) {
        if(board.castling[board.nextMove]) {
            if(sourceIndex == BoardIndex::A1 && board.nextMove == White)
                board.removeCastling(board.nextMove, QueenSide);
            if(sourceIndex == BoardIndex::H1 && board.nextMove == White)
                board.removeCastling(board.nextMove, KingSide);
            if(sourceIndex == BoardIndex::A8 && board.nextMove == Black)
                board.removeCastling(board.nextMove, QueenSide);
            if(sourceIndex == BoardIndex::H8 && board.nextMove == Black)
                board.removeCastling(board.nextMove, KingSide);
        }
    } else if(piece == King) {
        board.castling[board.nextMove] = None;
        if (sourceIndex == BoardIndex::E1 && board.nextMove == White) {
            //castling
            if (targetIndex == BoardIndex::C1) {
                pieces[ Rook] ^= BitMask::A1 | BitMask::D1;
                board.zobristKey ^= ChessBoard::zobrist.Z_PIECES[board.nextMove][ Rook][BoardIndex::A1] ^ ChessBoard::zobrist.Z_PIECES[board.nextMove][ Rook][BoardIndex::D1];
            } else if(targetIndex == BoardIndex::G1) {
                pieces[ Rook] ^= BitMask::H1 | BitMask::F1;
                board.zobristKey ^= ChessBoard::zobrist.Z_PIECES[board.nextMove][ Rook][BoardIndex::H1] ^ ChessBoard::zobrist.Z_PIECES[board.nextMove][ Rook][BoardIndex::F1];
            }
        }
        if (sourceIndex == BoardIndex::E8 && board.nextMove == Black) {
            //castling
            if (targetIndex == BoardIndex::C8) {
                pieces[ Rook] ^= BitMask::A8 | BitMask::D8;
                board.zobristKey ^= ChessBoard::zobrist.Z_PIECES[board.nextMove][ Rook][BoardIndex::A8] ^ ChessBoard::zobrist.Z_PIECES[board.nextMove][ Rook][BoardIndex::D8];
            } else if(targetIndex == BoardIndex::G8) {
                pieces[ Rook] ^= BitMask::H8 | BitMask::F8;
                board.zobristKey ^= ChessBoard::zobrist.Z_PIECES[board.nextMove][ Rook][BoardIndex::H8] ^ ChessBoard::zobrist.Z_PIECES[board.nextMove][ Rook][BoardIndex::F8];
            }
        }
    } else if(piece ==  Pawn) {
        board.halfMoveClock = 0;

        int step = targetIndex - sourceIndex;
        if (abs(step) > 10) {
            board.enPassantTargetIndex = sourceIndex + (board.nextMove == White ? 8 : -8);
        } else if (promotionPiece != Piece::NoPiece) {
            pieces[Pawn] ^= target;
            board.zobristKey ^= ChessBoard::zobrist.Z_PIECES[board.nextMove][Pawn][targetIndex];
            if (promotionPiece == Queen) {
                pieces[Queen] |= target;
                board.zobristKey ^= ChessBoard::zobrist.Z_PIECES[board.nextMove][Queen][targetIndex];
            } else if (promotionPiece ==  Rook) {
                pieces[Rook] |= target;
                board.zobristKey ^= ChessBoard::zobrist.Z_PIECES[board.nextMove][Rook][targetIndex];
            } else if (promotionPiece ==  Bishop) {
                pieces[Bishop] |= target;
                board.zobristKey ^= ChessBoard::zobrist.Z_PIECES[board.nextMove][Bishop][targetIndex];
            } else if (promotionPiece ==  Knight) {
                pieces[Knight] |= target;
                board.zobristKey ^= ChessBoard::zobrist.Z_PIECES[board.nextMove][Knight][targetIndex];
            }
        }
    }

    //reset halfmoveClock if piece was captured
    if (isCapture) {
        board.halfMoveClock = 0;

        //check capture
        if (isEnPassant) {
            if(board.nextMove == White) {
                board.pieces[opponentColor][Pawn] ^= BitBoard::oneSouth(target);
                board.zobristKey ^= ChessBoard::zobrist.Z_PIECES[opponentColor][Pawn][targetIndex - 8];
            } else {
                board.pieces[opponentColor][Pawn] ^= BitBoard::oneNorth(target);
                board.zobristKey ^= ChessBoard::zobrist.Z_PIECES[opponentColor][Pawn][targetIndex + 8];
            }
        } else if (board.pieces[opponentColor][Bishop] & target) {
            board.pieces[opponentColor][Bishop] ^= target;
            board.zobristKey ^= ChessBoard::zobrist.Z_PIECES[opponentColor][Bishop][targetIndex];
        } else if (board.pieces[opponentColor][Knight] & target) {
            board.pieces[opponentColor][Knight] ^= target;
            board.zobristKey ^= ChessBoard::zobrist.Z_PIECES[opponentColor][Knight][targetIndex];
        } else if (board.pieces[opponentColor][Pawn] & target) {
            board.pieces[opponentColor][Pawn] ^= target;
            board.zobristKey ^= ChessBoard::zobrist.Z_PIECES[opponentColor][Pawn][targetIndex];
        } else if (board.pieces[opponentColor][Queen] & target) {
            board.pieces[opponentColor][Queen] ^= target;
            board.zobristKey ^= ChessBoard::zobrist.Z_PIECES[opponentColor][Queen][targetIndex];
        } else if (board.pieces[opponentColor][ Rook] & target) {
            board.pieces[opponentColor][Rook] ^= target;
            board.zobristKey ^= ChessBoard::zobrist.Z_PIECES[opponentColor][Rook][targetIndex];

            if(board.nextMove == White) {
                if (targetIndex == BoardIndex::A8) {
                    board.removeCastling(Black, QueenSide);
                } else if (targetIndex == BoardIndex::H8) {
                    board.removeCastling(Black, KingSide);
                }
            } else {
                if (targetIndex == BoardIndex::A1) {
                    board.removeCastling(White, QueenSide);
                } else if (targetIndex == BoardIndex::H1) {
                    board.removeCastling(White, KingSide);
                }
            }
        }
    }

    if(board.nextMove == Black)
        board.fullMoveNumber++;

    board.nextMove = opponentColor;
    board.zobristKey ^= ChessBoard::zobrist.Z_SIDE;

    if(board.castling[White])
        board.zobristKey ^= ChessBoard::zobrist.Z_CASTLING[White][board.castling[White]];

    if(board.castling[Black])
        board.zobristKey ^= ChessBoard::zobrist.Z_CASTLING[Black][board.castling[Black]];

    if(board.enPassantTargetIndex)
        board.zobristKey ^= ChessBoard::zobrist.Z_ENPASSANT[board.enPassantTargetIndex];

}

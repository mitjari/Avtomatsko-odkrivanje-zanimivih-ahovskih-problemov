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
#include <iostream>
#include "chessboard.h"

const Zobrist ChessBoard::zobrist;
const std::string ChessBoard::STANDARD_BOARD_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

ChessBoard::ChessBoard()
{
    clearBoard();
}

ChessBoard::ChessBoard(const std::string &fen)
: ChessBoard()
{
    setupFEN(fen);
}

void ChessBoard::setupStandardBoard() {
    setupFEN(STANDARD_BOARD_FEN);
}

bool ChessBoard::operator==(const ChessBoard &other)
{
    return this->zobristKey == other.zobristKey;
}

void ChessBoard::clearBoard() {
    nextMove = White;
    castling[White] = None;
    castling[Black] = None;
    halfMoveClock = 0;
    fullMoveNumber = 1;
    enPassantTargetIndex = 0;
    zobristKey = 0;

    std::fill( &pieces[0][0], &pieces[0][0] + sizeof(pieces) / sizeof(pieces[0][0]), 0 );
}


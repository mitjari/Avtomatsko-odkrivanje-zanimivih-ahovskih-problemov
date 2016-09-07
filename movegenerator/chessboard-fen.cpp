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
#include <regex>
#include <iostream>
#include <sstream>

#include "chessboard.h"
#include "utility.h"

//rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
//8/1K6/1Q6/8/5r2/4rk2/8/8 w - -
void ChessBoard::setupFEN(const std::string &fen_)
{
    clearBoard();
    if(fen_.length() > 128) return;

    std::string fen = Utility::simplified(fen_);

    size_t pos = 0;
    const size_t len = fen.length();

    while(pos < len) {
        const char c = fen[pos];
        if(c == ' ') break;

        if(c == '/') {
            //nothing, next group
        } else if (c >= '0' && c <= '9') {
            const char i = c - '0';

            //output number of empty fields
            for(int color = 0; color <= 1; color ++) {
                for(int piece = King; piece <= Pawn; piece ++) {
                    pieces[color][piece] <<= i;
                }
            }
        } else {
            //output a piece
            pieces[White][King]   = (pieces[White][King]    << 1)   | (c == 'K' ? 1 : 0);
            pieces[White][Queen]  = (pieces[White][Queen]   << 1)   | (c == 'Q' ? 1 : 0);
            pieces[White][Rook]   = (pieces[White][Rook]    << 1)   | (c == 'R' ? 1 : 0);
            pieces[White][Knight] = (pieces[White][Knight]  << 1)   | (c == 'N' ? 1 : 0);
            pieces[White][Bishop] = (pieces[White][Bishop]  << 1)   | (c == 'B' ? 1 : 0);
            pieces[White][Pawn]   = (pieces[White][Pawn]    << 1)   | (c == 'P' ? 1 : 0);

            pieces[Black][King]   = (pieces[Black][King]    << 1)   | (c == 'k' ? 1 : 0);
            pieces[Black][Queen]  = (pieces[Black][Queen]   << 1)   | (c == 'q' ? 1 : 0);
            pieces[Black][Rook]   = (pieces[Black][Rook]    << 1)   | (c == 'r' ? 1 : 0);
            pieces[Black][Knight] = (pieces[Black][Knight]  << 1)   | (c == 'n' ? 1 : 0);
            pieces[Black][Bishop] = (pieces[Black][Bishop]  << 1)   | (c == 'b' ? 1 : 0);
            pieces[Black][Pawn]   = (pieces[Black][Pawn]    << 1)   | (c == 'p' ? 1 : 0);
        }
        pos++;
    }

    //flip rows from string representation to board
    for(int color = White; color <= Black; color ++) {
        for(int piece = King; piece <= Pawn; piece ++) {
            pieces[color][piece] = BitBoard::mirrorHorizontal(pieces[color][piece]);
        }
    }

    pos++; //skip space
    if(pos < len)
        nextMove = fen[pos++] == 'w' ? White : Black;

    pos++; //skip space
    while(pos < len) {
        const char c = fen[pos];
        if(c == ' ') break;

        if(c == 'k') castling[Black] = (Castling) (castling[Black] | KingSide);
        if(c == 'q') castling[Black] = (Castling) (castling[Black] | QueenSide);
        if(c == 'K') castling[White] = (Castling) (castling[White] | KingSide);
        if(c == 'Q') castling[White] = (Castling) (castling[White] | QueenSide);
        pos++;
    }

    pos++; //skip space
    std::string enPassantNotation;
    while(pos < len) {
        char c = fen[pos];
        if(c == ' ') break;

        if(c != '-' && enPassantNotation.length() < 2)
            enPassantNotation += c;
        if(enPassantNotation.length() == 2)
            enPassantTargetIndex = (short) BitBoard::indexFromNotation(enPassantNotation);
        pos++;
    }

    pos++; //skip space
    int n = 0;
    while(pos < len) {
        char c = fen[pos];
        if(c == ' ') break;

        if(c >= '0' && c<='9') {
            n = n * 10 + c - '0';
        }
        pos++;
    }
    if(n) halfMoveClock = n;    //leaves value set by clearBoard()

    pos++; //skip space
    n = 0;
    while(pos < len) {
        char c = fen[pos];
        if(c == ' ') break;

        if(c >= '0' && c<='9') {
            n = n * 10 + c - '0';
        }
        pos++;
    }
    if(n) fullMoveNumber = n; //leaves value set by clearBoard()

    //fix castling / set castling it the castling was not provided in the FEN
    if ((pieces[White][Rook] & BitMask::A1) == 0) removeCastling(White, QueenSide);
    if ((pieces[White][Rook] & BitMask::H1) == 0) removeCastling(White, KingSide);
    if ((pieces[Black][Rook] & BitMask::A8) == 0) removeCastling(Black, QueenSide);
    if ((pieces[Black][Rook] & BitMask::H8) == 0) removeCastling(Black, KingSide);

    //if king is misplaced, remove castling availability
    if ((pieces[White][King] & BitMask::E1) == 0) castling[White] = None;
    if ((pieces[Black][King] & BitMask::E8) == 0) castling[Black] = None;

    updateZobrist();
}

std::string ChessBoard::toFEN()
{
    const std::regex reHeader("a b c d e f g h|\\d| ");

    std::stringstream fen;
    std::string data = std::regex_replace(toString(), reHeader, std::basic_string<char>(), std::regex_constants::match_any);
    data = Utility::simplified(data);

    int emptyCount = 0;
    int lines = 0;

    for(auto piece: data) {
        if (piece == '-') {
            // count empty fields
            emptyCount++;
        } else if (piece == ' ') { //replaced by Utility::simplified
            lines++;

            // output empty fields at the end of rank
            if (emptyCount > 0) fen << emptyCount;
            emptyCount = 0;
            if(lines < 8) fen << '/';
        } else {
            if (emptyCount > 0) fen << emptyCount;
            fen << piece;
            emptyCount = 0;
        }
    }

    if (emptyCount > 0) fen << emptyCount;

    // next move
    fen << ' ' << (nextMove == White ? 'w' : 'b') << ' ';

    // castling
    if (castling[White] & KingSide)                   fen << 'K';
    if (castling[White] & QueenSide)                  fen << 'Q';
    if (castling[Black] & KingSide)                   fen << 'k';
    if (castling[Black] & QueenSide)                  fen << 'q';
    if ((castling[Black] | castling[White]) == 0)     fen << '-';
    fen << ' ';

    // enPassant
    fen << (enPassantTargetIndex ? BitBoard::fieldNotation(enPassantTargetIndex) : "-");
    fen << ' ' << halfMoveClock << ' ' << fullMoveNumber;
    return fen.str();
}

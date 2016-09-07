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
#include "bitboard.h"

bitmask BitBoard::reverseRanks(const bitmask b) {
    //return board with ranks (rows) in reverse order
    return  ((b >> 56ULL)   &  BitMask::RANK_1)                |
            (((b >> 48ULL)  &  BitMask::RANK_1) << 8ULL)       |
            (((b >> 40ULL)  &  BitMask::RANK_1) << 16ULL)      |
            (((b >> 32ULL)  &  BitMask::RANK_1) << 24ULL)      |
            (((b >> 24ULL)  &  BitMask::RANK_1) << 32ULL)      |
            (((b >> 16ULL)  &  BitMask::RANK_1) << 40ULL)      |
            (((b >> 8ULL)   &  BitMask::RANK_1) << 48ULL)      |
            ((b             &  BitMask::RANK_1) << 56ULL);
}

bitmask BitBoard::flipDiagA1H8(const bitmask b_) {
    //Flips around A1H8 diagonal
    static const bitmask k1 = 0x5500550055005500ULL;
    static const bitmask k2 = 0x3333000033330000ULL;
    static const bitmask k4 = 0x0f0f0f0f00000000ULL;

    bitmask b = b_;

    bitmask t = k4 & (b ^ (b << 28));
    b ^= t ^ (t >> 28);
    t = k2 & (b ^ (b << 14));
    b ^= t ^ (t >> 14);
    t = k1 & (b ^ (b << 7));
    b ^= t ^ (t >> 7);
    return b;
}

bitmask BitBoard::mirrorHorizontal(const bitmask b) {
    //mirrors the bitboard horizontally
    static const bitmask k1 = 0x5555555555555555ULL;
    static const bitmask k2 = 0x3333333333333333ULL;
    static const bitmask k4 = 0x0f0f0f0f0f0f0f0fULL;

    bitmask r = b;
    r = ((r >> 1) & k1) | ((r & k1) << 1);
    r = ((r >> 2) & k2) | ((r & k2) << 2);
    r = ((r >> 4) & k4) | ((r & k4) << 4);
    return r;
}

bitmask BitBoard::bitmaskFromNotation(const std::string &notation)
{
    return squareBitmask(indexFromNotation(notation));
}

int BitBoard::indexFromNotation(const std::string &notation)
{
    if(notation.length() != 2) return 0;
    int index = ((notation[0] - 'a') + ((notation[1] - '1') << 3));
    return index;
}

std::string BitBoard::fieldNotation(const int index) {
    std::string notation;

    notation += 'a' + (index % 8);
    notation += '1' + (index / 8);
    return notation;
}

std::string BitBoard::toString(const bitmask b) {
    static const std::string header =  "  a b c d e f g h\n";

    std::string board;
    bitmask reversedBoard = reverseRanks(b);

    board += header;

    for (int i = 0; i < 64; i++) {
        if ((i % 8) == 0) {
            if (i > 0) {
                //print right column digit
                board += '0' + 9 - (i / 8);
                board += '\n';
            }
            //print left column digit
            board += '0' + 8 - (i / 8);
            board += ' ';
        }

        if(reversedBoard & (1ULL << i)) {
            board += "x ";
        } else {
            board += "- ";
        }
    }

    board += "1\n"; //last right column digit
    board += header; //footer

    return board;
}



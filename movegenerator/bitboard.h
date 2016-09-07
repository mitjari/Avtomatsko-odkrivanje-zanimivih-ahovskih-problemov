#pragma once
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

#include <stdint.h>
#include <string>

typedef uint64_t bitmask;

namespace BitMask {
    const   bitmask    A1 = 1ULL;           const   bitmask    B1 = (1ULL << 1);    const   bitmask    C1 = (1ULL << 2);    const   bitmask    D1 = (1ULL << 3);    const   bitmask    E1 = (1ULL << 4);    const   bitmask    F1 = (1ULL << 5);    const   bitmask    G1 = (1ULL << 6);    const   bitmask    H1 = (1ULL << 7);
    const   bitmask    A2 = (1ULL << 8);    const   bitmask    B2 = (1ULL << 9);    const   bitmask    C2 = (1ULL << 10);   const   bitmask    D2 = (1ULL << 11);   const   bitmask    E2 = (1ULL << 12);   const   bitmask    F2 = (1ULL << 13);   const   bitmask    G2 = (1ULL << 14);   const   bitmask    H2 = (1ULL << 15);
    const   bitmask    A3 = (1ULL << 16);   const   bitmask    B3 = (1ULL << 17);   const   bitmask    C3 = (1ULL << 18);   const   bitmask    D3 = (1ULL << 19);   const   bitmask    E3 = (1ULL << 20);   const   bitmask    F3 = (1ULL << 21);   const   bitmask    G3 = (1ULL << 22);   const   bitmask    H3 = (1ULL << 23);
    const   bitmask    A4 = (1ULL << 24);   const   bitmask    B4 = (1ULL << 25);   const   bitmask    C4 = (1ULL << 26);   const   bitmask    D4 = (1ULL << 27);   const   bitmask    E4 = (1ULL << 28);   const   bitmask    F4 = (1ULL << 29);   const   bitmask    G4 = (1ULL << 30);   const   bitmask    H4 = (1ULL << 31);
    const   bitmask    A5 = (1ULL << 32);   const   bitmask    B5 = (1ULL << 33);   const   bitmask    C5 = (1ULL << 34);   const   bitmask    D5 = (1ULL << 35);   const   bitmask    E5 = (1ULL << 36);   const   bitmask    F5 = (1ULL << 37);   const   bitmask    G5 = (1ULL << 38);   const   bitmask    H5 = (1ULL << 39);
    const   bitmask    A6 = (1ULL << 40);   const   bitmask    B6 = (1ULL << 41);   const   bitmask    C6 = (1ULL << 42);   const   bitmask    D6 = (1ULL << 43);   const   bitmask    E6 = (1ULL << 44);   const   bitmask    F6 = (1ULL << 45);   const   bitmask    G6 = (1ULL << 46);   const   bitmask    H6 = (1ULL << 47);
    const   bitmask    A7 = (1ULL << 48);   const   bitmask    B7 = (1ULL << 49);   const   bitmask    C7 = (1ULL << 50);   const   bitmask    D7 = (1ULL << 51);   const   bitmask    E7 = (1ULL << 52);   const   bitmask    F7 = (1ULL << 53);   const   bitmask    G7 = (1ULL << 54);   const   bitmask    H7 = (1ULL << 55);
    const   bitmask    A8 = (1ULL << 56);   const   bitmask    B8 = (1ULL << 57);   const   bitmask    C8 = (1ULL << 58);   const   bitmask    D8 = (1ULL << 59);   const   bitmask    E8 = (1ULL << 60);   const   bitmask    F8 = (1ULL << 61);   const   bitmask    G8 = (1ULL << 62);   const   bitmask    H8 = (1ULL << 63);

    const bitmask      FILE_A      = A1 | A2 | A3 | A4 | A5 | A6 | A7 | A8;
    const bitmask      FILE_H      = H1 | H2 | H3 | H4 | H5 | H6 | H7 | H8;
    const bitmask      NOT_FILE_A  = (~FILE_A);
    const bitmask      NOT_FILE_H  = (~FILE_H);

    const bitmask      RANK_1      = A1 | B1 | C1 | D1 | E1 | F1 | G1 | H1;
    const bitmask      RANK_8      = A8 | B8 | C8 | D8 | E8 | F8 | G8 | H8;

    const bitmask      FRAME       = RANK_1 | RANK_8 | FILE_A | FILE_H;
    const bitmask      UNIVERSE    = (~0ULL);

    //pregenerated constants
    const   bitmask     RANK[8]   = {255ULL, 65280ULL, 16711680ULL, 4278190080ULL, 1095216660480ULL, 280375465082880ULL, 71776119061217280ULL, 18374686479671623680ULL};
    const   bitmask     FILE[8]   = {72340172838076673ULL, 144680345676153346ULL, 289360691352306692ULL, 578721382704613384ULL, 1157442765409226768ULL, 2314885530818453536ULL, 4629771061636907072ULL, 9259542123273814144ULL};
    const   bitmask     A1H8[15]  = {72057594037927936ULL, 144396663052566528ULL, 288794425616760832ULL, 577588855528488960ULL, 1155177711073755136ULL, 2310355422147575808ULL, 4620710844295151872ULL, 9241421688590303745ULL, 36099303471055874ULL, 141012904183812ULL, 550831656968ULL, 2151686160ULL, 8405024ULL, 32832ULL, 128ULL};
    const   bitmask     A8H1[15]  = {1ULL, 258ULL, 66052ULL, 16909320ULL, 4328785936ULL, 1108169199648ULL, 283691315109952ULL, 72624976668147840ULL, 145249953336295424ULL, 290499906672525312ULL, 580999813328273408ULL, 1161999622361579520ULL, 2323998145211531264ULL, 4647714815446351872ULL, 9223372036854775808ULL};
}

namespace BoardIndex {
    const int A1 = 0;    const int B1 = 1;    const int C1 = 2;    const int D1 = 3;    const int E1 = 4;    const int F1 = 5;    const int G1 = 6;    const int H1 = 7;
    const int A2 = 8;    const int B2 = 9;    const int C2 = 10;   const int D2 = 11;   const int E2 = 12;   const int F2 = 13;   const int G2 = 14;   const int H2 = 15;
    const int A3 = 16;   const int B3 = 17;   const int C3 = 18;   const int D3 = 19;   const int E3 = 20;   const int F3 = 21;   const int G3 = 22;   const int H3 = 23;
    const int A4 = 24;   const int B4 = 25;   const int C4 = 26;   const int D4 = 27;   const int E4 = 28;   const int F4 = 29;   const int G4 = 30;   const int H4 = 31;
    const int A5 = 32;   const int B5 = 33;   const int C5 = 34;   const int D5 = 35;   const int E5 = 36;   const int F5 = 37;   const int G5 = 38;   const int H5 = 39;
    const int A6 = 40;   const int B6 = 41;   const int C6 = 42;   const int D6 = 43;   const int E6 = 44;   const int F6 = 45;   const int G6 = 46;   const int H6 = 47;
    const int A7 = 48;   const int B7 = 49;   const int C7 = 50;   const int D7 = 51;   const int E7 = 52;   const int F7 = 53;   const int G7 = 54;   const int H7 = 55;
    const int A8 = 56;   const int B8 = 57;   const int C8 = 58;   const int D8 = 59;   const int E8 = 60;   const int F8 = 61;   const int G8 = 62;   const int H8 = 63;
}

namespace BitBoard {
    inline  bitmask     oneEast             (const bitmask b)                   { return ((b << 1) & BitMask::NOT_FILE_A); }
    inline  bitmask     oneNorthEast        (const bitmask b)                   { return ((b << 9) & BitMask::NOT_FILE_A); }
    inline  bitmask     oneSouthEast        (const bitmask b)                   { return ((b >> 7) & BitMask::NOT_FILE_A); }

    inline  bitmask     oneWest             (const bitmask b)                   { return ((b >> 1) & BitMask::NOT_FILE_H); }
    inline  bitmask     oneSouthWest        (const bitmask b)                   { return ((b >> 9) & BitMask::NOT_FILE_H); }
    inline  bitmask     oneNorthWest        (const bitmask b)                   { return ((b << 7) & BitMask::NOT_FILE_H); }

    inline  bitmask     oneNorth            (const bitmask b)                   { return (b << 8); }
    inline  bitmask     oneSouth            (const bitmask b)                   { return (b >> 8); }

    inline  int         fileIndex           (const int index)                   { return index % 8; }
    inline  int         rankIndex           (const int index)                   { return index / 8; }
    inline  bitmask     squareBitmask       (const int index)                   { return (1ULL << index); }

            bitmask     reverseRanks        (const bitmask b);
            bitmask     flipDiagA1H8        (const bitmask b);
            bitmask     mirrorHorizontal    (const bitmask b);

            bitmask     bitmaskFromNotation (const std::string &notation);
            int         indexFromNotation   (const std::string &notation);
            std::string fieldNotation       (const int index);
            std::string toString            (const bitmask b);

    inline  int         bitScan             (bitmask b) {
    #if defined(__x86_64__)
            __asm__("bsfq %1, %0" : "=r" (b) : "rm" (b));
            return (int) b;
    #elif defined(_MSC_VER) && defined(_M_X64)
            unsigned long idx;
            _BitScanForward64(&idx, b);
            return idx;
    #else
            /**
             * bitScanForward
             * @author Matt Taylor (2003)
             * @param bb bitboard to scan
             * @precondition bb != 0
             * @return index (0..63) of least significant one bit
             */
            static const int lsb_64_table[64] =
            {
               63, 30,  3, 32, 59, 14, 11, 33,
               60, 24, 50,  9, 55, 19, 21, 34,
               61, 29,  2, 53, 51, 23, 41, 18,
               56, 28,  1, 43, 46, 27,  0, 35,
               62, 31, 58,  4,  5, 49, 54,  6,
               15, 52, 12, 40,  7, 42, 45, 16,
               25, 57, 48, 13, 10, 39,  8, 44,
               20, 47, 38, 22, 17, 37, 36, 26
            };

           unsigned int folded;
           b ^= b - 1;
           folded = (int) b ^ (b >> 32);
           return lsb_64_table[folded * 0x78291ACF >> 26];
    #endif
    }

    inline  int         bitPop              (bitmask &b) {
        const int i = bitScan(b);
        b  &= b - 1;
        return i;
    }
    inline  int         popCount            (bitmask b) {
    #if defined(__x86_64__)
        return __builtin_popcountll(b);
    #elif defined(_MSC_VER) && defined(_M_X64)
        return (int) _mm_popcnt_u64(b);
    #else
        //source: http://chessprogramming.wikispaces.com/Population+Count
        static const uint64_t k1 = 0x5555555555555555ULL; /*  -1/3   */
        static const uint64_t k2 = 0x3333333333333333ULL; /*  -1/5   */
        static const uint64_t k4 = 0x0f0f0f0f0f0f0f0fULL; /*  -1/17  */

        b =  b       - ((b >> 1)  & k1); /* put count of each 2 bits into those 2 bits */
        b = (b & k2) + ((b >> 2)  & k2); /* put count of each 4 bits into those 4 bits */
        b = (b       +  (b >> 4)) & k4 ; /* put count of each 8 bits into those 8 bits */
        b += b >>  8;  /* put count of each 16 bits into their lowest 8 bits */
        b += b >> 16;  /* put count of each 32 bits into their lowest 8 bits */
        b += b >> 32;  /* put count of the final 64 bits into the lowest 8 bits */
        return (int) b & 255;
    #endif
    }
}

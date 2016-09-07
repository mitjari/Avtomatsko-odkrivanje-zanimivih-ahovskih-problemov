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

#include "move.h"

//https://chessprogramming.wikispaces.com/Chess+Position
static int const MAX_MOVES=220;


class MoveArray
{
public:
    MoveArray(): _size(0) {}


    inline void setNext(Piece movingPiece, int fromIndex, int toIndex, bool isCapture) {
        if(_size >= MAX_MOVES) return;

        Move &m = data[_size++];
        m.piece = movingPiece;
        m.sourceIndex = fromIndex;
        m.targetIndex = toIndex;
        m.isCapture = isCapture;
        m.promotionPiece = Piece::NoPiece;
        m.isEnPassant = false;
    }

    inline void setNext(Piece piece, int fromIndex, int toIndex, bool isCapture, bool isEnPassant, Piece promotionPiece)
    {
        if(_size >= MAX_MOVES) return;

        Move &m = data[_size++];
        m.piece = piece;
        m.sourceIndex = fromIndex;
        m.targetIndex = toIndex;
        m.isCapture = isCapture;
        m.promotionPiece = promotionPiece;
        m.isEnPassant = isEnPassant;
    }

    inline void         clear()     { _size=0; }
    inline int          size()      { return _size; }

    inline Move         &first()    { return data[0]; }

            int         _size;
            Move        data[MAX_MOVES];
};

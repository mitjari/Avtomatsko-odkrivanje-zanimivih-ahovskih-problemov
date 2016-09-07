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
#include "rand64.h"

Rand64::Rand64() {
    int seed = 113;

    a = 0xF1EA5EED;
    b = c = d = 0xD4E12C77;
    for (int i = 0; i < seed; ++i)
        rnd();
}

Rand64::~Rand64() {
}

uint64_t Rand64::rnd() {
    /* alternative:
        uint64_t  num = rand();
        num = (num << 32) | rand();
        return num;
    */

    const uint64_t e = a - rotate(b,  7);
    a = b ^ rotate(c, 13);
    b = c + rotate(d, 37);
    c = d + e;
    return d = e + a;
}

inline uint64_t Rand64::rotate(const uint64_t x, const uint64_t k)
{
    return (x << k) | (x >> (64 - k));
}
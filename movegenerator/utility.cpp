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

#include <chrono>
#include <thread>

#include "utility.h"


inline bool isSpace(const char c) {
    switch(c) {
        case ' ':
        case '\t':
        case '\n':
        case '\v':
        case '\f':
        case '\r':
                    return true;
        default:
                    return false;
    }
}

std::string Utility::simplified(const std::string &str)
{
    std::string buffer;
    int spaces = 0;

    for(auto c: str) {
        if(isSpace(c)) {
            spaces ++;
        } else {
            if(spaces && buffer.length() > 0)
            {
                //output one space
                buffer += ' ';
            }
            buffer += c;
            spaces = 0;
        }
    }
    return buffer;
}

/*
* Author:  David Robert Nadeau
* Site:    http://NadeauSoftware.com/
*/


StopWatch::StopWatch()
:
    start(std::chrono::steady_clock::now())
{
}

uint64_t StopWatch::elapsedTimeInMilliseconds()
{
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

void StopWatch::sleep(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}


uint64_t StopWatch::speedInSeconds(uint64_t count)
{
    uint64_t time = elapsedTimeInMilliseconds();

    return (count * 1000) / time;
}

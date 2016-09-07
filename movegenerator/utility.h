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

#include <string>
#include <chrono>

namespace Utility {
    //trims the string and replaces all multi spaces (including '\t', '\n', '\v', '\f', '\r', and ' ') by one space
    std::string simplified(const std::string &str);
    uint64_t getMemorySize();
}

class StopWatch {
public:
   StopWatch();

   uint64_t elapsedTimeInMilliseconds();
   uint64_t speedInSeconds(uint64_t count);

   static void sleep(int milliseconds);

private:
    const std::chrono::steady_clock::time_point start;
};

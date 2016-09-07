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

#define IMPLEMENTATION_VERSION  "0.2.1"
#define IMPLEMENTATION_DATE     __DATE__ " " __TIME__


#if defined(__i386__) || defined (_M_IX86)
  #define ARCHITECTURE    "x86"
#elif defined(__x86_64__) || defined(_M_X64)
  #define ARCHITECTURE    "x64"
#else
  #define ARCHITECTURE    "Unknown Architecture"
#endif

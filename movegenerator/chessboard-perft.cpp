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

#include <string.h>
#include <vector>
#include <iostream>
#include <thread>
#include <atomic>
#include <new>

#include "chessboard.h"
#include "zobrist.h"
#include "utility.h"
#include "move.h"
#include "movearray.h"


struct CacheEntry {
    uint64_t              hash;
    uint64_t              count;
    int                   depth;
};

struct Cache {
    int                       cacheSize;
    CacheEntry                *data;

    Cache(int size) {

        cacheSize = size;
        bool badAlloc = true;
        while(badAlloc) {
            badAlloc = false;
            try
            {
                data = new CacheEntry[cacheSize];
            }
            catch (std::bad_alloc& )
            {
                badAlloc = true;
                cacheSize = ((cacheSize * 2) / 3);
            }
        }
    }

    ~Cache() {
        delete[] data;
    }

    uint64_t get(const uint64_t &hash, int depth) {
        CacheEntry *entry = (data + ((cacheSize - 1) & hash));

        if(entry->hash == hash && entry->depth == depth) {
            return entry->count;
        }
        return 0;
    }

    void set(uint64_t hash, int depth, uint64_t count) {
        CacheEntry *entry = (data + ((cacheSize - 1) & hash));
        entry->hash = hash;
        entry->depth = depth;
        entry->count = count;
    }
};


/**
 * @brief This is a perft worker
 * @param board
 * @param depth
 * @param cache Shared cache, needs OMP synchronization
 * @return number of legal moves
 */
uint64_t minimax(Cache &cache, const ChessBoard &board, const int depth, const ChessBoardStats &stats)
{
    uint64_t count = cache.get(board.zobristKey, depth);
    if(count) return count;

    MoveArray moves;
    MoveGenerator::moves(board, stats, moves);

    bitmask attacks = MoveGenerator::attacks(board, stats.opponentColor, stats);
    bool    isCheck = 0 != (attacks & stats.king);

    for(int i=0; i < moves.size(); i++) {
        Move &move = moves.data[i];
        bitmask piece = BitBoard::squareBitmask(move.sourceIndex);

        if(depth == 1) {
            //we only need to validate the board in following cases
            if((piece & stats.king) || (isCheck) || (piece & attacks) || move.isEnPassant) {
                ChessBoard nextBoard = board;
                move.applyTo(nextBoard);

                ChessBoardStats nextStats(nextBoard);
                if(MoveGenerator::isOpponentsKingNotUnderCheck(nextBoard, nextStats)) {
                    count += 1;
                }
            } else {
                count += 1;
            }
        } else {
            ChessBoard nextBoard = board;
            move.applyTo(nextBoard);

            ChessBoardStats nextStats(nextBoard);
            if((piece & stats.king) || (isCheck) || (piece & attacks) || move.isEnPassant) {
                if(MoveGenerator::isOpponentsKingNotUnderCheck(nextBoard, nextStats)) {
                    count += minimax(cache, nextBoard, depth -1, nextStats);
                }
            } else {
                count += minimax(cache, nextBoard, depth -1, nextStats);
            }
        }
    }
    cache.set(board.zobristKey, depth, count);
    return count;
}

uint64_t ChessBoard::perft(int depth) const
{
    if(depth < 1) return 0;

    if(depth == 1) {
        Cache cache(1024*1024);
        return minimax(cache, *this, depth, ChessBoardStats(*this));
    }

    std::vector<std::thread> threads;
    std::atomic<uint64_t> count(0);

    MoveArray moves;
    MoveGenerator::moves(*this, ChessBoardStats(*this), moves);

    #if defined(__x86_64__) || defined(_M_X64)
    int cacheSize = 4*1024*1024;
    if(depth > 7) {
        cacheSize = 16*1024*1024;
    }
    #else
    int cacheSize = 4*1024*1024;
    #endif


    for(int i=0; i<moves.size(); i++) {
        Move &m = moves.data[i];

        ChessBoard board = *this;
        m.applyTo(board);
        if(MoveGenerator::isOpponentsKingNotUnderCheck(board, ChessBoardStats(board))) {
            threads.push_back(std::thread([board, depth, &count, cacheSize](){
                Cache cache(cacheSize);
                count += minimax(cache, board, depth -1, ChessBoardStats(board));
            }));
        }
    }

    //join threads
    for(auto& thread : threads){
           thread.join();
    }

    return count.load();
}










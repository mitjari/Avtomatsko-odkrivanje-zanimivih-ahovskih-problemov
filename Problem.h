#ifndef PROBLEM_H_INCLUDED
#define PROBLEM_H_INCLUDED

#include <string>
#include <sstream>

#include "movegenerator/chessboard.h"
#include "movegenerator/move.h"

class Problem
{
public:
	Problem(std::string, std::string);
	std::string toString() const;
	std::string getLog() const;
    bool allOk() const;

	int problem;
	int heuristika;
	int stPoteze;
	int stPolpoteze;
	int trnspositionTableSize;
	Color winner;
	Color looser;
    ChessBoard start;
    ChessBoard end;

    bool isciVse;

    Move startmove;

    int fixedMoveNumber;
    Move fixedmove;

    Move endmove;
    bool promotionSensitive;

    bool captureSensitive;
    bitmask activeFields;
    Piece capturedPiece;

    std::string solution;
    std::string solutionPgn;

    std::string outfile;


private:
	void parseProblem(std::string);
	void parseH(std::string);
	void parseNumMoves(std::string);
	void parseStartPos(std::string);
	void parseEndPos(std::string);
	void parseStartMove(std::string);
	void parseFixedMove(std::string);
	void parseEndMove(std::string);
	void parseActiveFields(std::string);
	void parseTranspositionTableSize(std::string);
	void parseCapture(std::string);
	void parseEndpiece(std::string);
	void parseWinColor(std::string);
	void parseCapturedPiece(std::string);
	void parseFixedMoveNumber(std::string);
	void parseFmovepiece(std::string);
	void parsePromotionPiece(std::string);
	void parseSearchAll(std::string);

	void log( std::string );

	std::string logstr;
	bool allok;
};

#endif // PROBLEM_H_INCLUDED

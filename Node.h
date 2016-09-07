#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include<list>
#include <fstream>
#include <iostream>

#include "movegenerator/move.h"
#include "movegenerator/chessboard.h"

#include"Problem.h"

class Node
{
public:
	Node();
	Node(Node* parrent, Move&);

	short getG() const;
	short getF() const;
	Node* getParrent();
	Move getMove() const;
	bitmask getActiveFields() const;

	ChessBoard board() const;
	ChessBoard legalMoves(MoveArray&);
	bool isMate() const;
	int isGoal() const;

	bool operator==(const Node&) const;

	static Problem problem;

	std::string path() const;
	std::string pathPgn() const;

private:
	short fCover() const;
	short fManhattan() const;
	short fProblemDva() const;
	short fPozicija() const;

	short manhattanDistance(int, int) const;


	Move move;
	Node* parrent;
	bitmask activeFields;
	short g;	//Znana pot (globina)
	short f;	//Izracunana ocena (znana pot? + heuristika)
	//h 		-> heuristika (ocena do cilja)
};

struct PointerNodeCompare
{
  bool operator()(const Node* lhs, const Node* rhs) const  { return  lhs->getF() < rhs->getF(); }
};

#endif // NODE_H_INCLUDED

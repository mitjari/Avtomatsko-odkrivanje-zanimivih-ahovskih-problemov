#include <iostream>
#include <sstream>

#include "Node.h"
#include "movegenerator/movearray.h"

Node::Node()
{
	this->parrent= NULL;
	this->g= 0;
	this->f= 0;
	this->move= Move();
	activeFields= problem.activeFields;
}

Node::Node(Node* parrent, Move& mv)
{
	this->parrent= parrent;
	this->g= parrent->getG()+1;
	this->move= mv;
	//Vrednosti ocenitvene funkcije
	if(problem.heuristika == 1) this->f= fCover();
	else if (problem.heuristika == 2 ) this->f=fManhattan();
	else if (problem.heuristika == 3 ) this->f=fProblemDva();
	else if (problem.heuristika == 4 ) this->f=fPozicija();
	else this->f=this->g;	//Brez heuristike

	//Popravi aktivna polja oziroma  figure
	bitmask sourceInv= ~BitBoard::squareBitmask( move.sourceIndex );
	bitmask dest= BitBoard::squareBitmask( move.targetIndex );

	//bitwise OR, AND
	this->activeFields= (parrent->activeFields & sourceInv) |dest;
}

short Node::getG() const
{
	return g;
}

short Node::getF() const
{
	return f;
}

Node* Node::getParrent()
{
	return parrent;
}

Move Node::getMove() const
{
	return move;
}

bitmask Node::getActiveFields() const
{
	return activeFields;
}

Problem Node::problem("", "");

ChessBoard Node::board() const
{
	if( parrent == NULL ) return problem.start;

	ChessBoard b= parrent->board();
	move.applyTo(b);
	return b;
}

ChessBoard Node::legalMoves(MoveArray& premiki_par)
{
	MoveArray premiki;
	MoveGenerator gen;
	ChessBoard cb= board();
    gen.moves(cb, ChessBoardStats(cb), premiki);

	//Preveri ali so generirani psevdo legalni premiki tudi legalni
    for( int i(0); i<premiki._size; i++ )
    {
    	bool legal= true;

    	if( !(activeFields & BitBoard::squareBitmask( premiki.data[i].sourceIndex) ) ) legal= false;
    	else
		{
			ChessBoard nextBoard= cb;
			premiki.data[i].applyTo(nextBoard);
			ChessBoardStats nextStats(nextBoard);

			if( !MoveGenerator::isOpponentsKingNotUnderCheck(nextBoard, nextStats) ) legal= false;
		}

		if(legal) premiki_par.setNext(premiki.data[i].piece, premiki.data[i].sourceIndex, premiki.data[i].targetIndex, premiki.data[i].isCapture, premiki.data[i].isEnPassant, premiki.data[i].promotionPiece);

    }
    return cb;
}

short Node::fCover() const
{
	short ocena(0);

	ChessBoard cb= board();
	bitmask kingMask= BitBoard::squareBitmask( BitBoard::bitScan(  cb.pieces[problem.looser][King] ) );

	int matniKvadrat[8];
	matniKvadrat[0]=BitBoard::oneNorthWest(kingMask); matniKvadrat[1]=BitBoard::oneNorth(kingMask); matniKvadrat[2]=BitBoard::oneNorthEast(kingMask);
	matniKvadrat[3]=BitBoard::oneWest(kingMask); matniKvadrat[4]=BitBoard::oneEast(kingMask);
	matniKvadrat[5]=BitBoard::oneSouthWest(kingMask); matniKvadrat[6]=BitBoard::oneSouth(kingMask); matniKvadrat[7]=BitBoard::oneSouthEast(kingMask);

	MoveGenerator gen;
    for ( int i(0); i < 8; i++)
	{
        if( matniKvadrat[i] != 0 && gen.isBitMaskUnderAttack(cb, problem.winner, ChessBoardStats(cb), matniKvadrat[i]) ) ocena++;
	}
	return 8-ocena;
}

short Node::fManhattan() const
{
	short ocena(0);

	ChessBoard cb= board();
	bitmask figure= cb.allWithoutPawnsAndKing(problem.winner);
	bitmask kingSquare= cb.pieces[problem.looser][King];

	int figura(0);
	while( 0 < BitBoard::popCount(figure) )
	{
		figura= BitBoard::bitPop(figure);
        ocena+= abs( BitBoard::fileIndex(figura) - BitBoard::fileIndex(kingSquare) );
        ocena+= abs( BitBoard::rankIndex(figura) - BitBoard::rankIndex(kingSquare) );
	}
    return ocena;
}

short Node::fProblemDva() const
{

	//short ocenaCover= fCover();
	short ocenaManhattan(0);

	//Izracunaj oddaljenost kralja od koncnega premika
	ChessBoard cb= board();

	int poljeKralj= BitBoard::bitScan(  cb.pieces[problem.winner][King] );
	int stolpecKralj= BitBoard::rankIndex(poljeKralj);
	int vrsticaKralj= BitBoard::fileIndex(poljeKralj);

	int stolpecKonec= BitBoard::rankIndex(problem.endmove.sourceIndex);
	int vrsticaKonec= BitBoard::fileIndex(problem.endmove.sourceIndex);

	ocenaManhattan= abs(stolpecKralj - stolpecKonec) + abs(vrsticaKralj - vrsticaKonec);

	//int ocena= (int) 8 * ocenaManhattan / problem.stPolpoteze;
	//ocena+= (8 - ocenaCover );

	return ocenaManhattan;
}

short Node::fPozicija() const
{
    //Za vsako figuro preveri ali se nahaja na sahovnici na zacetku in na koncu
    short score(0);
    ChessBoard cb= board();

    //Zanka za obe barvi 0=beli, 1=crni
    for( int i(0); i <= 1; i++ )
	{
		//King=0, Queen=1, Bishop=2, Knight=3,Rook=4
        for( int j(0); j <= 5; j++ )
		{
			bitmask figureTrenutno= cb.pieces[i][j];
			bitmask figureKonec= problem.end.pieces[i][j];

            score+= manhattanDistance(BitBoard::bitPop( figureTrenutno ), BitBoard::bitPop( figureKonec ));

            //Se za drugo figuro
            if( j == Rook|| j == Knight || j == Bishop ) score+= manhattanDistance(BitBoard::bitPop( figureTrenutno ), BitBoard::bitPop( figureKonec ));
		}
	}
	return score;
}

short Node::manhattanDistance(int x, int y) const
{
	//Kazen v primeru da figura X obstaja Y pa ne ali obratno
	if ((x < 0 && 0 <= y) || (y < 0 && 0 <= x)) return 10;

	int stolpecX= BitBoard::rankIndex(x);
	int vrsticaX= BitBoard::fileIndex(x);

	int stolpecY= BitBoard::rankIndex(y);
	int vrsticaY= BitBoard::fileIndex(y);

	return (abs(stolpecX - stolpecY) + abs(vrsticaX - vrsticaY));
}

std::string Node::path() const
{
	if( parrent == NULL ) return "";

	std::stringstream ss;
	ss << parrent->path();

	if( g%2 == 1) ss << ( g+1)/2 << ".\t";
	ss << move.toString(true) << "\t";
	if( g%2 == 0 ) ss << std::endl;

	return ss.str();

}

std::string Node::pathPgn() const
{
	if( parrent == NULL ) return "";

	std::stringstream ss;

	ss << parrent->pathPgn();
	if( g%2 == 1) ss << ( g+1)/2 << ". ";
	ss << move.toString(false) << " ";

	return ss.str();
}

int Node::isGoal() const
{
	//-1	Presegli smo iskano globino, v tej veji ni resitve
	//0->	Nismo na pravi globini, isci naprej
	//1->   Najdena resitev

	//Presezena globina
	if( problem.stPolpoteze < g ) return -1;
	//Nismo na pravi globini
	if( problem.problem == 1 && g < problem.stPolpoteze) return 0;

	if( ((problem.endmove.sourceIndex < 0)	|| (problem.endmove.sourceIndex == move.sourceIndex)) 	&&
		((problem.endmove.targetIndex < 0)	|| (problem.endmove.targetIndex == move.targetIndex))	&&
		((problem.endmove.piece == NoPiece)	|| (problem.endmove.piece == move.piece)) 				&&
		((!problem.captureSensitive)		|| (problem.endmove.isCapture == move.isCapture)) 		&&
		((!problem.captureSensitive)		|| (!problem.endmove.isCapture == move.isCapture) || ( parrent->board().pieces[problem.looser][problem.capturedPiece] & BitBoard::squareBitmask(move.targetIndex) ) ) &&
		((!problem.promotionSensitive)		|| (problem.endmove.promotionPiece == move.promotionPiece)) &&
		((problem.end.zobristKey == ST_ZOB) || (problem.end == board()) )							&&
		((problem.end.zobristKey != ST_ZOB)	|| (isMate()) ))
		{
			return 1;
		}

	return problem.problem == 1 ? -1 : 0;
}

//&& ( (problem.captureSensitive && ( move.isCapture == problem.endmove.isCapture )) || !problem.captureSensitive)

bool Node::isMate() const
{
	ChessBoard cb= board();
	MoveGenerator generator;

	//Ali je sah
	if( generator.isColorOnMoveUnderCheck(cb, ChessBoardStats(cb)) )
	{
		//Ali je mat
		MoveArray dovoljeniPremiki;
		generator.moves(cb, ChessBoardStats(cb), dovoljeniPremiki);

		for( int i(0); i<dovoljeniPremiki._size; i++ )
		{
			//Preiskusi vsak premik ce ga lahko izvedes( blokiras sah )
			ChessBoard tempBoard= cb;
			dovoljeniPremiki.data[i].applyTo(tempBoard);

			//Ce smo se resili saha vrni -1 ker ni resitve v tej veji
			if( generator.isOpponentsKingNotUnderCheck(tempBoard, ChessBoardStats(tempBoard)) ) return false;
		}
		return true;
	}
	return false;
}

bool Node::operator==(const Node& n1) const
{
	return ((this->board() == n1.board()) && (g == n1.g));
}

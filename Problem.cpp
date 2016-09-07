
#include <fstream>
#include <iostream>
#include <sstream>
#include <cctype>

#include "Problem.h"

Problem::Problem(std::string filename, std::string outname)
{
	//Nastavi log in ok flag
	allok= true;
	logstr= "";

	this->outfile= outname;


	this->problem=0;
	this->heuristika=0;
	this->stPoteze=0;
	this->stPolpoteze= 0;
	this->trnspositionTableSize=1048583;
	this->isciVse=false;

	this->start.setupStandardBoard();
	this->end.setupStandardBoard();

	this->fixedMoveNumber=-1;
	this->captureSensitive= false; this->capturedPiece= NoPiece; promotionSensitive=false;
	this->startmove= Move(); startmove.sourceIndex=-1; startmove.targetIndex=-1; startmove.piece= NoPiece; startmove.promotionPiece= NoPiece; startmove.isCapture=false; startmove.isEnPassant= false;
	this->fixedmove= Move(); fixedmove.sourceIndex=-1; fixedmove.targetIndex=-1; fixedmove.piece= NoPiece; fixedmove.promotionPiece= NoPiece; fixedmove.isCapture=false; fixedmove.isEnPassant= false;
	this->endmove= Move(); endmove.sourceIndex=-1; endmove.targetIndex=-1; endmove.piece= NoPiece; endmove.promotionPiece= NoPiece; endmove.isCapture= false; endmove.isEnPassant= false;

	this->activeFields= start.allPieces();

	this->winner=White;
	this->looser= Black;


	std::ifstream file(filename);
    std::string line;

    while( std::getline( file, line ) )
	{
        if(line.find('#') == 0 || line.length() <= 0) continue;		//Komentar ali prazno

        //Razdeli vrstico po znaku =
        std::string key= line.substr(0, line.find('='));
        std::string value= line.substr(line.find('=')+1, std::string::npos);

        if( key == "PROBLEM" ) parseProblem(value);
        if( key == "H" ) parseH(value);
        else if (key == "TRANSP_TABLE") parseTranspositionTableSize(value);
        else if (key == "SEARCHALL") parseSearchAll(value);

        else if (key == "MOVES") parseNumMoves(value);
        else if( key == "WINCOLOR") parseWinColor(value);

        else if( key == "START" ) parseStartPos(value);
        else if( key == "END" ) parseEndPos(value);

		else if( key == "STARTMOVE") parseStartMove(value);

		else if( key == "FMOVENUMBER") parseFixedMoveNumber(value);
		else if( key == "FMOVE" ) parseFixedMove(value);
		else if ( key == "FMOVEPIECE") parseFmovepiece(value);

        else if( key == "ENDMOVE" ) parseEndMove(value);
        else if( key == "ENDPIECE" ) parseEndpiece(value);
        else if (key == "CAPTURE") parseCapture(value);
        else if (key == "CAPTURED") parseCapturedPiece(value);

        else if( key == "PROMOTIONPIECE" ) parsePromotionPiece(value);

        else if(key == "ACTIVE") parseActiveFields(value);

	}

	//Izracunaj se st polpotez
	stPolpoteze= winner == White ? (2*stPoteze-1) : (2*stPoteze);
}

void Problem::parseH(std::string value)
{
	this->heuristika= stoi(value);
	if( heuristika < 0 || 4 < heuristika ) {log("Wrong heuristic function value"); allok= false;}
}

void Problem::parseProblem(std::string value)
{
	this->problem= stoi(value);
	if( problem < 0 || 2 < problem ) {log("Wrong problem type"); allok= false;}
}

void Problem::parseSearchAll( std::string value )
{
	if( stoi(value) == 0 ) isciVse= false;
	else if( stoi(value) == 1) isciVse= true;
	else {log("Wrong SEARCHALL VALUE"); allok= false;}

}

void Problem::parseNumMoves(std::string value)
{
	this->stPoteze= stoi(value);
	if( stPoteze <= 0 ) {log("Wrong move number"); allok= false;}
}

void Problem::parseFixedMoveNumber(std::string value)
{
	this->fixedMoveNumber= stoi(value);
	if( stPoteze <= 0 ) {log("Wrong fixedmove number"); allok= false;}
}

void Problem::parseTranspositionTableSize(std::string value)
{
	this->trnspositionTableSize= stoi(value);
	if( heuristika < 0 ) {log("Wrong transposition table size"); allok= false;}
}

void Problem::parseStartPos(std::string value)
{
	this->start= ChessBoard(value);
}

void Problem::parseEndPos(std::string value)
{
	this->end= ChessBoard(value);
}

void Problem::parseCapture(std::string value)
{
	int val= stoi(value);
	if( val == -1 ) {this->captureSensitive= true; endmove.isCapture= false;}
	else if( val == 0 )	this->captureSensitive= false;
	else if( val == 1 ) {this->captureSensitive= true; this->endmove.isCapture= true;}
	else {log("Wrong value for capture sensitivity"); allok= false;}
}

void Problem::parseStartMove(std::string value)
{
	if( value.length() == 4)
	{
		this->startmove.sourceIndex= BitBoard::indexFromNotation( value.substr(0, 2) );
		this->startmove.targetIndex= BitBoard::indexFromNotation( value.substr(2, 2) );
	}
	else if( value.length() == 5 && ( value[2] == 'x' || value[2] == 'X' ) )
	{
		this->startmove.sourceIndex= BitBoard::indexFromNotation( value.substr(0, 2) );
		this->startmove.targetIndex= BitBoard::indexFromNotation( value.substr(3, 2) );
	}
	else
	{
		log("Wrong startmove value"); allok= false;
	}
}

void Problem::parseEndMove(std::string value)
{
	//Pazi na neveljavne vnose oziroma pri -- vnosu za katero koli polje

	if( value.length() == 4)
	{
		this->endmove.sourceIndex= BitBoard::indexFromNotation( value.substr(0, 2) );
		this->endmove.targetIndex= BitBoard::indexFromNotation( value.substr(2, 2) );
	}
	else if( value.length() == 5 && ( value[2] == 'x' || value[2] == 'X' ) )
	{
		this->endmove.sourceIndex= BitBoard::indexFromNotation( value.substr(0, 2) );
		this->endmove.targetIndex= BitBoard::indexFromNotation( value.substr(3, 2) );
	}
	else
	{
		log("Wrong endmove value"); allok= false;
	}
}

void Problem::parseFixedMove(std::string value)
{
	//Pazi na neveljavne vnose oziroma pri -- vnosu za katero koli polje
	if( value.length() == 4)
	{
		this->fixedmove.sourceIndex= BitBoard::indexFromNotation( value.substr(0, 2) );
		this->fixedmove.targetIndex= BitBoard::indexFromNotation( value.substr(2, 2) );
	}
	else if( value.length() == 5 && ( value[2] == 'x' || value[2] == 'X' ) )
	{
		this->fixedmove.sourceIndex= BitBoard::indexFromNotation( value.substr(0, 2) );
		this->fixedmove.targetIndex= BitBoard::indexFromNotation( value.substr(3, 2) );
	}
	else
	{
		log("Wrong fixedmove value"); allok= false;
	}
}

void Problem::parseActiveFields(std::string value)
{
	activeFields= 0;
	std::istringstream ss(value);
	std::string polje;

	while( std::getline( ss, polje, ',' ) )
	{
		if( BitBoard::indexFromNotation(polje) < 0 ) {log("Wrong active fields value"); allok= false;}
		else
		{
			bitmask bm= BitBoard::squareBitmask( BitBoard::indexFromNotation(polje) );
			activeFields|= bm;
		}
	}
}

void Problem::parseEndpiece(std::string value)
{
	if( value == "k" || value == "K" ) endmove.piece= King;
	else if( value == "q" || value == "Q" ) endmove.piece= Queen;
	else if( value == "b" || value == "B" ) endmove.piece= Bishop;
	else if( value == "n" || value == "N" ) endmove.piece= Knight;
	else if( value == "r" || value == "R" ) endmove.piece= Rook;
	else if( value == "p" || value == "P" ) endmove.piece= Pawn;
	else {log("Wrong endpiece value"); allok= false;}
}

void Problem::parseCapturedPiece( std::string value )
{
	if( value == "q" || value == "Q" ) capturedPiece= Queen;
	else if( value == "b" || value == "B" ) capturedPiece= Bishop;
	else if( value == "n" || value == "N" ) capturedPiece= Knight;
	else if( value == "r" || value == "R" ) capturedPiece= Rook;
	else if( value == "p" || value == "P" ) capturedPiece= Pawn;
	else {log("Wrong captured value"); allok= false;}
}

void Problem::parseFmovepiece(std::string value)
{
	if( value == "k" || value == "K" ) fixedmove.piece= King;
	else if( value == "q" || value == "Q" ) fixedmove.piece= Queen;
	else if( value == "b" || value == "B" ) fixedmove.piece= Bishop;
	else if( value == "n" || value == "N" ) fixedmove.piece= Knight;
	else if( value == "r" || value == "R" ) fixedmove.piece= Rook;
	else if( value == "p" || value == "P" ) fixedmove.piece= Pawn;
	else {log("Wrong fixedmove value"); allok= false;}
}

void Problem::parsePromotionPiece(std::string value)
{
	promotionSensitive= true;
	if( value == "q" || value == "Q" ) endmove.promotionPiece= Queen;
	else if( value == "b" || value == "B" ) endmove.promotionPiece= Bishop;
	else if( value == "n" || value == "N" ) endmove.promotionPiece= Knight;
	else if( value == "r" || value == "R" ) endmove.promotionPiece= Rook;
	else if( value == "0") endmove.promotionPiece= NoPiece;
	else {log("Wrong promotion piece value value"); allok= false;}
}

void Problem::parseWinColor(std::string value)
{
	if( value == "W" || value == "w" ) {winner= White; looser= Black;}
	else if ( value == "B" || value == "b" ) {winner= Black; looser= White;}
	else {log("Wrong wincolor value"); allok= false;}
}

std::string Problem::getLog() const
{
	return logstr;
}

bool Problem::allOk() const
{
	return allok;
}

void Problem::log( std::string log )
{
	std::stringstream ss;
	ss << logstr << log << "\n";
	logstr= ss.str();
}

std::string Problem::toString() const
{
    using namespace std;

    std::stringstream ss;

    ss<< "\nStart position:\n" << start.toString() << endl;
    if ( end.zobristKey != 0 ) ss<< "\nEnd position:\n" << end.toString() << endl;
    ss<< "\nActive fields:\n" << BitBoard::toString(activeFields) << endl;

	ss<< "Used heuristic function: ";
	if( heuristika == 1 ) ss<< "COVER\n";
	else if( heuristika == 2 ) ss<< "MANHATTAN\n";
	else if( heuristika == 3 ) ss<< "(COVER+ MANHATTAN)\n";
	else if( heuristika == 4 ) ss<< "(MANHATTAN, ISKANJE POZICIJE)\n";
	else ss<< "NONE\n";

	if((0 <= startmove.sourceIndex) || (0 <= startmove.targetIndex)) ss<< "First move: " << startmove.toString(true) << endl;
	if((0 <= fixedmove.sourceIndex) || (0 <= fixedmove.targetIndex)) ss<< "Fixed move: " << fixedMoveNumber << ". " << fixedmove.toString(true) << endl;
	if((0 <= endmove.sourceIndex) || (0 <= endmove.targetIndex)) ss<< "Last move: " << endmove.toString(true) << endl;

	string figure[]= { "King", "Queen", "Bishop", "Knight", "Rook", "Pawn" };
	if( capturedPiece != NoPiece && captureSensitive == 1 ) ss << "Piece captured in last move: " << figure[capturedPiece] << endl;

	ss<< "Transposition table size (nodes): " << trnspositionTableSize << endl;;

	string barva= winner == White ? "White" : "Black";
    if(problem == 1) ss<< "Search for goal in " << stPoteze << " moves. Side to end: " << barva << endl;
    if(problem == 2) ss<<"Search for shortest goal. Side to end " << barva << ". Moves limit: " << stPoteze << ".\n";

    return ss.str();
}

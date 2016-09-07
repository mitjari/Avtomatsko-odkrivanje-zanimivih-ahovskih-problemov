#include <iostream>
#include <set>
#include <list>
#include<ctime>
#include <unordered_map>

#include "movegenerator/chessboard.h"
#include "movegenerator/move.h"
#include "movegenerator/movearray.h"

#include "Node.h"


using namespace std;

int main(int argc, char* argv[] )
{
	string datotekaIn= "bin/Release/LegalsMate.conf";
	string datotekaOut="Solution.pgn";
	if( argc > 1 ) datotekaIn= argv[1];
	if( argc > 2 ) datotekaOut= argv[2];

	Node::problem= Problem(datotekaIn, datotekaOut);

	//Preveri ali ke konfiguracija pravilna
	if( Node::problem.allOk() ) {std::cout<< Node::problem.toString(); std::cout.flush();}
	else {std::cout<< Node::problem.getLog(); std::cout.flush(); return 0;}

    //std::cout<< "\n--------PRESS ENTER TO START SEARCH OR CTRL-C TO EXIT---------\n"; std::cout.flush(); std::cin.get();

	Zobrist zobristGenerator;

	multiset<Node*, PointerNodeCompare> vrsta;
	list<Node*> razvita;

	Node** transpositionTable= new Node* [Node::problem.trnspositionTableSize];	//Array on heap
	for(int i(0); i<Node::problem.trnspositionTableSize; i++) transpositionTable[i]= NULL;

	//Dodaj koren -> zacetna postavitev
	vrsta.insert(new Node());

	//Merjenje hitrosti DEBUG
	const clock_t begin_time = clock();
	int stPreverjanj=0;
	//-----------------------
	while( !vrsta.empty() )
	{
		//Vzami vozlisce iz vrste
		Node* vozlisce= *(vrsta.begin());
		vrsta.erase(vrsta.begin());

		stPreverjanj++; 	//DEBUG
		//std::cout<< "Razvijam " << vozlisce->getF() << std::endl; std::cout.flush(); std:cin.get();
		//std::cout<< vozlisce->board().toString() << std::endl; std::cout.flush(); std::cin.get();

		//Poglej ce je robni pogoj
		short ret= vozlisce->isGoal();

		if(ret == -1) continue;			//-1-> presezena globina ali pa imamo vec resitev, nadaljuj v drugi veji
		else if( ret ==  1)
		{
			//Izpisi resitev
			std::cout<<vozlisce->path();

			std::ofstream solutionFile(Node::problem.outfile, fstream::app);
			if( solutionFile.is_open() )
			{
				std::string eventName= vozlisce->getMove().toString(false);
				solutionFile << "[Event \"" << eventName << "\"]\n" << vozlisce->pathPgn() << std::endl;
				solutionFile.close();
				std::cout<< "\n Solution written to file " << datotekaOut << std::endl;
			}
			else std::cout<< "Error writing to file";

			if(!Node::problem.isciVse) break;	//Ce iscemo samo eno resitev koncaj drugace nadaljuj
		}

		//Generiraj premike iz trenutnega vozlisca
		MoveArray premiki;
		vozlisce->legalMoves(premiki);

		//Za vsak premik ustvari novega naslednjika
		for( int i(0); i<premiki._size; i++ )
		{
			//Preveri prvi premik
			if( vozlisce->getG() == 0 &&
				((0 <= Node::problem.startmove.sourceIndex && Node::problem.startmove.sourceIndex != premiki.data[i].sourceIndex) ||
				(0 <= Node::problem.startmove.targetIndex && Node::problem.startmove.targetIndex != premiki.data[i].targetIndex)) ) continue;

			//Preveri vmesni premik
            if( (0 < Node::problem.fixedMoveNumber && vozlisce->getG()+1 == Node::problem.fixedMoveNumber)								&&
				((0 <= Node::problem.fixedmove.sourceIndex && Node::problem.fixedmove.sourceIndex != premiki.data[i].sourceIndex)	||
				(0 <= Node::problem.fixedmove.targetIndex && Node::problem.fixedmove.targetIndex != premiki.data[i].targetIndex)	||
				(Node::problem.fixedmove.piece != NoPiece && Node::problem.fixedmove.piece != premiki.data[i].piece) )) continue;

			Node* novo= new Node(vozlisce, premiki.data[i]);

			int tIndex= (int) (novo->board().zobristKey % Node::problem.trnspositionTableSize );
			if( transpositionTable[tIndex] != NULL && *(transpositionTable[tIndex]) == *novo )
			{
				//Transpozicija!
				transpositionTable[tIndex]= novo;
			}
			else
			{
				//Ni transpozicija!
				std::multiset<Node*>::iterator xx= vrsta.insert(novo); xx++;
				//if( xx == vrsta.end() && novo->getF() == 23) std::cout<< "JEEEEEEEEEEEEEe";
				transpositionTable[tIndex]= novo;
			}
		}

		//Dodaj vozlisce med razvite
		razvita.push_back(vozlisce);
	}

	double endTime= float( clock () - begin_time ) /  CLOCKS_PER_SEC;

	std::cout << "\n --------Elapsed time: " << endTime << " sec. ---------" << endl << endl;
	std::cout<< " --------Nodes evalueated: " << stPreverjanj << "-------------\n\n"; std::cout.flush();

	//pobrisi vsa vozlisca v vrsti
	for( std::multiset<Node*>::iterator it= vrsta.begin(); it != vrsta.end(); it++)  delete *it;
    //Pobrisi se vsa vozlisca, ki smo jih ze razvili, PAZI na ze izbrisana
    for( std::list<Node*>::iterator it= razvita.begin(); it != razvita.end(); it++)  delete *it;

    //Bobrisi transpozicijsko tabelo (array on heap)
    delete transpositionTable;

    return 0;
}

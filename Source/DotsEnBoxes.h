#ifndef DOTSENBOXES_H
#define DOTSENBOXES_H

#include <vector>
using std::vector;

#include "JohanEngine\Object.h"

enum Player {
	plNone,
	plRed,
	plBlue,
};

// Stores info about kind of wall put around square
class Square {
	public:
		Square() {
			top = plNone;
			right = plNone;
			bottom = plNone;
			left = plNone;
			winner = plNone;
		}
		Player top;
		Player right;
		Player bottom;
		Player left;
		Player winner;
};

class DotsEnBoxes {
	public:
		DotsEnBoxes();
		~DotsEnBoxes();
		
		int redscore;
		int bluescore;
		vector<vector<Square*> > field;
		int xsize;
		int ysize;
		Player currentplayer;
		
		void Reset(int x,int y); // reset to size X*Y
		void DeleteField();
		void ClickHorizontalWall(Object* sender,int x,int y);
		void ClickVerticalWall(Object* sender,int x,int y);
		void NextPlayer();
		bool CheckSquare(int x,int y); // bool -> add point
		void Furnish(int x,int y); // add furniture to conquered rooms
		void UniteRoom(int x,int y); // remove walls between equal colors
};

#endif

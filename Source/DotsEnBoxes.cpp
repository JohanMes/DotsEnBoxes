#include "JohanEngine\Dialogs.h"
#include "JohanEngine\Scene.h"
#include "DotsEnBoxes.h"
#include "resource.h"

DotsEnBoxes::DotsEnBoxes() {
	xsize = 0;
	ysize = 0;
	redscore = 0;
	bluescore = 0;
	currentplayer = plRed;
}

DotsEnBoxes::~DotsEnBoxes() {
	DeleteField();
}

void DotsEnBoxes::Reset(int x,int y) {
	
	// Throw away everything
	DeleteField();
	
	// Set new size
	xsize = x;
	ysize = y;
	
	// Rebuild
	field.resize(xsize);
	for(int i = 0;i < xsize;i++) {
		field[i].resize(ysize);
	}
	
	// Create squares
	for(unsigned int i = 0;i < field.size();i++) {
		for(unsigned int j = 0;j < field[i].size();j++) {
			field[i][j] = new Square();
		}
	}

	// Reset score too
	redscore = 0;
	bluescore = 0;
	redsquare->SetCaption("0");
	bluesquare->SetCaption("0");
}

void DotsEnBoxes::DeleteField() {
	for(unsigned int i = 0;i < field.size();i++) {
		for(unsigned int j = 0;j < field[i].size();j++) {
			delete field[i][j];
		}
	}	
}

void DotsEnBoxes::ClickHorizontalWall(Object* sender,int x,int y) {
	
	char wallname[128];
	sprintf(wallname,"HighWallH %d %d",x,y);
	
	// Replace wall
	new Object(wallname,"HighWall.obj","Brick.mtl",sender->GetTranslation(),sender->matRotation,1);
	delete sender;
	
	bool pointsawarded = false;
	if(x < xsize) { // top exists
		field[x][y]->bottom = currentplayer; // square of which wall is bottom
		pointsawarded = CheckSquare(x,y); // check for winners
	}
	if(x > 0) { // bottom exists
		field[x-1][y]->top = currentplayer; // square of which wall is bottom
		pointsawarded = CheckSquare(x-1,y) or pointsawarded;
	}
	if(!pointsawarded) {
		NextPlayer();
	}
}

void DotsEnBoxes::ClickVerticalWall(Object* sender,int x,int y) {
	
	char wallname[128];
	sprintf(wallname,"HighWallV %d %d",x,y);
	
	// Replace wall
	new Object(wallname,"HighWall.obj","Brick.mtl",sender->GetTranslation(),sender->matRotation,1);
	delete sender;
	
	bool pointsawarded = false;
	if(y < ysize) { // left exists
		field[x][y]->right = currentplayer; // square of which wall is bottom
		pointsawarded = CheckSquare(x,y); // check for winners
	}
	if(y > 0) { // right exists
		field[x][y-1]->left = currentplayer; // square of which wall is bottom
		pointsawarded = CheckSquare(x,y-1) or pointsawarded;
	}
	if(!pointsawarded) {
		NextPlayer();
	}
}

void DotsEnBoxes::NextPlayer() {
	switch(currentplayer) {
		case plBlue: {
			currentplayer = plRed;
			break;
		}
		case plRed: {
			currentplayer = plBlue;
			break;
		}
		case plNone: {
			currentplayer = plRed; // default
			break;
		}
	}
}

bool DotsEnBoxes::CheckSquare(int x,int y) {
	Square* square = field[x][y];

	if(square->top != plNone && square->right != plNone && square->bottom != plNone && square->left != plNone) {
		if(currentplayer == plBlue) {
			bluescore++;
			
			square->winner = currentplayer;
			
			// Add carpet
			scene->objects->AddPlane("BlueCarpet","Blue.mtl",float3(1.5 + 3 * x,0.02,1.5 + 3 * y),0,3,2,13,NULL);

			// Add furniture
			Furnish(x,y);
			
			// Break down walls between rooms
			UniteRoom(x,y);
			
			// Update UI
			char text[8];
			sprintf(text,"%d",bluescore);
			bluesquare->SetCaption(text);
			return true;
			
		} else if(currentplayer == plRed) {
			redscore++;
			
			square->winner = currentplayer;
			
			// 3D!
			scene->objects->AddPlane("RedCarpet","Red.mtl",float3(1.5 + 3 * x,0.02,1.5 + 3 * y),0,3,2,13,NULL);

			// Voeg wat meubels toe
			Furnish(x,y);
			
			// Break down walls between rooms
			UniteRoom(x,y);
	
			// Update UI
			char text[8];
			sprintf(text,"%d",redscore);
			redsquare->SetCaption(text);
			return true;
		}
	}
	return false; // no points awarded
}

void DotsEnBoxes::Furnish(int x,int y) {
	if(RandomRange(0,1) > 0.8) {
		new Object("Table","Painting.obj","Painting.mtl",
			float3(1.5 + 3 * x + 1.4,0.25,1.5 + 3 * y - 0.8),0,1);	
	}
	if(RandomRange(0,1) > 0.6) {
		new Object("Table","Table.obj","WoodBoards.mtl",
			float3(1.5 + 3 * x + 0.25,0.02,1.5 + 3 * y + 0.75),0,1);		
	}
	if(RandomRange(0,1) > 0.7) {
		new Object("Table","Chair.obj","WoodBoards.mtl",
			float3(1.5 + 3 * x,0.02,1.5 + 3 * y - 0.8),float3(0,160,0),1);		
	}
}

void DotsEnBoxes::UniteRoom(int x,int y) {
	Square* center = field[x][y]; // assume occupied
	char name[128];
	
	// Check top/bottm
	if(x > 0 && field[x-1][y]->winner == center->winner) { // two equal colors next to each other
		sprintf(name,"HighWallH %d %d",x,y);
		delete scene->objects->GetByName(name);
	}
	if(x < xsize-1 && field[x+1][y]->winner == center->winner) {
		sprintf(name,"HighWallH %d %d",x+1,y);
		delete scene->objects->GetByName(name);
	}
	
	// Check right/left
	if(y > 0 && field[x][y-1]->winner == center->winner) {
		sprintf(name,"HighWallV %d %d",x,y);
		delete scene->objects->GetByName(name);
	}
	if(y < ysize-1 && field[x][y+1]->winner == center->winner) {
		sprintf(name,"HighWallV %d %d",x,y+1);
		delete scene->objects->GetByName(name);
	}
}

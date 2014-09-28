#include "JohanEngine\Scene.h"
#include "JohanEngine\Renderer.h"
#include "JohanEngine\Skybox.h"
#include "Listeners.h"
#include "resource.h"

void ResetRooms(Component* sender) {

	// Clear and load
	scene->Load("DotsEnBoxes.script"); // template
	
	// Voeg nu het veld toe, leeg welteverstaan
	int xsize = 1;
	int ysize = 1;
	sscanf(horizontalsize->GetCaption(),"%d",&xsize);
	sscanf(verticalsize->GetCaption(),"%d",&ysize);
	
	// Create backend logic
	game->Reset(xsize,ysize);
	
	// Maak hoekpunten
	for(int x = 0;x < xsize+1;x++) {
		for(int y = 0;y < ysize+1;y++) {
			new Object("LowPost","LowPost.obj","Wood.mtl",float3(3 * x,0,3 * y),0,1);
		}
	}
	
	// Maak horizontale muurvakken
	for(int x = 0;x < xsize+1;x++) {
		for(int y = 0;y < ysize;y++) {
			char name[128];
			sprintf(name,"LowWallH %d %d",x,y);
			Object* wall = new Object(name,"LowWall.obj","Wood.mtl",float3(3 * x,0,3 * y + 1.5),0,1);
			wall->OnClick = OnWallClick;
		}
	}
	
	// Maak verticale muurvakken
	for(int x = 0;x < xsize;x++) {
		for(int y = 0;y < ysize+1;y++) {
			char name[128];
			sprintf(name,"LowWallV %d %d",x,y);
			Object* wall = new Object(name,"LowWall.obj","Wood.mtl",float3(3 * x + 1.5,0,3 * y),float3(0,90,0),1);
			wall->OnClick = OnWallClick;
		}
	}
	
	// Center camera
	float3 focus = float3(
		xsize/2.0f * 3.0f,
		-4,
		ysize/2.0f * 3.0f);
	camera->SetPos(focus + float3(-6,14,0));
	camera->SetLookAt(focus);
	
	// Add ground
	Object* ground = scene->objects->AddPlane("Ground","Ground.mtl",0,0,1024,4,192,NULL);
	ground->castshadows = false;
	
	// Add daylight system, enable during the day
	daylight = new Skybox();
	daylight->SetSunlight("Scene\\Sun.obj","Sun.mtl"); // casts shadows too
	scene->lights->SetAmbientlight(0);
	
	// Add flash light, enable at night
	nightlight = new Spotlight(
		float3(0,0,0),
		float3(0,0,0),
		float3(20,20,28),
		45,
		false);
	nightlight->SetCastShadows(true);
}

void OnWallClick(Object* sender) {
	int x,y;
	char type; // type: H == horiz, V == vertical
	if(sscanf(sender->GetName(),"LowWall%1c %d %d",&type,&x,&y) == 3) {
		if(type == 'H') {
			game->ClickHorizontalWall(sender,x,y);
		} else if(type == 'V') {
			game->ClickVerticalWall(sender,x,y);
		}
	}
}

void Help(void* data) {
	new Messagebox("F1:Show help");
}

void SaveBuffers(void* data) {
	renderer->SaveBuffers();
}

void ToggleFullscreen(void* data) {
	renderer->ToggleFullScreen();
}

void ToggleOptions(void* data) {
	OptionsDialog->Toggle();
}

void ToggleConsole(void* data) {
	ConsoleDialog->Toggle();
}

void MoveLight() {
	// Get point of mouse on ground
	float3 worldpos = camera->pos;
	float3 worlddir = renderer->GetPixelWorldRay(ui->mousepos).SetLength(1000); // make sure it intersects the ground plane
	
	// Intersect with ground
	Object* ground = scene->objects->GetByName("Ground");
	if(ground) {
		Collision intersect = ground->IntersectModel(worldpos,worlddir);
		if(intersect.object) {
			float3 lookat = intersect.point;
			float3 pos = lookat + float3(0,10,0);
			
			scene->lights->BeginUpdate();
			nightlight->SetPosition(pos);
			nightlight->SetLookat(lookat);
			scene->lights->EndUpdate();
		}
	}
}

void OnMouseWheel(WPARAM wParam,LPARAM lParam) {
	signed short scroll = HIWORD(wParam);
	if(scroll > 0) {
		camera->Move(2*camera->dir);
	} else {
		camera->Move(-2*camera->dir);
	}
	MoveLight(); // Update light source pos
}

void OnUpdateTime(float dt) {
	scene->lights->BeginUpdate();
	
	// Update skybox
	daylight->UpdateTime(dt);
	
	// Enable flash light at night
	int hours = renderer->GetTimeHours();
	if(hours >= 7 && hours <= 17) {
		nightlight->SetEnabled(false);
	} else {
		nightlight->SetEnabled(true);
		MoveLight(); // Update light source pos
	}
	
	scene->lights->EndUpdate();
}

void OnMouseMove(WPARAM wParam,LPARAM lParam,int dx,int dy) {
	MoveLight();
}

void OnKeyDown(WPARAM wParam,LPARAM lParam,bool handled) {
	if(handled) {
		return;
	}
	
	switch(wParam) {
		case 'W': {
			camera->Move(float3(0.5,0,0));
			break;
		}
		case 'A': {
			camera->Move(float3(0,0,0.5));
			break;
		}
		case 'S': {
			camera->Move(float3(-0.5,0,0));
			break;
		}
		case 'D': {
			camera->Move(float3(0,0,-0.5));
			break;
		}
	}
}

void OnMouseDown(WPARAM wParam,LPARAM lParam,bool handled) {
	if(handled) {
		return;
	}
	
	float3 worldpos = camera->pos;
	float3 worlddir = renderer->GetPixelWorldRay(ui->mousepos).SetLength(1000); // reach 1000m
	Object* mouseobject = scene->IntersectScene(worldpos,worlddir).object;
	if(mouseobject && mouseobject->OnClick) {
		mouseobject->OnClick(mouseobject);
	}
}

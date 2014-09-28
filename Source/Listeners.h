#ifndef LISTENERS_INCLUDED
#define LISTENERS_INCLUDED

#include "JohanEngine\Dialogs.h"

void ResetRooms(Component* sender);
void Help(void* data);
void SaveBuffers(void* data);
void ToggleFullscreen(void* data);
void ToggleConsole(void* data);
void ToggleOptions(void* data);
void MoveLight();
void OnWallClick(Object* sender);
void OnMouseWheel(WPARAM wParam,LPARAM lParam);
void OnUpdateTime(float dt);
void OnMouseMove(WPARAM wParam,LPARAM lParam,int dx,int dy);
void OnMouseDown(WPARAM wParam,LPARAM lParam,bool handled);
void OnKeyDown(WPARAM wParam,LPARAM lParam,bool handled);

#endif

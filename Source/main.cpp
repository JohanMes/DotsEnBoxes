#include "JohanEngine\Renderer.h"
#include "JohanEngine\Dialogs.h"
#include "Listeners.h"
#include "DotsEnBoxes.h"
#include "resource.h"

void CreateKeys() {
	
	ui->AddKey(new Key(VK_F1,Help));
	ui->AddKey(new Key(VK_F2,ToggleFullscreen));
	ui->AddKey(new Key(VK_F3,SaveBuffers));
	ui->AddKey(new Key(VK_F11,ToggleOptions));	
	ui->AddKey(new Key(VK_F12,ToggleConsole));
	
	ui->OnMouseDown = OnMouseDown;
	ui->OnMouseMove = OnMouseMove; // point light at cursor
	ui->OnMouseWheel = OnMouseWheel; // Zoom met scrollwiel
	ui->OnKeyDown = OnKeyDown; // beweeg met WASD
	renderer->OnUpdateTime = OnUpdateTime;
}

void InitGame() {
	
	// Handy keys (if you know of them)
	CreateKeys();
	
	// FPS only
	renderer->ShowTooltip(0);
	
	// Create only menu
	scoreboard = new Window(10,10,200,140,"Dots en Boxes");
	
	// Create score fields
	redsquare = new Label(10,20,85,85,"0",false);
	redsquare->backcolor = float4(1,0,0,1);
	scoreboard->AddChild(redsquare);
	
	bluesquare = new Label(105,20,85,85,"0",false);
	bluesquare->backcolor = float4(0,0,1,1);
	scoreboard->AddChild(bluesquare);
	
	// Allow the player to change field size
	Label* fieldsize = new Label(10,112,15,20,"Size:",true);
	scoreboard->AddChild(fieldsize);
	
	horizontalsize = new Edit(35,112,20,20,"5");
	scoreboard->AddChild(horizontalsize);

	Label* times = new Label(58,112,5,20,"x",true);
	scoreboard->AddChild(times);
	
	verticalsize = new Edit(70,112,20,20,"5");
	scoreboard->AddChild(verticalsize);
	
	// Reset button at the bottom
	Button* resetbutton = new Button(95,110,95,25,"Reset");
	resetbutton->OnClick = ResetRooms;
	scoreboard->AddChild(resetbutton);
	
	ui->AddComponent(scoreboard);
	scoreboard->Show(1);
	
	// Maak backend
	game = new DotsEnBoxes();
	
	// Load playground
	ResetRooms(NULL);
}

void DeleteGame() {
	delete game;
}

// This is where Windows sends user input messages
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	
	// send message to 3D interface
	ui->OnMessage(hwnd,Message,wParam,lParam);
	
	// Perform more handling
	switch(Message) {
		case WM_DESTROY: {
			PostQuitMessage(0); // we are asked to close: kill main thread
			break;
		}
		default: {
			return DefWindowProc(hwnd, Message, wParam, lParam);
		}
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEXA wc = {0};
	MSG Msg = {0};

	// Create a window with these properties
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.lpfnWndProc   = WndProc;
	wc.hInstance     = hInstance;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszClassName = "WindowClass";
	wc.hIcon         = LoadIcon(hInstance,"A"); // laad projecticoon
	wc.hIconSm       = LoadIcon(hInstance,"A");
	
	// Say hi to Windows
	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	// Set up a window with 1024x768 usable pixels
	RECT result = {0,0,1024,768};
	AdjustWindowRect(&result,WS_VISIBLE|WS_OVERLAPPEDWINDOW,false);

	// Create a window with a border and 'client rect' of 1024x768
	hwnd = CreateWindow("WindowClass","DotsEnBoxes",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, // x
		CW_USEDEFAULT, // y
		result.right - result.left, // width
		result.bottom - result.top, // height
		NULL,NULL,hInstance,NULL);
	if(hwnd == NULL) {
		MessageBox(NULL,"Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	// Init render loop
	InitEngine();
	
	InitGame();
	
	// Handle user input. If done, render a frame. Goto 1
	while(Msg.message != WM_QUIT) {
		while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		
		if(!renderer->paused) {
			renderer->Begin(false);
			renderer->DrawScene(scene);
			renderer->End();
		} else {
			Sleep(100);
		}
	}
	
	DeleteGame();
	
	// Render loop stopped due to Alt+F4 etc? Delete everything
	DeleteEngine();

	return Msg.wParam;
}

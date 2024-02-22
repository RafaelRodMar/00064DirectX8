#include <windows.h>
#include <d3d8.h>

//application title
#define APPTITLE "DirectX8 example"

//window event callback function
LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//helper function to set up the window properties
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	//create the window class structure
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	//fill the struct with info
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = (LPSTR)APPTITLE;
	wc.hIconSm = NULL;

	//set up the window with the class info
	return RegisterClassEx(&wc);
}

//entry point for a Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	// declare variables
	MSG msg;

	// register the class
	MyRegisterClass(hInstance);

	// initialize application
	//note--got rid of initinstance
	HWND hWnd;

	//create a new window
	hWnd = CreateWindow(
		(LPSTR)(APPTITLE), //window class
		(LPSTR)(APPTITLE), //title bar
		WS_OVERLAPPEDWINDOW, //window style
		CW_USEDEFAULT, //x position of window
		CW_USEDEFAULT, //y position of window
		500, //width of the window
		400, //height of the window
		NULL, //parent window
		NULL, //menu
		hInstance, //application instance
		NULL); //window parameters

	//was there an error creating the window?
	if (!hWnd)
		return FALSE;

	//display the window
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	IDirect3D8* pd3d; //pointer to an IDirect3D8 object
	pd3d = Direct3DCreate8(D3D_SDK_VERSION); //create IDirect3D8 object
	//destroy IDirect3D8 object
	if (pd3d)
	{
		pd3d->Release();
		pd3d = NULL;
	}

	// main message loop
	int done = 0;
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//look for quit message
			if (msg.message == WM_QUIT)
			{
				MessageBox(hWnd, (LPSTR)"Received WM_QUIT message", (LPSTR)("WinMain"), MB_OK);
				done = 1;
			}

			//decode and pass messages on to WndProc
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//process game loop (else prevents running after window is closed)
		}
	}
	return msg.wParam;
}
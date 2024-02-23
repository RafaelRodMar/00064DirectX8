#include <windows.h>
#include <stdio.h>
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

LPCSTR IntToLPCSTR(int number) {
	static char buffer[256]; // Assuming a maximum of 256 characters
	sprintf_s(buffer, "%d", number);
	return buffer;
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

	//get the number of display adapters available.
	UINT adapters = pd3d->GetAdapterCount();
	MessageBox(hWnd, IntToLPCSTR(adapters), "Number of display adapters available", MB_OK);

	//identify the adapter.
	D3DADAPTER_IDENTIFIER8 adapData;
	pd3d->GetAdapterIdentifier(0, 0, &adapData);
	static char buffer[2000];
	sprintf_s(buffer, "%s %s %d", adapData.Driver, adapData.Description, adapData.VendorId);
	MessageBox(hWnd, buffer, "device data", MB_OK);

	//get capabilities of the adapter
	D3DCAPS8 capsHAL;
	D3DCAPS8 capsRef;
	pd3d->GetDeviceCaps(0, D3DDEVTYPE_HAL, &capsHAL);
	pd3d->GetDeviceCaps(0, D3DDEVTYPE_REF, &capsHAL);

	if (capsHAL.Caps2 & D3DCAPS2_CANRENDERWINDOWED)
	{
		MessageBox(hWnd, "you can render to a window", "caps", MB_OK);
	}
	else
	{
		MessageBox(hWnd, "you can not render to a window", "caps", MB_OK);
	}

	//check the modes of the adapter
	UINT modes = pd3d->GetAdapterModeCount(0);
	MessageBox(hWnd, IntToLPCSTR(modes), "modes", MB_OK);

	int higherWidth = 0;
	int height = 0;
	for (int i = 0; i < modes; i++) {
		D3DDISPLAYMODE dmode;
		pd3d->EnumAdapterModes(0, i, &dmode);
		if (dmode.Width > higherWidth)
		{
			higherWidth = dmode.Width;
			height = dmode.Height;
		}
	}
	static char bufmode[200];
	sprintf_s(bufmode, "%d x %d", higherWidth, height);
	MessageBox(hWnd, bufmode, "resolution with highest width", MB_OK);

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
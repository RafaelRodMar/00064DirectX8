#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <d3d8.h>

//application title
#define APPTITLE "DirectX8 example"

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw; // The transformed position for the vertex
	DWORD color;        // The vertex color
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

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
	MessageBox(hWnd, bufmode, "first resolution with highest width", MB_OK);

	//get the current display mode
	D3DDISPLAYMODE pMode;
	pd3d->GetAdapterDisplayMode(0, &pMode);
	static char bufmode2[200];
	sprintf_s(bufmode2, "%d x %d", pMode.Width, pMode.Height);
	MessageBox(hWnd, bufmode2, "Actual resolution", MB_OK);

	//checking resource formats. The IDirect3D8::CheckDeviceFormat function is used to check whether
	//a particular type of resource can be used on a given adapter in a given
	//mode using a given device type.

	//check if a certain pixel format is supported.
	D3DFORMAT formatToCheck = D3DFMT_X8R8G8B8; // Example pixel format to check
	HRESULT hr = pd3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, pMode.Format,
		D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, formatToCheck);

	if (SUCCEEDED(hr))
	{
		MessageBox(hWnd, "Pixel format is supported", "check support", MB_OK);
	}
	else
	{
		MessageBox(hWnd, "Pixel format is not supported", "check support", MB_OK);
	}

	//check if hardware acceleration is available with certain configuration.
	hr = pd3d->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, pMode.Format, D3DFMT_X8R8G8B8, TRUE);
	if (SUCCEEDED(hr)) {
		MessageBox(hWnd, "D3DDEVTYPE_HAL is supported", "check support", MB_OK);
	}
	else {
		MessageBox(hWnd, "D3DDEVTYPE_HAL is not supported", "check support", MB_OK);
	}

	//create a device.
	D3DPRESENT_PARAMETERS pParam; //structure telling how you want your devide to present scenes.
	ZeroMemory(&pParam, sizeof(pParam));
	pParam.BackBufferFormat = pMode.Format; //D3DFMT_UNKNOWN; //use current.
	//pParam.BackBufferCount = 1;
	//pParam.BackBufferWidth = 0; //0 means the same as the window.
	//pParam.BackBufferHeight = 0;
	//pParam.hDeviceWindow = hWnd;
	//pParam.EnableAutoDepthStencil = FALSE;
	//pParam.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
	//pParam.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	//pParam.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	pParam.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pParam.Windowed = TRUE;
	IDirect3DDevice8 *d8Dev;
	hr = pd3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pParam, &d8Dev);

	static char bufdev[200];
	if (SUCCEEDED(hr))
	{
		sprintf_s(bufdev, "%s", "device created succesfully");
	}
	else
	{
		sprintf_s(bufdev, "%s", "can't create device");
		if(hr == D3DERR_INVALIDCALL) sprintf_s(bufdev, "%s", "invalid call");
		if(hr == D3DERR_NOTAVAILABLE) sprintf_s(bufdev, "%s", "device not available");
		if(hr == D3DERR_OUTOFVIDEOMEMORY) sprintf_s(bufdev, "%s", "out of video memory");
	}
	MessageBox(hWnd, bufdev, "device creation", MB_OK);

	//setting up a viewport (the zone to which you will be rendering).
	D3DVIEWPORT8 pViewport;
	pViewport.X = 0;
	pViewport.Y = 0;
	pViewport.Width = 480;  //dimensions of the window or less.
	pViewport.Height = 360;
	pViewport.MinZ = 0.0f;
	pViewport.MaxZ = 1.0f;
	hr = d8Dev->SetViewport(&pViewport);

	static char bufview[200];
	if (SUCCEEDED(hr))
	{
		sprintf_s(bufview, "%s", "viewport created succesfully");
	}
	else
	{
		sprintf_s(bufview, "%s %s %s", "can not create viewport", IntToLPCSTR(pViewport.Width), IntToLPCSTR(pViewport.Height));
	}
	MessageBox(hWnd, bufview, "viewport creation", MB_OK);

	// Initialize a list of vertices
	CUSTOMVERTEX g_Vertices[] =
	{
		{ 150.0f,  50.0f, 0.5f, 1.0f, 0xffff0000, }, // x, y, z, rhw, color
		{ 151.0f,  50.0f, 0.5f, 1.0f, 0xffff0000, },
		{ 149.0f,  50.0f, 0.5f, 1.0f, 0xffff0000, },
		{ 150.0f,  49.0f, 0.5f, 1.0f, 0xffff0000, },
		{ 150.0f,  51.0f, 0.5f, 1.0f, 0xffff0000, },
		{ 250.0f, 250.0f, 0.5f, 1.0f, 0xff00ff00, },
		{ 251.0f, 250.0f, 0.5f, 1.0f, 0xff00ff00, },
		{ 252.0f, 250.0f, 0.5f, 1.0f, 0xff00ff00, },
		{ 253.0f, 250.0f, 0.5f, 1.0f, 0xff00ff00, },
		{ 254.0f, 250.0f, 0.5f, 1.0f, 0xff00ff00, },
		{  50.0f, 250.0f, 0.5f, 1.0f, 0xff00ffff, },
		{  51.0f, 250.0f, 0.5f, 1.0f, 0xff00ffff, },
		{  52.0f, 250.0f, 0.5f, 1.0f, 0xff00ffff, },
		{  53.0f, 250.0f, 0.5f, 1.0f, 0xff00ffff, },
		{  54.0f, 250.0f, 0.5f, 1.0f, 0xff00ffff, },
	};
	d8Dev->SetVertexShader(D3DFVF_CUSTOMVERTEX);

	//clear a viewport
	d8Dev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0f, 0);

	d8Dev->BeginScene();

	d8Dev->DrawPrimitiveUP(D3DPT_POINTLIST, 15, g_Vertices, sizeof(CUSTOMVERTEX)); //draw some points

	d8Dev->EndScene();

	// Present the backbuffer contents to the display
	// source rect, dest rect, window to send (null for current), always Null.
	d8Dev->Present(NULL, NULL, NULL, NULL);


	//destroy IDirect3D8 object
	if (pd3d)
	{
		if(d8Dev) d8Dev->Release(); //destroy the device.
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
#include "openlens.h"



int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
) {
	print_console = false;
	return OpenLens(hInstance);
}

int main() {
	HWND hInstance = NULL;
	print_console = true;
		GetWindowLongA(
			hInstance,
			GWL_HINSTANCE
		);

	return OpenLens((HINSTANCE)hInstance);
}


int OpenLens(HINSTANCE hInstance)
{

	MSG msg = { 0 };
	gInstance = hInstance;

	CreatePreview(hInstance);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CoUninitialize();

	return (int)msg.wParam;
}

void CreatePreview(HINSTANCE hInstance) {
	WNDCLASSEX wc;
	HWND preWnd;
	// Initialize COM
	if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
	{
		Msg(TEXT("CoInitialize Failed!\r\n"));
		exit(1);
	}

	ZeroMemory(&wc, sizeof wc);
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndMainProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIconSm= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hIcon =  LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(NULL, IDC_NO);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szWindowClass;
	wc.hIconSm = LoadIcon(wc.hInstance, IDI_APPLICATION);
	if (!RegisterClassEx(&wc))
	{
		Msg(TEXT("RegisterClass Failed! Error=0x%x\r\n"), GetLastError());
		CoUninitialize();
		exit(1);
	}

	preWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_CLIPCHILDREN | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT,
		0,
		0,
		hInstance,
		0
	); 
	if (!preWnd) {
		Msg(TEXT("Window cannot be created! Error=0x%x\r\n"), GetLastError());
		exit(0);
	}
	else {
		SetWindowLong(preWnd, GWL_STYLE,
			GetWindowLong(preWnd, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
		CaptureVideo(preWnd);
	}
}


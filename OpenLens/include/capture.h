

#include <dshow.h>
#include "opencl.h"

#pragma once
#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

void CaptureVideo(HWND pWnd);
HRESULT	FindCaptureDevice(IBaseFilter** gottaFilter, WCHAR* wDeviceName);
LRESULT CALLBACK WndMainProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

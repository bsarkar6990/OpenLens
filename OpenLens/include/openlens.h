

#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE
#include <Windows.h>
#include "capture.h"

#pragma once
static TCHAR szWindowClass[] = _T("OpenLens");
static TCHAR szTitle[] = _T("OpenLens Preview");

void CreatePreview(HINSTANCE hInstance);

int OpenLens(HINSTANCE hInstance);

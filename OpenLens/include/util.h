

#include <tchar.h>
#include <d3d9.h>
#include <uuids.h>
#include <strsafe.h>
#include <CL/cl.h>
#include <vector>
#include "resource.h"

#pragma once

#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x501
#endif

#define WM_GRAPHNOTIFY  WM_APP+1
static bool print_console= true;
__declspec(selectany) HINSTANCE gInstance=NULL;

void LogInfo(const char* str, ...);

// Print error notification to the default output. Same usage as with printf
void LogError(const char* str, ...);

#define JIF(x) if (FAILED(hr=(x))) \
    {Msg(TEXT("FAILED(hr=0x%x) in ") TEXT(#x) TEXT("\n\0"), hr); return hr;}

#define LIF(x) if (FAILED(hr=(x))) \
    {Msg(TEXT("FAILED(hr=0x%x) in ") TEXT(#x) TEXT("\n\0"), hr);}

#define NIF(x) if (FAILED(hr=(x))) \
    {Msg(TEXT("FAILED(hr=0x%x) in ") TEXT(#x) TEXT("\n\0"), hr); return NULL;}

#define CL_RELEASE(x) if (x){OIF(clReleaseMemObject(x));}

#define DEFAULT_WIN_WIDTH     640
#define DEFAULT_WIN_HEIGHT    500

#define DEFAULT_VIDEO_WIDTH     DEFAULT_WIN_WIDTH
#define DEFAULT_VIDEO_HEIGHT    480

#define OPENCL_VERSION_1_2  1.2f
#define OPENCL_VERSION_2_0  2.0f

void Msg(TCHAR* szFormat, ...);

int ReadSourceFromFile(const char* fileName, unsigned char** source, size_t* sourceSize);

int read_file(unsigned char** output, size_t* size, const char* name);

int write_file(const char* name, const unsigned char* content, size_t size);

cl_int write_binaries(cl_program program);

void getCacheKernel(unsigned char** output, size_t* size);

void setCacheKernel(unsigned char* output, size_t size);

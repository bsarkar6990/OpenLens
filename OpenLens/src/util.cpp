#include "util.h"

#pragma warning( push )
#pragma warning( disable : 4996 )

void Msg(TCHAR* szFormat, ...)
{
	
	TCHAR szBuffer[1024];  // Large buffer for long filenames or URLs
	const size_t NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
	const int LASTCHAR = NUMCHARS - 1;

	// Format the input string
	va_list pArgs;
	va_start(pArgs, szFormat);
	// Use a bounded buffer size to prevent buffer overruns.  Limit count to
	// character size minus one to allow for a NULL terminating character.
	if (print_console)
		vfprintf(stdout,(const char*) szFormat, pArgs);
	(void)StringCchVPrintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
	va_end(pArgs);

	// Ensure that the formatted string is NULL-terminated
	szBuffer[LASTCHAR] = TEXT('\0');

	if (!print_console)
		MessageBox(NULL, szBuffer, TEXT("OpenLens Message"), MB_OK | MB_ICONERROR);
	

}



void LogInfo(const char* str, ...)
{
	if (str)
	{
		va_list args;
		va_start(args, str);

		vfprintf(stdout, str, args);

		va_end(args);
	}
}

void LogError(const char* str, ...)
{
	if (str)
	{
		va_list args;
		va_start(args, str);

		vfprintf(stderr, str, args);

		va_end(args);
	}
}


int ReadSourceFromFile(const char* fileName, unsigned char** source, size_t* sourceSize)
{
	int errorCode = CL_SUCCESS;

	FILE* fp = NULL;
	fopen_s(&fp, fileName, "rb");
	if (fp == NULL)
	{
		errorCode = CL_INVALID_VALUE;
	}
	else {
		fseek(fp, 0, SEEK_END);
		*sourceSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		*source = new unsigned char[*sourceSize];
		if (*source == NULL)
		{
			Msg(TEXT("Error: Couldn't allocate %d bytes for program source from file '%s'.\n"), *sourceSize, fileName);
			errorCode = CL_OUT_OF_HOST_MEMORY;
		}
		else {
			fread(*source, 1, *sourceSize, fp);

		}
	}
	return errorCode;
}

int read_file(unsigned char** output, size_t* size, const char* name) {
	FILE* fp = fopen(name, "rb");
	if (!fp) {
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	*size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	*output = (unsigned char*)malloc(*size);
	if (!*output) {
		fclose(fp);
		return -1;
	}

	fread(*output, *size, 1, fp);
	fclose(fp);
	return 0;
}


int write_file(const char* name, const unsigned char* content, size_t size) {
	FILE* fp = fopen(name, "wb+");
	if (!fp) {
		return -1;
	}
	if(content!=NULL)
		fwrite(content, size, 1, fp);
	fclose(fp);
	return 0;
}

cl_int write_binaries(cl_program program) {
	cl_int err = CL_SUCCESS;
	size_t binary_size=NULL;
	unsigned char* binary=NULL;

	err=clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &binary_size, NULL);
	if (err != CL_SUCCESS) {
		goto cleanup;
	}

	binary =(unsigned char*) malloc(binary_size);
	if (!binary) {
		err = CL_OUT_OF_HOST_MEMORY;
		goto cleanup;
	}
	err = clGetProgramInfo(program, CL_PROGRAM_BINARIES, binary_size, &binary, NULL);

	char filename[128];
	snprintf(filename, sizeof(filename), "clKernel.bin");
	write_file(filename, binary, binary_size);

	if (err != CL_SUCCESS) {
		goto cleanup;
	}

cleanup:
	// Free the return value buffer
	if (binary!=NULL) {
		free(binary);
	}
	return err;
}

void getCacheKernel(unsigned char** output, size_t* size){
	HRSRC hResource = FindResource(gInstance, MAKEINTRESOURCE(IDR_BIN2), L"BIN");

	if (hResource)
	{
		HGLOBAL hLoadedResource = LoadResource(gInstance, hResource);

		if (hLoadedResource)
		{
			LPVOID pLockedResource = LockResource(hLoadedResource);

			if (pLockedResource)
			{
				DWORD dwResourceSize = SizeofResource(gInstance, hResource);

				if (0 != dwResourceSize)
				{
					*output =(unsigned char*) pLockedResource;
					*size = dwResourceSize;
				}
			}
			FreeResource(hLoadedResource);
		}
	}
}


void setCacheKernel(unsigned char* output, size_t size) {
	HANDLE hndResource  = BeginUpdateResource(TEXT("clKernel.bin"), FALSE);
	if (NULL != hndResource)
	{
		if (UpdateResource(hndResource,
			RT_RCDATA,
			MAKEINTRESOURCE(104),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPVOID)output,
			size) != FALSE)
		{
			EndUpdateResourceW(hndResource, FALSE);
		}
	}
}

#pragma warning( pop )
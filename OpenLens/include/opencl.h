#include "util.h"

#pragma once

class OpenCL_CLS{
public:
	cl_image_format format;
	cl_image_desc desc;
	cl_context context;
	cl_command_queue commandQueue;      // hold the commands-queue handler
	cl_kernel        kernel;
	cl_mem           srcA;            
	cl_mem           dstMem;
	OpenCL_CLS(cl_uint w, cl_uint h);
	~OpenCL_CLS();
	void process(unsigned char* pBuffer);
private:
	int init=0;
	int SetupOpenCL();
	int CreateBufferArguments(unsigned char* pBuffer);
	int CreateAndBuildProgram();
	cl_uint SetKernelArguments();
	cl_uint ExecuteAddKernel();
	cl_uint MapOutBuffer();
};

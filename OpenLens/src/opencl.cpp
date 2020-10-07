
#pragma warning(disable : 4996)
#include "opencl.h"

cl_device_id     device;            // hold the selected device handler
float            platformVersion;   // hold the OpenCL platform version (default 1.2)
float            deviceVersion;     // hold the OpenCL device version (default. 1.2)
float            compilerVersion;   // hold the device OpenCL C version (default. 1.2)
cl_uint			 width;
cl_uint			 height;

const char* TranslateOpenCLError(cl_int errorCode)
{
	switch (errorCode)
	{
	case CL_SUCCESS:                            return "CL_SUCCESS";
	case CL_DEVICE_NOT_FOUND:                   return "CL_DEVICE_NOT_FOUND";
	case CL_DEVICE_NOT_AVAILABLE:               return "CL_DEVICE_NOT_AVAILABLE";
	case CL_COMPILER_NOT_AVAILABLE:             return "CL_COMPILER_NOT_AVAILABLE";
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:      return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case CL_OUT_OF_RESOURCES:                   return "CL_OUT_OF_RESOURCES";
	case CL_OUT_OF_HOST_MEMORY:                 return "CL_OUT_OF_HOST_MEMORY";
	case CL_PROFILING_INFO_NOT_AVAILABLE:       return "CL_PROFILING_INFO_NOT_AVAILABLE";
	case CL_MEM_COPY_OVERLAP:                   return "CL_MEM_COPY_OVERLAP";
	case CL_IMAGE_FORMAT_MISMATCH:              return "CL_IMAGE_FORMAT_MISMATCH";
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:         return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case CL_BUILD_PROGRAM_FAILURE:              return "CL_BUILD_PROGRAM_FAILURE";
	case CL_MAP_FAILURE:                        return "CL_MAP_FAILURE";
	case CL_MISALIGNED_SUB_BUFFER_OFFSET:       return "CL_MISALIGNED_SUB_BUFFER_OFFSET";                          //-13
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:    return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";   //-14
	case CL_COMPILE_PROGRAM_FAILURE:            return "CL_COMPILE_PROGRAM_FAILURE";                               //-15
	case CL_LINKER_NOT_AVAILABLE:               return "CL_LINKER_NOT_AVAILABLE";                                  //-16
	case CL_LINK_PROGRAM_FAILURE:               return "CL_LINK_PROGRAM_FAILURE";                                  //-17
	case CL_DEVICE_PARTITION_FAILED:            return "CL_DEVICE_PARTITION_FAILED";                               //-18
	case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:      return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";                         //-19
	case CL_INVALID_VALUE:                      return "CL_INVALID_VALUE";
	case CL_INVALID_DEVICE_TYPE:                return "CL_INVALID_DEVICE_TYPE";
	case CL_INVALID_PLATFORM:                   return "CL_INVALID_PLATFORM";
	case CL_INVALID_DEVICE:                     return "CL_INVALID_DEVICE";
	case CL_INVALID_CONTEXT:                    return "CL_INVALID_CONTEXT";
	case CL_INVALID_QUEUE_PROPERTIES:           return "CL_INVALID_QUEUE_PROPERTIES";
	case CL_INVALID_COMMAND_QUEUE:              return "CL_INVALID_COMMAND_QUEUE";
	case CL_INVALID_HOST_PTR:                   return "CL_INVALID_HOST_PTR";
	case CL_INVALID_MEM_OBJECT:                 return "CL_INVALID_MEM_OBJECT";
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:    return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case CL_INVALID_IMAGE_SIZE:                 return "CL_INVALID_IMAGE_SIZE";
	case CL_INVALID_SAMPLER:                    return "CL_INVALID_SAMPLER";
	case CL_INVALID_BINARY:                     return "CL_INVALID_BINARY";
	case CL_INVALID_BUILD_OPTIONS:              return "CL_INVALID_BUILD_OPTIONS";
	case CL_INVALID_PROGRAM:                    return "CL_INVALID_PROGRAM";
	case CL_INVALID_PROGRAM_EXECUTABLE:         return "CL_INVALID_PROGRAM_EXECUTABLE";
	case CL_INVALID_KERNEL_NAME:                return "CL_INVALID_KERNEL_NAME";
	case CL_INVALID_KERNEL_DEFINITION:          return "CL_INVALID_KERNEL_DEFINITION";
	case CL_INVALID_KERNEL:                     return "CL_INVALID_KERNEL";
	case CL_INVALID_ARG_INDEX:                  return "CL_INVALID_ARG_INDEX";
	case CL_INVALID_ARG_VALUE:                  return "CL_INVALID_ARG_VALUE";
	case CL_INVALID_ARG_SIZE:                   return "CL_INVALID_ARG_SIZE";
	case CL_INVALID_KERNEL_ARGS:                return "CL_INVALID_KERNEL_ARGS";
	case CL_INVALID_WORK_DIMENSION:             return "CL_INVALID_WORK_DIMENSION";
	case CL_INVALID_WORK_GROUP_SIZE:            return "CL_INVALID_WORK_GROUP_SIZE";
	case CL_INVALID_WORK_ITEM_SIZE:             return "CL_INVALID_WORK_ITEM_SIZE";
	case CL_INVALID_GLOBAL_OFFSET:              return "CL_INVALID_GLOBAL_OFFSET";
	case CL_INVALID_EVENT_WAIT_LIST:            return "CL_INVALID_EVENT_WAIT_LIST";
	case CL_INVALID_EVENT:                      return "CL_INVALID_EVENT";
	case CL_INVALID_OPERATION:                  return "CL_INVALID_OPERATION";
	case CL_INVALID_GL_OBJECT:                  return "CL_INVALID_GL_OBJECT";
	case CL_INVALID_BUFFER_SIZE:                return "CL_INVALID_BUFFER_SIZE";
	case CL_INVALID_MIP_LEVEL:                  return "CL_INVALID_MIP_LEVEL";
	case CL_INVALID_GLOBAL_WORK_SIZE:           return "CL_INVALID_GLOBAL_WORK_SIZE";                           //-63
	case CL_INVALID_PROPERTY:                   return "CL_INVALID_PROPERTY";                                   //-64
	case CL_INVALID_IMAGE_DESCRIPTOR:           return "CL_INVALID_IMAGE_DESCRIPTOR";                           //-65
	case CL_INVALID_COMPILER_OPTIONS:           return "CL_INVALID_COMPILER_OPTIONS";                           //-66
	case CL_INVALID_LINKER_OPTIONS:             return "CL_INVALID_LINKER_OPTIONS";                             //-67
	case CL_INVALID_DEVICE_PARTITION_COUNT:     return "CL_INVALID_DEVICE_PARTITION_COUNT";                     //-68
	case CL_INVALID_PIPE_SIZE:                  return "CL_INVALID_PIPE_SIZE";                                  //-69
	case CL_INVALID_DEVICE_QUEUE:               return "CL_INVALID_DEVICE_QUEUE";                               //-70    

	default:
		return "UNKNOWN ERROR CODE";
	}
}

bool CheckPreferredPlatformMatch(cl_platform_id platform, const char* preferredPlatform);
cl_platform_id FindOpenCLPlatform(const char* preferredPlatform, cl_device_type deviceType);
void GetPlatformAndDeviceVersion(cl_platform_id platformId);


OpenCL_CLS::OpenCL_CLS(cl_uint w, cl_uint h) {
	kernel = NULL;
	commandQueue = NULL;
	context = NULL;
	width = w;
	height = h;
	srcA = NULL;
	dstMem = NULL;
	format.image_channel_data_type = CL_UNORM_INT8;
	format.image_channel_order = CL_RGBA;
	desc.image_type = CL_MEM_OBJECT_IMAGE2D;
	desc.image_width = w;
	desc.image_height = h;
	desc.image_depth = 0;
	desc.image_array_size = 1;
	desc.image_row_pitch = 0;
	desc.image_slice_pitch = 0;
	desc.num_mip_levels = 0;
	desc.num_samples = 0;
#ifdef CL_VERSION_2_0
	desc.mem_object = NULL;
#else
	desc.buffer = NULL;
#endif

}

OpenCL_CLS::~OpenCL_CLS() {

	cl_int hr = CL_SUCCESS;
	if (kernel)
	{
		LIF(clReleaseKernel(kernel));
	}
	if (commandQueue)
	{
		LIF(clReleaseCommandQueue(commandQueue));
	}
	if (srcA) {
		clReleaseMemObject(srcA);
	}
	if (dstMem) {
		clReleaseMemObject(dstMem);
	}
	if (commandQueue)
	{
		clReleaseCommandQueue(commandQueue);
	}
	if (device)
	{
		clReleaseDevice(device);
	}
	if (context)
	{
		clReleaseContext(context);
	}
}

int OpenCL_CLS::CreateBufferArguments(unsigned char* iBuffer) {

	cl_int hr = CL_SUCCESS;
	srcA = clCreateImage(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, &format, &desc, iBuffer, &hr);
	if (CL_SUCCESS != hr)
	{
		return hr;
	}
	if (init == 0) {
		dstMem = clCreateImage(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, &format, &desc, iBuffer, &hr);
		if (CL_SUCCESS != hr)
		{
			LogError("Error: clCreateFromGLTexture for dstMem returned %s\n", TranslateOpenCLError(hr));
			return hr;
		}
	}
}

cl_uint OpenCL_CLS::SetKernelArguments()
{

	cl_int hr = CL_SUCCESS;

	JIF(clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)& srcA));

	JIF(clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)& dstMem));

	JIF(clSetKernelArg(kernel, 2, sizeof(cl_uint), (void*)& width));

	JIF(clSetKernelArg(kernel, 3, sizeof(cl_uint), (void*)& height));

	return hr;
}

void OpenCL_CLS::process(unsigned char* pBuffer) {
	if (init == 0) {
		SetupOpenCL();
	}
	CreateBufferArguments(pBuffer);
	if (init == 0) {
		CreateAndBuildProgram();
	}
	SetKernelArguments();
	ExecuteAddKernel();
	MapOutBuffer();
	if (srcA) {
		clReleaseMemObject(srcA);
	}
	if (init == 0) {
		init = 1;
	}
}

cl_uint OpenCL_CLS::ExecuteAddKernel() {
	cl_int hr = CL_SUCCESS;

	size_t globalWorkSize[2] = { width, height };

	JIF(clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, NULL));

	JIF(clFinish(commandQueue));

	return CL_SUCCESS;
}

cl_uint OpenCL_CLS::MapOutBuffer() {
	cl_int hr = CL_SUCCESS;
	bool result = true;

	size_t origin[] = { 0, 0, 0 };
	size_t region[] = { width, height, 1 };
	size_t image_row_pitch;
	size_t image_slice_pitch;
	cl_int* resultPtr = (cl_int*)clEnqueueMapImage(commandQueue, dstMem, true, CL_MAP_READ, origin, region, &image_row_pitch, &image_slice_pitch, 0, NULL, NULL, &hr);

	if (CL_SUCCESS != hr)
	{
		return false;
	}

	hr = clFinish(commandQueue);

	hr = clEnqueueUnmapMemObject(commandQueue, dstMem, resultPtr, 0, NULL, NULL);

	return result;
}

int OpenCL_CLS::CreateAndBuildProgram()
{
	cl_int hr = CL_SUCCESS, binary_status;
	unsigned char* source = NULL;
	size_t src_size = 0;
	cl_program program = NULL;
	bool cache = false;
	hr = ReadSourceFromFile("src\\kernel.cl", &source, &src_size);
	if (CL_SUCCESS != hr)
	{
		LogError("Error: ReadSourceFromFile returned %s.\n", TranslateOpenCLError(hr));
	}
	if (source != NULL) {
		program = clCreateProgramWithSource(context, 1, (const char**)& source, &src_size, &hr);
	}
	if (program == NULL) {
		if (source!=NULL)
		{
			delete[] source;
			source = NULL;
		}
		getCacheKernel(&source, &src_size);
		if (source != NULL) {
			cache = true;
			program =
				clCreateProgramWithBinary(context, 1, &device, &src_size,
				(const unsigned char**)& source,
					NULL, &hr);
			if (CL_SUCCESS != hr)
			{
				LogError("Error: clCreateProgramWithBinary returned %s.\n", TranslateOpenCLError(hr));
				goto Finish;
			}
		}
	}
	hr = clBuildProgram(program, 1, &device, "", NULL, NULL);
	if (CL_SUCCESS != hr)
	{
		LogError("Error: clBuildProgram() for source program returned %s", (TranslateOpenCLError(hr)));

		if (hr == CL_BUILD_PROGRAM_FAILURE)
		{
			size_t log_size = 0;
			clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

			std::vector<char> build_log(log_size);
			clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, &build_log[0], NULL);

			LogError("Error happened during the build of OpenCL program.\nBuild log:%s", &build_log[0]);
		}
	}

Finish:
	if (source != NULL && cache==false)
	{
		delete[] source;
		source = NULL;
		if (program != NULL) {
			write_binaries(program);
			getCacheKernel(&source, &src_size);
		}
	}
	if (CL_SUCCESS == hr) {
		kernel = clCreateKernel(program, "Transfer", &hr);
	}
	if (program) {
		LIF(clReleaseProgram(program));
	}
	if (device) {
		LIF(clReleaseDevice(device));
	}
	return hr;
}

int OpenCL_CLS::SetupOpenCL() {

	cl_int hr = CL_SUCCESS;
	cl_device_type deviceType = CL_DEVICE_TYPE_GPU;

	cl_platform_id platformId = FindOpenCLPlatform("Intel", deviceType);
	if (NULL == platformId)
	{
		Msg(TEXT("Error: Failed to find OpenCL platform.\n"));
		return CL_INVALID_VALUE;
	}

	cl_context_properties contextProperties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platformId, 0 };

	context = clCreateContextFromType(contextProperties, deviceType, NULL, NULL, &hr);
	if ((CL_SUCCESS != hr) || (NULL == context))
	{
		Msg(TEXT("Couldn't create a context, clCreateContextFromType() returned '%s'.\n"), TranslateOpenCLError(hr));
		return hr;
	}

	JIF(clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(cl_device_id), &device, NULL));

	GetPlatformAndDeviceVersion(platformId);

#ifdef CL_VERSION_2_0
	if (OPENCL_VERSION_2_0 == deviceVersion)
	{
		const cl_command_queue_properties properties[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0 };
		commandQueue = clCreateCommandQueueWithProperties(context, device, properties, &hr);
	}
	else {
		cl_command_queue_properties properties = CL_QUEUE_PROFILING_ENABLE;
		commandQueue = clCreateCommandQueue(context, device, properties, &hr);
	}
#else
	cl_command_queue_properties properties = CL_QUEUE_PROFILING_ENABLE;
	commandQueue = clCreateCommandQueue(context, device, properties, &hr);
#endif
	if (CL_SUCCESS != hr)
	{
		Msg(TEXT("Error: clCreateCommandQueue() returned %s.\n"), TranslateOpenCLError(hr));
		return hr;
	}

	return hr;
}

cl_platform_id FindOpenCLPlatform(const char* preferredPlatform, cl_device_type deviceType) {
	cl_uint numPlatforms = 0;

	cl_int hr = CL_SUCCESS;
	NIF(clGetPlatformIDs(0, NULL, &numPlatforms));

	if (0 == numPlatforms)
	{
		Msg(TEXT("Error: No Opencl platforms found!\n"));
		return NULL;
	}

	std::vector<cl_platform_id> platforms(numPlatforms);

	NIF(clGetPlatformIDs(numPlatforms, &platforms[0], NULL));

	for (cl_uint i = 0; i < numPlatforms; i++)
	{
		bool match = true;
		cl_uint numDevices = 0;

		if ((NULL != preferredPlatform) && (strlen(preferredPlatform) > 0))
		{
			match = CheckPreferredPlatformMatch(platforms[i], preferredPlatform);
		}
		if (match)
		{
			NIF(clGetDeviceIDs(platforms[i], deviceType, 0, NULL, &numDevices));

			if (0 != numDevices)
			{
				return platforms[i];
			}
		}

	}

	return NULL;
}

bool CheckPreferredPlatformMatch(cl_platform_id platform, const char* preferredPlatform)
{
	size_t stringLength = 0;
	bool match = false;

	cl_int hr = CL_SUCCESS;
	JIF(clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, NULL, &stringLength));

	std::vector<char> platformName(stringLength);

	JIF(clGetPlatformInfo(platform, CL_PLATFORM_NAME, stringLength, &platformName[0], NULL));

	if (strstr(&platformName[0], preferredPlatform) != 0)
	{
		match = true;
	}

	return match;
}

void GetPlatformAndDeviceVersion(cl_platform_id platformId)
{
	size_t stringLength = 0;
	cl_int hr = CL_SUCCESS;

	LIF(clGetPlatformInfo(platformId, CL_PLATFORM_VERSION, 0, NULL, &stringLength));

	std::vector<char> platformVer(stringLength);

	LIF(clGetPlatformInfo(platformId, CL_PLATFORM_VERSION, stringLength, &platformVer[0], NULL));

	if (strstr(&platformVer[0], "OpenCL 2.0") != NULL)
	{
		platformVersion = OPENCL_VERSION_2_0;
	}

	LIF(clGetDeviceInfo(device, CL_DEVICE_VERSION, 0, NULL, &stringLength));

	std::vector<char> deviceVer(stringLength);

	LIF(clGetDeviceInfo(device, CL_DEVICE_VERSION, stringLength, &deviceVer[0], NULL));

	if (strstr(&deviceVer[0], "OpenCL 2.0") != NULL)
	{
		deviceVersion = OPENCL_VERSION_2_0;
	}

	LIF(clGetDeviceInfo(device, CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &stringLength));

	std::vector<char> compilerVer(stringLength);

	LIF(clGetDeviceInfo(device, CL_DEVICE_OPENCL_C_VERSION, stringLength, &compilerVer[0], NULL));

	if (strstr(&compilerVer[0], "OpenCL C 2.0") != NULL)
	{
		compilerVersion = OPENCL_VERSION_2_0;
	}
}

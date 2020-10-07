#include "capture.h"

EXTERN_C const CLSID CLSID_SampleGrabber;
EXTERN_C const IID IID_ISampleGrabber;


ICaptureGraphBuilder2* pCaptureGraph;
IGraphBuilder* pGraph;
IMediaControl* pControl;
IMediaEventEx* pMediaEventEx;
IBaseFilter* pCameraFilter;
IBaseFilter* pVideoInputFilter;
IBaseFilter* pDestFilter;
AM_MEDIA_TYPE* pAmMediaType;
IBaseFilter* pGrabberF;
WCHAR wDeviceName[255];
HRESULT hr;
IVideoWindow* pVidWin;
IBasicVideo* pBasicVideo;
HWND preWnd;

bool processVideo = false;
struct ISampleGrabber;
struct ISampleGrabberCB;

MIDL_INTERFACE("6B652FFF-11FE-4fce-92AD-0266B5D7C78F")
ISampleGrabber : public IUnknown
{
  public:
	virtual HRESULT STDMETHODCALLTYPE SetOneShot(
		BOOL OneShot) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetMediaType(
		const AM_MEDIA_TYPE* pType) = 0;

	virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType(
		AM_MEDIA_TYPE* pType) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetBufferSamples(
		BOOL BufferThem) = 0;

	virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer(
		/* [out][in] */ long* pBufferSize,
		/* [out] */ long* pBuffer) = 0;

	virtual HRESULT STDMETHODCALLTYPE GetCurrentSample(
		/* [retval][out] */ IMediaSample** ppSample) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetCallback(
		ISampleGrabberCB* pCallback,
		long WhichMethodToCallback) = 0;

};

MIDL_INTERFACE("0579154A-2B53-4994-B0D0-E773148EFF85")
ISampleGrabberCB : public IUnknown
{
  public:
	virtual HRESULT STDMETHODCALLTYPE SampleCB(
		double SampleTime,
		IMediaSample * pSample) = 0;

	virtual HRESULT STDMETHODCALLTYPE BufferCB(
		double SampleTime,
		BYTE* pBuffer,
		long BufferLen) = 0;

};

class SampleGrabberCallback : public ISampleGrabberCB {
public:
	SampleGrabberCallback() {
		ocl = new OpenCL_CLS(640, 480);
	}

	~SampleGrabberCallback() {
	}
private:
	OpenCL_CLS* ocl;
	STDMETHODIMP_(ULONG) AddRef() { return 1; }

	STDMETHODIMP_(ULONG) Release() { return 2; }

	STDMETHODIMP QueryInterface(REFIID /*riid*/, void** ppvObject) {
		*ppvObject = static_cast<ISampleGrabberCB*>(this);
		return S_OK;
	}

	STDMETHODIMP SampleCB(double /*Time*/, IMediaSample* pSample) {
		unsigned char* ptrBuffer;
		HRESULT hr = pSample->GetPointer(&ptrBuffer);
		if (processVideo) {
			ocl->process(ptrBuffer);
			ptrBuffer = NULL;
		}
			return S_OK;
	}

	STDMETHODIMP BufferCB(double /*Time*/, BYTE* pBuffer, long /*BufferLen*/) {
		return E_NOTIMPL;
	}
};

ISampleGrabber* pGrabber;
SampleGrabberCallback* sgCallback;

HRESULT FindCaptureDevice(IBaseFilter** gottaFilter, WCHAR* wDeviceName) {
	BOOL done = false;
	int deviceCounter = 0;
	memset(wDeviceName, 0, sizeof(WCHAR) * 255);
	// Create the System Device Enumerator.
	ICreateDevEnum* pSysDevEnum = NULL;
	JIF( CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)& pSysDevEnum));
	
	// Obtain a class enumerator for the video input category.
	IEnumMoniker* pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);

	if (hr == S_OK)
	{
		// Enumerate the monikers.
		IMoniker* pMoniker = NULL;
		ULONG cFetched;
		while ((pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK) && (!done))
		{

			// Bind the first moniker to an object
			IPropertyBag* pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)& pPropBag);
			if (SUCCEEDED(hr))
			{
				// To retrieve the filter's friendly name, do the following:
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				if (SUCCEEDED(hr))
				{

					//copy the name to nDeviceName & wDeviceName
					int count = 0;
					while (varName.bstrVal[count] != 0x00) {
						wDeviceName[count] = varName.bstrVal[count];
						count++;
					}
					// We found it, so send it back to the caller
					hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)gottaFilter);
					done = true;
				}
				VariantClear(&varName);
				SAFE_RELEASE(pPropBag);
				SAFE_RELEASE(pMoniker);
			}

			deviceCounter++;
		}
		SAFE_RELEASE(pEnumCat);
	}
	SAFE_RELEASE(pSysDevEnum);
	if (done) {
		return hr;	// found it, return native error
	}
	else {
		return VFW_E_NOT_FOUND;	// didn't find it error
	}
}

HRESULT InitVideoWindow()
{
	LONG lHeight, lWidth;
	HRESULT hr = S_OK;
	RECT rect;
	int nMultiplier = 1;
	int nDivider = 1;
	if (!pBasicVideo)
		return S_OK;

	// Read the default video size
	hr = pBasicVideo->GetVideoSize(&lWidth, &lHeight);
	if (hr == E_NOINTERFACE)
		return S_OK;

	// Account for requests of normal, half, or double size
	lWidth = lWidth * nMultiplier / nDivider;
	lHeight = lHeight * nMultiplier / nDivider;

	int nTitleHeight = GetSystemMetrics(SM_CYCAPTION);
	int nBorderWidth = GetSystemMetrics(SM_CXBORDER);
	int nBorderHeight = GetSystemMetrics(SM_CYBORDER);

	// Account for size of title bar and borders for exact match
	// of window client area to default video size
	SetWindowPos(preWnd, NULL, 0, 0, lWidth + 2 * nBorderWidth,
		lHeight + nTitleHeight + 2 * nBorderHeight,
		SWP_NOMOVE | SWP_NOOWNERZORDER);

	// Move the video size/position to within the application window
	GetClientRect(preWnd, &rect);
	JIF(pVidWin->SetWindowPosition(rect.left, rect.top, rect.right, rect.bottom));

	return hr;
}

HRESULT SetupVideoWindow(void) {

	long pWidth, pHeight;
	JIF(pGraph->QueryInterface(IID_IVideoWindow, (void**)& pVidWin));
	JIF(pGraph->QueryInterface(IID_IBasicVideo, (void**)& pBasicVideo));
	JIF(pVidWin->put_Owner((OAHWND)preWnd));
	InitVideoWindow();
	JIF(pVidWin->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS));
	JIF(pVidWin->put_WindowStyleEx(WS_EX_LAYOUTRTL | WS_POPUP));
}

void InitCapture(void) {

	LIF(CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)& pCaptureGraph));

	LIF(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)& pGraph));

	LIF(pCaptureGraph->SetFiltergraph(pGraph));

	LIF(pGraph->QueryInterface(IID_IMediaControl, (void**)& pControl));

	LIF(pGraph->QueryInterface(IID_IMediaEventEx, (LPVOID*)& pMediaEventEx));

	LIF(pMediaEventEx->SetNotifyWindow((OAHWND)preWnd, WM_GRAPHNOTIFY, 0));

	LIF(FindCaptureDevice(&pVideoInputFilter, wDeviceName));

	LIF(pGraph->AddFilter(pVideoInputFilter, wDeviceName));

	LIF(CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)& pGrabberF));

	LIF(pGraph->AddFilter(pGrabberF, L"Sample Grabber"));

	LIF(pGrabberF->QueryInterface(IID_ISampleGrabber, (void**)& pGrabber));

	LIF(pGrabber->SetBufferSamples(FALSE));

	sgCallback = new SampleGrabberCallback();
	LIF(pGrabber->SetCallback(sgCallback, 0));

	AM_MEDIA_TYPE mt;
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_ARGB32;
	mt.formattype = FORMAT_VideoInfo;

	LIF(pGrabber->SetMediaType(&mt));

	LIF(CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)(&pDestFilter)));

	LIF(pGraph->AddFilter(pDestFilter, L"Video Mixing Renderer 9"));

	LIF(pCaptureGraph->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pVideoInputFilter, pGrabberF, pDestFilter));

	IMediaFilter* pMediaFilter = 0;
	hr = pGraph->QueryInterface(IID_IMediaFilter, (void**)& pMediaFilter);
	if (FAILED(hr)) {
		Msg(TEXT("ERROR: Could not sync source to sink"));
	}
	else {
		pMediaFilter->SetSyncSource(NULL);
		SAFE_RELEASE(pMediaFilter);
	}

	SetupVideoWindow();

	LIF(pControl->Run());

	SAFE_RELEASE(pDestFilter);
	SAFE_RELEASE(pGrabber);
	SAFE_RELEASE(pGrabberF);
	SAFE_RELEASE(pVideoInputFilter);
	SAFE_RELEASE(pGraph);
	SAFE_RELEASE(pCaptureGraph);
}

void CaptureVideo(HWND pWnd) {
	preWnd = pWnd;
	processVideo = true;
	InitCapture();
}

HRESULT HandleGraphEvent(void)
{
	LONG evCode;
	LONG_PTR evParam1, evParam2;

	if (!pMediaEventEx)
		return E_POINTER;

	while (SUCCEEDED(pMediaEventEx->GetEvent(&evCode, &evParam1, &evParam2, 0)))
	{
		//
		// Free event parameters to prevent memory leaks associated with
		// event parameter data.  While this application is not interested
		// in the received events, applications should always process them.
		//
		hr = pMediaEventEx->FreeEventParams(evCode, evParam1, evParam2);

		// Insert event processing code here, if desired
	}

	return hr;
}

void ResizeVideoWindow(void)
{
	// Resize the video preview window to match owner window size
	if (pVidWin)
	{
		RECT rc;

		// Make the preview video fill our window
		GetClientRect(preWnd, &rc);
		pVidWin->SetWindowPosition(rc.left, rc.top, rc.right, rc.bottom);
	}
}

void CloseInterfaces(void) {
	if (pControl) {
		pControl->StopWhenReady();
		SAFE_RELEASE(pControl);
	}
	if (pMediaEventEx) {
		pMediaEventEx->SetNotifyWindow(NULL, WM_GRAPHNOTIFY, 0);
		SAFE_RELEASE(pMediaEventEx);
	}
	if (pVidWin)
	{
		pVidWin->put_Visible(OAFALSE);
		pVidWin->put_Owner(NULL);
		SAFE_RELEASE(pVidWin);
	}
	if(pBasicVideo)
		SAFE_RELEASE(pBasicVideo);
	if(sgCallback)
		delete sgCallback;
}


LRESULT CALLBACK WndMainProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = GetDC(hwnd);
	switch (message)
	{
	case WM_GRAPHNOTIFY:
		HandleGraphEvent();
		break;
	//case WM_MOVE:
	case WM_SIZE:
		ResizeVideoWindow();
		break;

	case WM_WINDOWPOSCHANGED:
		//ChangePreviewState(!(IsIconic(hwnd)));
		break;

	case WM_CLOSE:
		// Hide the main window while the graph is destroyed
		ShowWindow(hwnd, SW_HIDE);
		CloseInterfaces();  // Stop capturing and release interfaces
		break;
	case WM_CHAR:

		switch (wParam)
		{
		case 'p':
			SetTextColor(hdc, RGB(0, 255, 0));
			TextOut(hdc, 0, DEFAULT_VIDEO_HEIGHT - 100, TEXT("p pressed"), 9);
			return 0;
			break;
		case 0x09:
			SetTextColor(hdc, RGB(0, 255, 0));
			TextOut(hdc, 0, 10, L"tabkey pressed", 15);
			return 0;
			break;
		case 0x1B:
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, 0, 10, L"esckey pressed", 15);
			return 0;
			break;
		default: return 0;
			break;
			ReleaseDC(hwnd, hdc);
		}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	// Pass this message to the video window for notification of system changes
	if (pVidWin)
		pVidWin->NotifyOwnerMessage((LONG_PTR)hwnd, message, wParam, lParam);

	return DefWindowProc(hwnd, message, wParam, lParam);
}

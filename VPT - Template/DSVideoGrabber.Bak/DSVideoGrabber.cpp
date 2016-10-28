#include "StdAfx.h"
#include "DSVideoGrabber.h"

#define WM_GRAPHNOTIFY WM_APP+13

// The sample grabber is calling us back on its deliver thread.
// This is NOT the main app thread!
//

STDMETHODIMP CSampleGrabberCB::BufferCB( double SampleTime, BYTE * pBuffer, long BufferSize )
{
	framenum++;

	/*_tprintf(TEXT("Found a sample at time %ld ms\t[%s]\r\n"), 
	long(SampleTime*1000), szFilename );*/
	/*
	// Write out the file header
	//
	BITMAPFILEHEADER bfh;
	memset( &bfh, 0, sizeof( bfh ) );
	bfh.bfType = 'MB';
	bfh.bfSize = sizeof( bfh ) + BufferSize + sizeof( BITMAPINFOHEADER );
	bfh.bfOffBits = sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER );

	// Write the bitmap format
	//
	BITMAPINFOHEADER bih;
	memset( &bih, 0, sizeof( bih ) );
	bih.biSize = sizeof( bih );
	bih.biWidth = Width;
	bih.biHeight = Height;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	*/
	if(pFrameBufferQueue) {
//		memcpy(pCopyBuf, pBuffer, BufferSize) ;
		pFrameBufferQueue->Enqueue(pBuffer) ;
		if(pFrameBufferQueue->FreeSlotsCount() <= 0) {
			pCDSGraphBuilder->Pause() ;
		}
//		if(hOwner) {
//			::SendMessage(hOwner, WM_TIMER, 0, 0) ;
//		}
	}

	return 0;
}


CDSVideoGrabber::CDSVideoGrabber(void)
{
	pCDSGraphBuilder = NULL ;
	pSampleGrabber = NULL ;
	pNullRenderer = NULL ;
	pGrabber = NULL ;

	pFrameBufferQueue = NULL ;

	frameCount = 0 ;
	currentFrameNumber = 0 ;
}

void CDSVideoGrabber::Pause(void) {
	pCDSGraphBuilder->Pause() ;
}

void CDSVideoGrabber::Run(void) {
	pCDSGraphBuilder->Run() ;
}

int CDSVideoGrabber::Init(HWND owner) {

	pCDSGraphBuilder = new CDSGraphBuilder(owner) ;

	if(pCDSGraphBuilder) {
		hOwner = owner ;
	}
	else {
		hOwner = 0 ;
		return -1 ;
	}

	pFrameBufferQueue = NULL ;
	return 0 ;
}

int CDSVideoGrabber::SetOwner(HWND owner) {
	// hOwner stores the handle to the dialog
	hOwner = owner;

	grabberCB.hOwner = owner ;

	// set our dialog to receive events from graph manager
	pCDSGraphBuilder->SetOwner(owner) ;
	return 0 ;
}

void CDSVideoGrabber::CleanUp(void) {
	if (pSampleGrabber)
	{
		pSampleGrabber->Release() ;
		pSampleGrabber = NULL ;
	}
	if (pGrabber) {
		pGrabber->Release() ;
		pGrabber = NULL ;
	}
	if (pNullRenderer)
	{
		pNullRenderer->Release() ;
		pSampleGrabber = NULL ;
	}
	if (pFrameBufferQueue)
	{
		delete pFrameBufferQueue ;
		pFrameBufferQueue = NULL ;
	}
	if (pCDSGraphBuilder) {
		delete pCDSGraphBuilder ;
		pCDSGraphBuilder = NULL ;
	}

	frameCount = 0 ;
	currentFrameNumber = 0 ;
}

int CDSVideoGrabber::AddSampleGrabber(void) {

	HRESULT hr ;
	AM_MEDIA_TYPE mt;

	hr = pCDSGraphBuilder->AddFilter(CLSID_SampleGrabber, &pSampleGrabber, _T("SampleGrabber")) ;

	if(FAILED(hr)) return -1 ;

	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24;
//	mt.formattype = GUID_NULL;
	mt.formattype = FORMAT_VideoInfo ;

	if(pGrabber) {
		pGrabber->Release() ;
		pGrabber = NULL ;
	}
	hr = pSampleGrabber->QueryInterface(IID_ISampleGrabber, (void**)&pGrabber) ;
	hr = pGrabber->SetMediaType(&mt) ;
	pGrabber->SetOneShot(FALSE);   
	pGrabber->SetBufferSamples(FALSE);
	pGrabber->SetCallback(&grabberCB, 1) ;
	grabberCB.SetGraphBuilder(pCDSGraphBuilder) ;

	if(FAILED(hr)) {
		return -1 ;
	}

	return 0 ;
}

int CDSVideoGrabber::AddNullRenderer(void) {

	HRESULT hr ;

	hr = pCDSGraphBuilder->AddFilter(CLSID_NullRenderer, &pNullRenderer, _T("NullRenderer")) ;
	if(FAILED(hr)) return -1 ;
	return 0 ;
}

int CDSVideoGrabber::LoadFile(const TCHAR *filename) {

	IBaseFilter *pVideoRenderer = NULL ;

	AddSampleGrabber() ;

	if(pCDSGraphBuilder->RenderFile(filename)) {
		return -1 ;
	}

	width = pCDSGraphBuilder->GetVideoWidth() ;
	height = pCDSGraphBuilder->GetVideoHeight() ;

	if(pFrameBufferQueue) {
		delete pFrameBufferQueue ;
	}

	AddNullRenderer() ;

	pVideoRenderer = pCDSGraphBuilder->GetVideoRenderer() ;

	if(!pVideoRenderer) {
		return -1 ;
	}

	pFrameBufferQueue = new CFrameBufferQueue(4, width, height, 3) ;
	grabberCB.pFrameBufferQueue = pFrameBufferQueue ;

	/*
	{
		if(pVideoRender)
			pVideoRenderer->Release() ;
		pVideoRenderer = NULL ;
	}
	*/

	if(pVideoRenderer) {
		//get input pin of video renderer
		IPin* ipin = pCDSGraphBuilder->GetPin(pVideoRenderer, PINDIR_INPUT);
		IPin* opin = NULL;
		//find out who the renderer is connected to and disconnect from them
		ipin->ConnectedTo(&opin);
		ipin->Disconnect();
		opin->Disconnect();

		if(pNullRenderer) {
			pCDSGraphBuilder->ConnectFilters(opin, pNullRenderer) ;
		}

		ipin->Release() ;
		opin->Release() ;

		//remove the default renderer from the graph		
		pCDSGraphBuilder->RemoveFilter(pVideoRenderer);

		pVideoRenderer->Release() ;

		/*
		//see if the video renderer was originally connected to 
		//a color space converter
		IBaseFilter* pColorConverter = NULL;
		m_pGraph->FindFilterByName(L"Color Space Converter", &pColorConverter);
		if(pColorConverter)
		{
			opin->Release() ;

			//remove the converter from the graph as well
			ipin = GetPin(pColorConverter, PINDIR_INPUT);

			ipin->ConnectedTo(&opin);
			ipin->Disconnect();
			opin->Disconnect();

			ipin->Release() ;
			
			m_pGraph->RemoveFilter(pColorConverter);
			pColorConverter->Release() ;
		}
		*/
	}

	frameCount = (long long) pCDSGraphBuilder->GetFrameCount() ;

	currentFrameNumber = 0 ;
	SeekToFrame(0) ;
	pCDSGraphBuilder->Run() ;

	return 0 ;
}

int CDSVideoGrabber::SeekToFrame(long long frameNum) {

	currentFrameNumber = frameNum ;
	return pCDSGraphBuilder->SetFramePositions(frameNum, frameNum) ;
}

int CDSVideoGrabber::SetStopFrame(long long frameNum) {

	return pCDSGraphBuilder->SetStopFrame(frameNum) ;
}

int CDSVideoGrabber::SetCurrentFrame(long long frameNum) {

	return pCDSGraphBuilder->SetCurrentFrame(frameNum) ;
}

HRESULT CDSVideoGrabber::NextFrame(unsigned char *bits) {

	if(pFrameBufferQueue->UsedSlotsCount() > 0) {
		currentFrameNumber++ ;
		pFrameBufferQueue->Dequeue(bits) ;
		pCDSGraphBuilder->Run() ;
	} else {
		if(currentFrameNumber < (frameCount - 1))
			return E_PENDING ;
		else
			return E_POINTER ;
	}
	return S_OK ;
}

HRESULT CDSVideoGrabber::StepNextFrame(unsigned char *bits) {

	currentFrameNumber++ ;
	if(currentFrameNumber < frameCount) {
		SeekToFrame(currentFrameNumber) ;
		while(pFrameBufferQueue->Dequeue(bits)) ;
	} else {
		currentFrameNumber = frameCount - 1 ;
		return E_POINTER ;
	}
	return S_OK ;
}

HRESULT CDSVideoGrabber::StepPrevFrame(unsigned char *bits) {

	currentFrameNumber-- ;
	if(currentFrameNumber >= 0) {
		SeekToFrame(currentFrameNumber) ;
		while(pFrameBufferQueue->Dequeue(bits)) ;
	} else {
		currentFrameNumber = 0 ;
		return E_POINTER ;
	}
	return S_OK ;
}

int CDSVideoGrabber::GetVideoWidth(void) {

	return width ;
}

int CDSVideoGrabber::GetVideoHeight(void) {

	return height ;
}

CDSVideoGrabber::~CDSVideoGrabber(void)
{
	CleanUp() ;
}

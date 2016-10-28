#include "StdAfx.h"
#include "AppQuantize.h"

CAppQuantize::CAppQuantize(void)
{
	// Class Constructor
}

CAppQuantize::~CAppQuantize(void)
{
	// Class Destructor
	// Must call Final() function in the base class

	Final() ;
}

void CAppQuantize::CustomInit(CView *pView) {
	// Add custom initialization code here
	// This initialization code will be called when this application is added to a processing task lists

	CreateTunnel(width, height, &pOutput565) ;
}

void CAppQuantize::CustomFinal(void) {
	// Add custom finalization code here

	RemoveTunnel(pOutput565) ;
}

// This function converts input 24-bit image (8-8-8 format, in pInput pointer) to a 5-5-5 image.
// This function shall allocate storage space for output images, and return the output image as a pointer.
// The input reference variable qDataSize, is also serve as an output variable to indicate the buffer size (in bytes) required for the output image
unsigned char *CAppQuantize::Quantize555(int &qDataSize) {

	int i, j ;
	unsigned int r, g, b ;
	unsigned short rgb16 ;

	qDataSize = width * height * 2 ;

	unsigned char *quantizedImageData = new unsigned char[width * height * 2] ;

	for(j = 0; j < height; j++) {
		for(i = 0; i < width; i++) {
			b = pInput[(i + j * width) * 3 + 0] ;	// Blue Color Component
			g = pInput[(i + j * width) * 3 + 1] ;	// Red Color Component
			r = pInput[(i + j * width) * 3 + 2] ;	// Green COlor Component
			rgb16 = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3) ;
			quantizedImageData[(i + j * width) * 2 + 0] = rgb16 & 0xFF ;
			quantizedImageData[(i + j * width) * 2 + 1] = (rgb16 >> 8) & 0xFF ;
		}
	}

	return quantizedImageData ;
}

// This function converts input 16-bit image (5-5-5 format, in quantizedImageData) to a 24-bit (8-8-8) image.
// This function should return the output image through unsigned char *unquantizedImageData.
void CAppQuantize::Dequantize555(unsigned char *quantizedImageData, unsigned char *unquantizedImageData) {

	int i, j ;
	unsigned int r, g, b ;
	unsigned short rgb16 ;

	for(j = 0; j < height; j++) {
		for(i = 0; i < width; i++) {
			rgb16 = quantizedImageData[(i + j * width) * 2 + 0] | (((unsigned short) quantizedImageData[(i + j * width) * 2 + 1]) << 8) ;
			b = rgb16 & 0x1F;
			g = (rgb16 >> 5) & 0x1F ;
			r = (rgb16 >> 10) & 0x1F ;
			unquantizedImageData[(i + j * width) * 3 + 0] = (b << 3) ;
			unquantizedImageData[(i + j * width) * 3 + 1] = (g << 3) ;
			unquantizedImageData[(i + j * width) * 3 + 2] = (r << 3) ;
		}
	}
}

// This function converts input 24-bit image (8-8-8 format, in pInput pointer) to a 5-6-5 image.
// This function shall allocate storage space for output images, and return the output image as a pointer.
// The input reference variable qDataSize, is also serve as an output variable to indicate the buffer size (in bytes) required for the output image
unsigned char *CAppQuantize::Quantize565(int &qDataSize) {

	// You can modify anything within this function, but you cannot change the function prototype.

	qDataSize = width * height * 2 ;
	unsigned char *quantizedImageData = new unsigned char[width * height * 2] ;

	return quantizedImageData ;
}

// This function converts input 16-bit image (5-6-5 format, in quantizedImageData) to a 24-bit (8-8-8) image.
// This function should return the output image through unsigned char *unquantizedImageData.
void CAppQuantize::Dequantize565(unsigned char *quantizedImageData, unsigned char *unquantizedImageData) {

	// You can modify anything within this function, but you cannot change the function prototype.
}

void CAppQuantize::Process(void) {

	// Don't change anything within this function.

	int qDataSize ;

	unsigned char *quantizedData ;

	SetTitle(pOutput, _T("5-5-5 Output")) ;

	SetTitle(pOutput565, _T("5-6-5 Output")) ;

	quantizedData = Quantize555(qDataSize) ;

	Dequantize555(quantizedData, pOutput) ;

	delete [] quantizedData ;

	quantizedData = Quantize565(qDataSize) ;

	Dequantize565(quantizedData, pOutput565) ;

	delete [] quantizedData ;

	printf(_T("Original Size = %d, Quantized Size = %d, Compression Ratio = %2.2f\r\n"), width * height * 3, qDataSize, (double) width * height * 3 / qDataSize) ;

	PutDC(pOutput) ;
	PutDC(pOutput565) ;
}

#pragma once
#include "../processing.h"

class CAppQuantize :
	public CProcessing
{
public:
	// Add variables here
	unsigned char *pOutput565 ;

public:
	CAppQuantize(void);
	~CAppQuantize(void);
	// Add methods here

	unsigned char *Quantize555(int &qDataSize) ;
	void Dequantize555(unsigned char *quantizedImageData, unsigned char *unquantizedImageData) ;

	unsigned char *Quantize565(int &qDataSize) ;
	void Dequantize565(unsigned char *quantizedImageData, unsigned char *unquantizedImageData) ;

public:
	void CustomInit(CView *pView) ;
	void Process(void) ;
	void CustomFinal(void) ;
};

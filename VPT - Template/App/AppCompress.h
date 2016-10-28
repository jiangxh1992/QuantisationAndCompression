#pragma once
#include "../processing.h"

class CAppCompress :
	public CProcessing
{
public:
	// Add variables here

public:
	CAppCompress(void);
	~CAppCompress(void);
	// Add methods here

	unsigned char *Compress(int &cDataSize) ;
	void Decompress(unsigned char *compressedData, int cDataSize, unsigned char *deCompressedData) ;

public:
	void CustomInit(CView *pView) ;
	void Process(void) ;
	void CustomFinal(void) ;
};

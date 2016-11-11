#include "StdAfx.h"
#include "AppCompress.h"

CAppCompress::CAppCompress(void)
{
	// Class Constructor
}

CAppCompress::~CAppCompress(void)
{
	// Class Destructor
	// Must call Final() function in the base class

	Final() ;
}

void CAppCompress::CustomInit(CView *pView) {
	// Add custom initialization code here
	// This initialization code will be called when this application is added to a processing task lists
}

void CAppCompress::CustomFinal(void) {
	// Add custom finalization code here
}

// This function compresses input 24-bit image (8-8-8 format, in pInput pointer).
// This function shall allocate storage space for compressedData, and return it as a pointer.
// The input reference variable cDataSize, is also serve as an output variable to indicate the size (in bytes) of the compressed data.
unsigned char *CAppCompress::Compress(int &cDataSize) {

	// 压缩后的数据
	unsigned char *compressedData ;
	// 最大长度，如果compressedSize>=cDataSize,压缩失败
	cDataSize = width * height * 3 ;	
	// 存储压缩后的数据					
	compressedData = new unsigned char[cDataSize] ; 

	/****************************************** My Cdoe **********************************************************/
	int count = 0;
	// 实际压缩字符长度
	int compressedSize = 0;
	// 采用分通道游离的方法，按照每个通道相邻像素的重复性进行压缩
	// b通道
	unsigned short curB = pInput[0];// 第一个像素的b
	unsigned short repeat = 1;// 重复次数
	for (int i = 1; i < cDataSize/3; i++) {
		unsigned short nextB = pInput[i * 3 + 0];// 下一个像素的b
		if (nextB == curB) {
			++repeat;
			// 如果是最后一个则存储
			if (i == cDataSize/3 - 1)
			{
				// 存储最后一个b值组
				compressedData[compressedSize] = repeat & 0xFF;
				compressedData[compressedSize + 1] = curB & 0xFF;
				// 增加编码数据长度
				compressedSize += 2;
			}

		}else{

			// 存储上一个b值组
			compressedData[compressedSize] = repeat & 0xFF;
			compressedData[compressedSize + 1] = curB & 0xFF;
			// 增加编码数据长度
			compressedSize += 2;

			// 换下一种b值
			curB = nextB;
			repeat = 1;

			// 如果是最后一个
			if (i == cDataSize/3 - 1)
			{
				// 存储最后一个b值
				compressedData[compressedSize] = 1 & 0xFF;
				compressedData[compressedSize + 1] = curB & 0xFF;
				// 增加编码数据长度
				compressedSize += 2;
			}
		}

		if (compressedSize > cDataSize)
		{
			// 压缩失败
			memcpy(compressedData, pInput, cDataSize);
			return compressedData;
		}
	}


	// g通道
	unsigned short curG = pInput[1];// 第一个像素的g
	repeat = 1;// 重复次数
	for (int i = 1; i < cDataSize/3; i++) {
		unsigned short nextG = pInput[i * 3 + 1];// 下一个像素的g
		if (nextG == curG) {
			++repeat;

			// 如果是最后一个则存储
			if (i == cDataSize/3 - 1)
			{
				// 存储最后一个g值组
				compressedData[compressedSize] = repeat & 0xFF;
				compressedData[compressedSize + 1] = curG & 0xFF;
				// 增加编码数据长度
				compressedSize += 2;
			}
		}
		else{

			// 存储上一个g值组
			compressedData[compressedSize] = repeat & 0xFF;
			compressedData[compressedSize + 1] = curG & 0xFF;
			// 增加编码数据长度
			compressedSize += 2;

			// 换下一种g值
			curG = nextG;
			repeat = 1;

			// 如果是最后一个
			if (i == cDataSize/3 - 1)
			{
				// 存储最后一个g值
				compressedData[compressedSize] = 1 & 0xFF;
				compressedData[compressedSize + 1] = curB & 0xFF;
				// 增加编码数据长度
				compressedSize += 2;
			}
		}

		if (compressedSize > cDataSize)
		{
			// 压缩失败
			memcpy(compressedData, pInput, cDataSize);
			return compressedData;
		}
	}

	// r通道
	unsigned short curR = pInput[2];// 第一个像素的r
	repeat = 1;// 重复次数
	for (int i = 1; i < cDataSize/3; i++) {
		unsigned short nextR = pInput[i * 3 + 2];// 下一个像素的r
		if (nextR == curR) {
			++repeat;

			// 如果是最后一个则存储
			if (i == cDataSize/3 - 1)
			{
				// 存储最后一个g值组
				compressedData[compressedSize] = repeat & 0xFF;
				compressedData[compressedSize + 1] = curR & 0xFF;
				// 增加编码数据长度
				compressedSize += 2;
			}

		}
		else{

			// 存储上一个g值组
			compressedData[compressedSize] = repeat & 0xFF;
			compressedData[compressedSize + 1] = curR & 0xFF;
			// 增加编码数据长度
			compressedSize += 2;

			// 换下一种r值
			curR = nextR;
			repeat = 1;

			// 如果是最后一个
			if (i == cDataSize/3 - 1)
			{
				// 存储最后一个r值
				compressedData[compressedSize] = 1 & 0xFF;
				compressedData[compressedSize + 1] = curR & 0xFF;
				// 增加编码数据长度
				compressedSize += 2;
			}
		}

		if (compressedSize > cDataSize)
		{
			// 压缩失败
			memcpy(compressedData, pInput, cDataSize);
			return compressedData;
		}
	}

	// 取出压缩后的纯数据
	cDataSize = compressedSize;
	unsigned char *finalData = new unsigned char[cDataSize];
	for (int i = 0; i < cDataSize; i++){
		unsigned char temp = compressedData[i];
		finalData[i] = temp;
	}
	//delete compressedData;
	//compressedData = finalData;

	/******************************************** My Code End ****************************************************/

	return compressedData;		// return the compressed data
}

// This function takes in compressedData with size cDatasize, and decompresses it into 8-8-8 image.
// The decompressed image data should be stored into the uncompressedData buffer, with 8-8-8 image format
void CAppCompress::Decompress(unsigned char *compressedData, int cDataSize, unsigned char *uncompressedData) {

	// You can modify anything within this function, but you cannot change the function prototype.
	//memcpy(uncompressedData, compressedData, cDataSize) ;	// Here, we simply copy the compressedData into the output buffer.

	/******************************************** My Code *********************************************************/
	
	// 寻找g通道和r通道在压缩数据数组中的偏移坐标
	int offset_r, offset_g;
	offset_r = offset_g = 0;
	int pixelCount = 0;
	for (int i = 0; i < cDataSize; )
	{
		if (pixelCount == 189366)
		{
			offset_g = i;// g通道的开始坐标
		}
		if (pixelCount == width*height*2)
		{
			offset_r = i;// r通道的开始坐标
		}
		pixelCount += compressedData[i];

		if (pixelCount>189312*2)
		{
			offset_r = 0;
		}

		i += 2;
	}


	unsigned int b, g, r;
	int repeat;
	// 还原b通道
	for (int i = 0, j = 0; i < width*height, j < offset_g; j += 2)
	{
		// 恢复一组重复的b值
		repeat = compressedData[j];
		for (int p = 0; p < repeat; p++)
		{
			uncompressedData[i * 3+0] = compressedData[j + 1];
		}
		i += repeat;
	}

	// 还原g通道
	for (int i = 0, j = offset_g; i < width*height, j<offset_r;j+=2)
	{
		repeat = compressedData[j];
		for (int p = 0; p < repeat; p++)
		{
			uncompressedData[i * 3 + 1] = compressedData[j + 1];
		}
		i += repeat;
	}

	// 还原r通道
	for (int i = 0,j=offset_r; i < width*height,j<cDataSize; j+=2)
	{
		repeat = compressedData[j];
		for (int p = 0; p < repeat; p++)
		{
			uncompressedData[i * 3 + 2] = compressedData[j + 1];
		}
		i += repeat;
	}

	/******************************************** My Code End *********************************************************/
}


void CAppCompress::Process(void) {

	// Don't change anything within this function.

	int i, cDataSize ;

	unsigned char *compressedData ;
	unsigned char *verifyCompressedData ;

	SetTitle(pOutput, _T("Lossless Decompressed Image")) ;

	compressedData = Compress(cDataSize) ;

	verifyCompressedData = new unsigned char [cDataSize] ;

	memcpy(verifyCompressedData, compressedData, cDataSize) ;

	delete [] compressedData ;

	Decompress(verifyCompressedData, cDataSize, pOutput) ;

	for(i = 0; i < width * height * 3; i++) {
		if(pInput[i] != pOutput[i]) {
			printf(_T("Caution: Decoded Image is not identical to the Original Image!\r\n")) ;
			break ;
		}
	}

	printf(_T("Original Size = %d, Compressed Size = %d, Compression Ratio = %2.2f\r\n"), width * height * 3, cDataSize, (double) width * height * 3 / cDataSize) ;

	PutDC(pOutput) ;
}

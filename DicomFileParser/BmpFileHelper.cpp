#include "stdafx.h"
#include "BmpFileHelper.h"
//#include "wingdi.h"

#pragma pack(2)//必须得写，否则sizeof得不到正确的结果 

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef long    LONG;

typedef struct {
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
	DWORD      biSize;
	LONG       biWidth;
	LONG       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	LONG       biXPelsPerMeter;
	LONG       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
} BITMAPINFOHEADER;

/* 彩色表:调色板 */
typedef struct RGB_QUAD
{
	char rgbBlue;     // 蓝色强度   
	char rgbGreen;    // 绿色强度   
	char rgbRed;      
	char rgbReserved;
} RGBQUAD;

BmpFileHelper::BmpFileHelper()
{
}


BmpFileHelper::~BmpFileHelper()
{
}

void BmpFileHelper::CreateBmpFile(int width, int height, unsigned char* pData, int nSize)
{
	// Part.1 Create Bitmap File Header  
	BITMAPFILEHEADER fileHeader;

	fileHeader.bfType = 0x4D42;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + nSize;
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// Part.2 Create Bitmap Info Header  
	BITMAPINFOHEADER bitmapHeader = { 0 };

	bitmapHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapHeader.biHeight = height;
	bitmapHeader.biWidth = width;
	bitmapHeader.biPlanes = 1;
	bitmapHeader.biBitCount = 8;
	bitmapHeader.biSizeImage = nSize;
	bitmapHeader.biCompression = 0;   


	//RGB_QUAD plate[256] = {0};
	RGB_QUAD *plate = new RGB_QUAD[256];
	for (int i = 0; i < 256; i++)
	{
		plate[i].rgbBlue = i;
		plate[i].rgbGreen = i;
		plate[i].rgbRed = i;
		plate[i].rgbReserved = 0;
	}
									// Write to file  
	FILE *output;
	fopen_s(&output, "d:\\output.bmp", "wb");

	if (output == NULL)
	{
		printf("Cannot open file!\n");
	}
	else
	{
		fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, output);
		fwrite(&bitmapHeader, sizeof(BITMAPINFOHEADER), 1, output);
		fwrite(plate, sizeof(RGB_QUAD) * 256, 1, output);
		fwrite(pData, nSize, 1, output);
		fclose(output);
	}
}

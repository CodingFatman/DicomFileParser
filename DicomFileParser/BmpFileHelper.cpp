#include "stdafx.h"
#include "BmpFileHelper.h"
#include  <direct.h>    
#include  <stdio.h> 
#include "iostream"
#include "windows.h"

#pragma pack(2)//必须得写，否则sizeof得不到正确的结果 

BmpFileHelper::BmpFileHelper()
{
}


BmpFileHelper::~BmpFileHelper()
{
}

std::string BmpFileHelper::TCHAR2String(TCHAR* STR)
{
	int iLen = WideCharToMultiByte(CP_ACP, 0,STR, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen * sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
	std::string str(chRtn);
	return str;
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
	RGBQUAD *plate = new RGBQUAD[256];
	for (int i = 0; i < 256; i++)
	{
		plate[i].rgbBlue = i;
		plate[i].rgbGreen = i;
		plate[i].rgbRed = i;
		plate[i].rgbReserved = 0;
	}

	TCHAR  szFullPath[255];
	ZeroMemory(szFullPath, 255);
	GetModuleFileName(NULL, szFullPath, MAX_PATH);
	(_tcsrchr(szFullPath, _T('\\')))[1] = 0;

	std::string currentPath = TCHAR2String(szFullPath);
	currentPath.append("output.bmp");

	FILE *output;
	fopen_s(&output, currentPath.c_str(), "wb");

	if (output == NULL)
	{
		printf("Cannot open file!\n");
	}
	else
	{
		fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, output);
		fwrite(&bitmapHeader, sizeof(BITMAPINFOHEADER), 1, output);
		fwrite(plate, sizeof(RGBQUAD) * 256, 1, output);
		fwrite(pData, nSize, 1, output);
		fclose(output);
	}
}

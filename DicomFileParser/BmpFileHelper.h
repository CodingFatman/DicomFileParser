#pragma once
#include "iostream"

class BmpFileHelper
{
public:
	BmpFileHelper();
	~BmpFileHelper();

	void CreateBmpFile(int width, int height, unsigned char* pData, int nSize);

	static std::string TCHAR2String(TCHAR *STR);
};


#pragma once
class BmpFileHelper
{
public:
	BmpFileHelper();
	~BmpFileHelper();

	void CreateBmpFile(int width, int height, unsigned char* pData, int nSize);
};


// TestDicom.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "iostream"
#include "fstream"
#include "DicomReader.h"


int main()
{
	TCHAR  szFullPath[255];
	ZeroMemory(szFullPath, 255);
	GetModuleFileName(NULL, szFullPath, MAX_PATH);
	(_tcsrchr(szFullPath, _T('\\')))[1] = 0;

	std::string currentPath = BmpFileHelper::TCHAR2String(szFullPath);
	currentPath.append("input.dcm");

	DicomReader reader;
	reader.LoadDicomFile(currentPath);

	std::cout << "press enter key to finish programme";
	getchar();

	return 0;
}

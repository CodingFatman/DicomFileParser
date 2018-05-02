// TestDicom.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "iostream"
#include "fstream"
#include "DicomReader.h"


int main()
{
	DicomReader reader;
	reader.LoadDicomFile("d:\\test.dcm");

	return 0;
}

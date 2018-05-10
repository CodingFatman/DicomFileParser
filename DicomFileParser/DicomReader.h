#pragma once

#include "iostream"
#include "fstream"
#include <map>
#include <sstream>
#include "DataElement.h"
#include "BmpFileHelper.h"

class DicomReader
{
public:
	bool IsValidDicomFile;

	std::string transferSyntax;
	std::string patientName;
	int samplesPerPixel;
	int pixelRepresentation;
	int rows;
	int columns;
	int bitsAllocated;
	int bitsStored;
	int highBit;
	float rescaleIntercept;
	float rescaleSlop;
	int windowCenter;
	int windowWidth;

	DicomReader();
	~DicomReader();

	void LoadDicomFile(std::string dicomFileName);
private:
	std::string CstExplicitVRLittleEndian = "1.2.840.10008.1.2.1";
	DataElement *currentTag;
	std::map<std::string, DataElement*> dicomData;
	std::ifstream infile;
	bool ReadFileMetaInfo();
	bool IsReservedVR(std::string vr);
	void SkipOtherDicomtag();
	DataElement* ReadDicomTag();
	void ReadTagValue(DataElement* dataElement);
	void ExtractTagValue();

	void DisplayDicomTagValue();

	void ProcessPixelDataTag();

	template<typename PixelType>
	PixelType* ReadPixelData();

	template<typename PixelType>
	void SavePixelDataTo8BitBmpFile(PixelType* pixelData);
};


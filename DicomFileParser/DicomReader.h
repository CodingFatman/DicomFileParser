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

	DicomReader();
	~DicomReader();

	void LoadDicomFile(std::string dicomFileName);

private:
	DataElement *currentTag;
	std::map<std::string, DataElement*> dicomData;
	std::ifstream infile;
	void ReadFileMetaInfo();
	bool IsReservedVR(std::string vr);
	void SkipOtherDicomtag();
	void ReadImageData();
	DataElement* ReadDicomTag();
	void ReadTagValue(DataElement* dataElement);
};


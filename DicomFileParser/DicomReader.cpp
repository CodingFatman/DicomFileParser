#include "stdafx.h"
#include "DicomReader.h"


DicomReader::DicomReader()
{
}


DicomReader::~DicomReader()
{
}

void DicomReader::LoadDicomFile(std::string dicomFileName)
{
	infile.open(dicomFileName, std::ifstream::binary);

	infile.seekg(128);
	char cdicom[5]{};
	infile.read(cdicom, 4);

	IsValidDicomFile = true;
	if (strcmp(cdicom, "DICM") != 0)
	{
		IsValidDicomFile = false;
	}
	else
	{
		bool isSupportTransferSyntax = ReadFileMetaInfo();
		if (!isSupportTransferSyntax)
		{
			std::cout << "不支持的传输语法，目前仅支持显示VR小端字节序的传输语法";
			getchar();
		}
		else
		{
			SkipOtherDicomtag();
			ProcessPixelDataTag();
			DisplayDicomTagValue();
		}
	}
}

DataElement* DicomReader::ReadDicomTag()
{
	char group[3]{};
	infile.read(group, 2);

    unsigned char low = group[0];
	int groupLow = int(low);

	unsigned char high = group[1];
	int groupHigh = int(high);

	char tag[3]{};
	infile.read(tag, 2);

	low = tag[0];
	int tagLow = int(low);

	high = tag[1];
	int tagHigh = int(high);

	DataElement *element = new DataElement(groupHigh, groupLow, tagHigh, tagLow);
	return element;
}

void DicomReader::ReadTagValue(DataElement* dataElement)
{
	char vr[3]{};
	infile.read(vr, 2);
	std::string vrStr(vr);

	int valueLengthBytes = 2;
	if (IsReservedVR(vr))
	{
		valueLengthBytes = 4;
		char reserved[3];
		infile.read(reserved, 2);
	}
	char *valueLength = new char[valueLengthBytes + 1];
	infile.read(valueLength, valueLengthBytes);

	long long length = 0;
	if (valueLengthBytes == 2)
	{
		unsigned int byte2 = valueLength[1];
		unsigned int byte1 = valueLength[0];
		length = (byte2 << 8) + byte1;
	}
	else
	{
		unsigned int byte4 = valueLength[3];
		unsigned int byte3 = valueLength[2];
		unsigned int byte2 = valueLength[1];
		unsigned int byte1 = valueLength[0];
		length = (byte4 << 24) + (byte3 << 16) + (byte2 << 8) + byte1;
	}

	char *value = new char[length + 1]{};
	infile.read(value, length);
	value[length] = '\0';

	dataElement->value = value;
	dataElement->valueLength = length;
}

void DicomReader::ExtractTagValue()
{
	unsigned char low, high;

	char* strPatientName = dicomData["0010-0010"]->value;
	patientName = strPatientName;

	char* strSamplesPerPixel = dicomData["0028-0002"]->value;
	low = strSamplesPerPixel[0];
	high = strSamplesPerPixel[1];
	samplesPerPixel = (high << 8) + low;

	char* strPixelRepresentation = dicomData["0028-0103"]->value;
	low = strPixelRepresentation[0];
	high = strPixelRepresentation[1];
	pixelRepresentation = (high << 8) + low;

	char* strRows = dicomData["0028-0010"]->value;
	low = strRows[0];
	high = strRows[1];
	rows = (high << 8) + low;

	char* strColumns = dicomData["0028-0011"]->value;
	low = strColumns[0];
	high = strColumns[1];
	columns = (high << 8) + low;

	char* strBitsAllocated = dicomData["0028-0100"]->value;
	low = strBitsAllocated[0];
	high = strBitsAllocated[1];
	bitsAllocated = (high << 8) + low;

	char* strBitsStored = dicomData["0028-0101"]->value;
	low = strBitsStored[0];
	high = strBitsStored[1];
	bitsStored = (high << 8) + low;

	char* strHighBit = dicomData["0028-0102"]->value;
	low = strHighBit[0];
	high = strHighBit[1];
	highBit = (high << 8) + low;

	char* strRescaleIntercept = dicomData["0028-1052"]->value;
	rescaleIntercept = atof(strRescaleIntercept);

	char* strRescaleSlop = dicomData["0028-1053"]->value;
	rescaleSlop = atof(strRescaleSlop);

	char* strWindowCenter = dicomData["0028-1050"]->value;
	windowCenter = atoi(strWindowCenter);

	char* strWindowWidth = dicomData["0028-1051"]->value;
	windowWidth = atoi(strWindowWidth);
}

bool DicomReader::ReadFileMetaInfo()
{
	bool isSupportTransferSyntax = true;

	currentTag = ReadDicomTag();
	while (currentTag->group == "0002")
	{
		ReadTagValue(currentTag);
		dicomData[currentTag->tag] = currentTag;

		currentTag = ReadDicomTag();
	}

	char* strTransferSyntax = dicomData["0002-0010"]->value;
	transferSyntax = strTransferSyntax;

	if (transferSyntax.compare(CstExplicitVRLittleEndian) != 0)
	{
		isSupportTransferSyntax = false;
	}

	return isSupportTransferSyntax;
}

bool DicomReader::IsReservedVR(std::string vr)
{
	bool result = false;

	if (vr == "OB") result = true;
	if (vr == "OD") result = true;
	if (vr == "OF") result = true;
	if (vr == "OL") result = true;
	if (vr == "OW") result = true;
	if (vr == "SQ") result = true;
	if (vr == "UC") result = true;
	if (vr == "UR") result = true;
	if (vr == "UT") result = true;
	if (vr == "UN") result = true;

	return result;
}

void DicomReader::SkipOtherDicomtag()
{
	while(currentTag->tag != "7FE0-0010")
	{
		ReadTagValue(currentTag);
		dicomData[currentTag->tag] = currentTag;

		currentTag = ReadDicomTag();
	} 
}

void DicomReader::ProcessPixelDataTag()
{
	ExtractTagValue();
	if (pixelRepresentation == 0)
	{
		unsigned short* pixelData = ReadPixelData<unsigned short>();
		SavePixelDataTo8BitBmpFile(pixelData);
	}
	else
	{
		short* pixelData = ReadPixelData<short>();
		SavePixelDataTo8BitBmpFile(pixelData);
	}

	std::cout << patientName <<"的Dicom图像已经成功解析，并转存为BMP格式" << std::endl;
}

void DicomReader::DisplayDicomTagValue()
{
  std::cout << "0010-0010 patientName:" << patientName << std::endl;
  std::cout << "0028-0002 samplesPerPixel:" << samplesPerPixel << std::endl;
  std::cout << "0028-0103 pixelRepresentation:" << pixelRepresentation << std::endl;
  std::cout << "0028-0010 rows:" << rows << std::endl;
  std::cout << "0028-0011 columns:" << columns << std::endl;
  std::cout << "0028-0100 bitsAllocated:" << bitsAllocated << std::endl;
  std::cout << "0028-0101 bitsStored:" << bitsStored << std::endl;
  std::cout << "0028-0102 highBit:" << highBit << std::endl;
  std::cout << "0028-1052 rescaleIntercept:" << rescaleIntercept << std::endl;
  std::cout << "0028-1053 rescaleSlop:" << rescaleSlop << std::endl;
  std::cout << "0028-1050 windowCenter:" << windowCenter << std::endl;
  std::cout << "0028-1051 windowWidth:" << windowWidth << std::endl;
}
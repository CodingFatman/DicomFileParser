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
		ReadFileMetaInfo();
		SkipOtherDicomtag();
		ReadImageData();
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

	char *value = new char[length]{};
	infile.read(value, length);

	dataElement->value = value;
	dataElement->valueLength = length;
}

void DicomReader::ReadFileMetaInfo()
{
	currentTag = ReadDicomTag();
	while (currentTag->group == "0002")
	{
		ReadTagValue(currentTag);
		dicomData[currentTag->tag] = currentTag;

		currentTag = ReadDicomTag();
	}
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

void DicomReader::ReadImageData()
{
	unsigned char low, high;

	if (currentTag->tag != "7FE0-0010") return;

	ReadTagValue(currentTag);
	dicomData[currentTag->tag] = currentTag;

	char* strSamplesPerPixel = dicomData["0028-0002"]->value;
	low = strSamplesPerPixel[0];
	high = strSamplesPerPixel[1];
	int samplesPerPixel = (high << 8) + low;

	char* strPixelRepresentation = dicomData["0028-0103"]->value;
	low = strPixelRepresentation[0];
	high = strPixelRepresentation[1];
	int pixelRepresentation = (high << 8) + low;

	char* strRows = dicomData["0028-0010"]->value;
	low = strRows[0];
	high = strRows[1];
	int rows = (high << 8) + low;

	char* strColumns = dicomData["0028-0011"]->value;
	low = strColumns[0];
	high = strColumns[1];
	int columns = (high << 8) + low;

	char* strBitsAllocated = dicomData["0028-0100"]->value;
	low = strBitsAllocated[0];
	high = strBitsAllocated[1];
	int bitsAllocated = (high << 8) + low;

	char* strBitsStored = dicomData["0028-0101"]->value;
	low = strBitsStored[0];
	high = strBitsStored[1];
	int bitsStored = (high << 8) + low;

	char* strHighBit = dicomData["0028-0102"]->value;
	low = strHighBit[0];
	high = strHighBit[1];
	int highBit = (high << 8) + low;

	char* strRescaleIntercept = dicomData["0028-1052"]->value;
	float rescaleIntercept = atof(strRescaleIntercept);

	char* strRescaleSlop = dicomData["0028-1053"]->value;
	float rescaleSlop = atof(strRescaleSlop);

	char* strWindowCenter = dicomData["0028-1050"]->value;
	int windowCenter = atoi(strWindowCenter);

	char* strWindowWidth = dicomData["0028-1051"]->value;
	int windowWidth = atoi(strWindowWidth);

	DataElement *imageData = dicomData["7FE0-0010"];

	if (pixelRepresentation == 0)
	{
		//char low, high;

		char* strSmallestPixelValue = dicomData["0028-0106"]->value;
		low = strSmallestPixelValue[0];
		high = strSmallestPixelValue[1];
		short smallestPixelValue = (high << 8) + low;

		char* strLargestPixelValue = dicomData["0028-0107"]->value;
		low = strLargestPixelValue[0];
		high = strLargestPixelValue[1];
		short largestPixelValue = (high << 8) + low;

	    unsigned short *pixelData = new unsigned short[rows * columns]{};
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < columns; j++)
			{
				int index = i * columns + j;

				low = imageData->value[2 * index];
				high = imageData->value[2 * index + 1];
				unsigned short sourceValue = (high << 8) + low;
				pixelData[index] = sourceValue;
			}
		}

		//int maxValue = windowCenter + (windowWidth / 2);
		//int minValue = windowCenter - (windowWidth / 2);

		int maxValue = (2 * windowCenter + windowWidth) / 2.0 + 0.5;
		int minValue = (2 * windowCenter - windowWidth) / 2.0 + 0.5;
		unsigned char *bmpData = new unsigned char[rows * columns]{};
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < columns; j++)
			{
				unsigned short pixelValue = pixelData[i*columns + j];
				int hu = pixelValue * rescaleSlop + rescaleIntercept;

				//int displayValue = (hu / ((maxValue - minValue) * 1.0)) * 255;
				int displayValue = (hu-minValue)*255.0 / (double)(maxValue - minValue);
				//int displayValue = (pixelValue / ((maxValue - minValue) * 1.0)) * 255;

				if (displayValue > 255) displayValue = 255;

				if (displayValue < 0) displayValue = 0;

				bmpData[(rows - i - 1)*columns + j] = displayValue;
				if ((displayValue > 0) && (displayValue < 256))
				{
					//std::cout << index << ":" << displayValue << std::endl;
				}
			}
		}

		BmpFileHelper bmpHelper;
		bmpHelper.CreateBmpFile(columns, rows, bmpData, rows * columns);
	}
	
}

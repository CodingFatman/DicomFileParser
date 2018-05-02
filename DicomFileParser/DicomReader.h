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
	PixelType* ReadPixelData()
	{
		{
			if (currentTag->tag != "7FE0-0010") return NULL;

			ReadTagValue(currentTag);
			dicomData[currentTag->tag] = currentTag;

			unsigned char low, high;
			DataElement *imageData = dicomData["7FE0-0010"];
			PixelType *pixelData = new PixelType[rows * columns]{};
			for (int i = 0; i < rows; i++)
			{
				for (int j = 0; j < columns; j++)
				{
					int index = i * columns + j;

					low = imageData->value[2 * index];
					high = imageData->value[2 * index + 1];
					PixelType sourceValue = (high << 8) + low;
					pixelData[index] = sourceValue;
				}
			}

			return pixelData;
		}
	}

	template<typename PixelType>
	void SavePixelDataTo8BitBmpFile(PixelType* pixelData)
	{
		//int maxValue = windowCenter + (windowWidth / 2);
		//int minValue = windowCenter - (windowWidth / 2);
		int maxValue = (2 * windowCenter + windowWidth) / 2.0 + 0.5;
		int minValue = (2 * windowCenter - windowWidth) / 2.0 + 0.5;

		unsigned char *bmpData = new unsigned char[rows * columns]{};
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < columns; j++)
			{
				PixelType pixelValue = pixelData[i*columns + j];
				int hu = pixelValue * rescaleSlop + rescaleIntercept;

				//int displayValue = (hu / ((maxValue - minValue) * 1.0)) * 255;
				int displayValue = (hu - minValue)*255.0 / (double)(maxValue - minValue);

				if (displayValue > 255) displayValue = 255;

				if (displayValue < 0) displayValue = 0;

				bmpData[(rows - i - 1)*columns + j] = displayValue;
			}
		}

		BmpFileHelper bmpHelper;
		bmpHelper.CreateBmpFile(columns, rows, bmpData, rows * columns);
	}
};


#include "stdafx.h"
#include "DataElement.h"


DataElement::DataElement()
{
	value = "";
	valueLength = 0;
}

DataElement::DataElement(int groupHigh, int groupLow, int tagHigh, int tagLow):DataElement()
{
	char temp[10]{};
	std::stringstream dicomTag;

	sprintf_s(temp, "%02X", groupHigh);
	dicomTag << temp;

	memset(temp, 0, 10);
	sprintf_s(temp, "%02X", groupLow);
	dicomTag << temp;

	group = dicomTag.str();

	dicomTag.clear();
	dicomTag.str("");

	memset(temp, 0, 10);
	sprintf_s(temp, "%02X", tagHigh);
	dicomTag << temp;

	memset(temp, 0, 10);
	sprintf_s(temp, "%02X", tagLow);
	dicomTag << temp;

	element = dicomTag.str();

	tag = group + "-" + element;
}


DataElement::~DataElement()
{
}

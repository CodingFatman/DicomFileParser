#pragma once

#include <iostream>
#include <sstream>

class DataElement
{
private:
	DataElement();
public:
	DataElement(int groupHigh, int groupLow, int tagHigh, int tagLow);
	~DataElement();

	std::string tag;
	std::string group;
	std::string element;
	
	char* value;
	unsigned int valueLength;
};


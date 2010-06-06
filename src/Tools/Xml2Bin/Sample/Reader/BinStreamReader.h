#pragma once
#include <stdio.h>
#include "MengeTypes.h"


class BinStreamReader
{
private:
	FILE *binFile;
public:
	BinStreamReader(char* _filePath);
	~BinStreamReader(void);

	template<typename PODType>
	void read(PODType& value)
	{
		fread(&value, sizeof(PODType), 1, binFile);
	}
	void read(String& value);
};

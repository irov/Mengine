#include "BinStreamReader.h"

//////////////////////////////////////////////////////////////////////////
BinStreamReader::BinStreamReader(char* _filePath)
:binFile(NULL)
{
	binFile = fopen(_filePath, "rb");
}
//////////////////////////////////////////////////////////////////////////
BinStreamReader::~BinStreamReader(void)
{
	fclose(binFile);
}
//////////////////////////////////////////////////////////////////////////
void BinStreamReader::read( String& value )
{
	size_t length;
	read<size_t>(length);
	value = new char[length+1];
	value[length] = '\0';
	fread(value, sizeof(char), length, binFile);
}
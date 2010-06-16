#pragma once
#include "BinParsListener.h"
#include "BinStreamReader.h"

class BinParser
{
private:
	BinParsListener* listener;
	BinStreamReader* reader;
public:
	BinParser(BinStreamReader *_reader, BinParsListener* _listener);
	~BinParser();
private:
	void ReadBinFile();
	bool ReadNode();
	bool ReadAttribute();
};

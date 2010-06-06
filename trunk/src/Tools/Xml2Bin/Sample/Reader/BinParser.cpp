#include "BinParser.h"
//////////////////////////////////////////////////////////////////////////
BinParser::BinParser(BinStreamReader *_reader, BinParsListener *_listener)
{
	listener = _listener;
	reader = _reader;
	ReadBinFile();
}
//////////////////////////////////////////////////////////////////////////
BinParser::~BinParser()
{
	
}
//////////////////////////////////////////////////////////////////////////
void BinParser::ReadBinFile()
{
	listener->DocumentStart();

	ReadNode();

	listener->DocumentEnd();
}
//////////////////////////////////////////////////////////////////////////
bool BinParser::ReadNode()
{
	size_t nodeID;
	reader->read(nodeID);

	listener->NodeStart(nodeID);
	
	int attrCount;
	reader->read(attrCount);
	for(int i=0;i<attrCount;++i)
	{
		if(!ReadAttribute())
			return false;
	}
	
	int subNodeCount;
	reader->read(subNodeCount);
	for(int i=0;i<subNodeCount;++i)
	{
		if(!ReadNode())
			return false;
	}

	listener->NodeEnd(nodeID);

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool BinParser::ReadAttribute()
{
	size_t attrID;
	reader->read(attrID);
	return listener->AddAttribute(attrID, reader);
}
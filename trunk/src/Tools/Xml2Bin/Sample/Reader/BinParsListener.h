#pragma once
#include <stdio.h>
#include "BinStreamReader.h"
#include "protocol.h"
#include <string>
class BinParsListener
{
private:
	std::string tabulation;
public:
	BinParsListener();
	~BinParsListener();

	void DocumentStart();
	void DocumentEnd();
	void NodeStart(size_t _nodeID);
	bool AddAttribute(size_t _attrID, BinStreamReader* _reader);
	void NodeEnd(size_t _nodeID);
};
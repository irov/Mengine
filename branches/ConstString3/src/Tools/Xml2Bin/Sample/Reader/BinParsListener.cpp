#include "BinParsListener.h"
//////////////////////////////////////////////////////////////////////////
BinParsListener::BinParsListener()
:tabulation("")
{
}
//////////////////////////////////////////////////////////////////////////
BinParsListener::~BinParsListener()
{
}
//////////////////////////////////////////////////////////////////////////
void BinParsListener::DocumentStart()
{
	printf("Document Started\n");
}
//////////////////////////////////////////////////////////////////////////
void BinParsListener::DocumentEnd()
{
	printf("Document Ended\n");
}
//////////////////////////////////////////////////////////////////////////
void BinParsListener::NodeStart( size_t _nodeID )
{
	tabulation += " ";
	printf("%sNode Start: %d\n", tabulation.c_str(), _nodeID);
}
//////////////////////////////////////////////////////////////////////////
void BinParsListener::NodeEnd( size_t _nodeID )
{
	printf("%sNode End: %d\n", tabulation.c_str(), _nodeID);
	tabulation.erase(tabulation.length()-1);
}
//////////////////////////////////////////////////////////////////////////
bool BinParsListener::AddAttribute( size_t _attrID, BinStreamReader* _reader )
{
	printf("%sAttribute: %d\n", tabulation.c_str(), _attrID);

	String value = NULL;
	bool res = true;
	switch(_attrID)
	{
	case Menge::Protocol::Resource_Name::id:
		_reader->read(value);
		break;
	case Menge::Protocol::Resource_Type::id:
		_reader->read(value);
		break;
	case Menge::Protocol::File_Path::id:
		_reader->read(value);
		break;
	case Menge::Protocol::Folder_Path::id:
		_reader->read(value);
		break;
	case Menge::Protocol::Fontdef_Path::id:
		_reader->read(value);
		break;
	case Menge::Protocol::Image_Path::id:
		_reader->read(value);
		break;
	default:
		printf("Wrong id %d\n", _attrID);
		res = false;
	    break;
	}
	
	if(res)
		printf("%sValue: %s\n", tabulation.c_str(), value);

	if(value != NULL)
	{
		delete [] value;
	}

	return res;
}
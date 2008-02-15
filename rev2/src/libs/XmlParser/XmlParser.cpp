#	include "XmlParser.h"
#	include "XmlExpatParser.h"

//////////////////////////////////////////////////////////////////////////
XmlParser::XmlParser()
{
}
//////////////////////////////////////////////////////////////////////////
XmlParser::~XmlParser()
{
}
//////////////////////////////////////////////////////////////////////////
XmlExpatParser * XmlParser::newParser()
{
	return new XmlExpatParser;
}
//////////////////////////////////////////////////////////////////////////
void XmlParser::deleteParser( XmlExpatParser * _parser )
{
	delete _parser;
}
//////////////////////////////////////////////////////////////////////////
void * XmlParser::makeBuffer( XmlExpatParser * _parser, size_t _size )
{
	return _parser->makeBuffer( _size );
}
//////////////////////////////////////////////////////////////////////////
bool XmlParser::parseBuffer( XmlExpatParser * _parser, size_t _size, XmlElementListener * _listener )
{
	return _parser->parseXML( _size, _listener );
}
#	pragma once

#	include "Config/Typedef.h"

//#	ifdef MENGE_UNICODE
//#	define XML_UNICODE_WCHAR_T
//#	endif

#	include "expat.h"
#	include <list>

class XmlElementListener;
class XmlElementValueListener;

class XmlExpatParser
{
public:
	XmlExpatParser();
	~XmlExpatParser();

public:
	void * makeBuffer( std::size_t _size );
	bool parseXML( std::size_t _size, XmlElementListener * _listener );	
	bool parseStatus();

public:
	void pushListener( XmlElementListener * _listener );
	XmlElementListener * topListener();
	void popListener();
	void clearListener();

	void setValueListener( XmlElementValueListener * _listener );
	void callValueListener( const Menge::TChar * _value, int _len );

protected:
	typedef std::list<XmlElementListener *> TListStackListener;
	TListStackListener m_listStackListener;

	XmlElementValueListener * m_valueListener;

	XML_Parser m_parser;

	static int s_unknownEncodingHandler( void *encodingHandlerData,	const XML_Char *name, XML_Encoding *info);
};

#	pragma once

#	include "Config/Typedef.h"

//#	ifdef MENGE_UNICODE
//#	define XML_UNICODE_WCHAR_T
//#	endif

#	include "expat.h"
#	include <list>

class XmlElementListener;
class XmlElementValueListener;

namespace Menge
{
	class ConstManager;
}

class XmlExpatParser
{
public:
	XmlExpatParser( Menge::ConstManager * _constManager );
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

	Menge::ConstManager * getConstManager();

protected:
	typedef std::list<XmlElementListener *> TListStackListener;
	TListStackListener m_listStackListener;

	XmlElementValueListener * m_valueListener;

	Menge::ConstManager * m_constManager;

	XML_Parser m_parser;

	static int s_unknownEncodingHandler( void *encodingHandlerData,	const XML_Char *name, XML_Encoding *info);
};

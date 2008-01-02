#	pragma once

#	include "expat.h"
#	include <list>

class XmlListener;

class XmlExpatParser
{
public:
	XmlExpatParser();
	~XmlExpatParser();

public:
	void * makeBuffer( int _size );
	bool parseXML( int _size, XmlListener * _listener );	

public:
	void pushListener( XmlListener * _listener );
	XmlListener * topListener();
	void popListener();
	void clearListener();

protected:
	typedef std::list<XmlListener *> TListStackListener;
	TListStackListener m_listStackListener;

	XML_Parser m_parser;
};

#	pragma once

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
	void * makeBuffer( unsigned int _size );
	bool parseXML( unsigned int _size, XmlElementListener * _listener );	
	bool parseStatus();

public:
	void pushListener( XmlElementListener * _listener );
	XmlElementListener * topListener();
	void popListener();
	void clearListener();

	void setValueListener( XmlElementValueListener * _listener );
	void callValueListener( const char * _value, int _len );

protected:
	typedef std::list<XmlElementListener *> TListStackListener;
	TListStackListener m_listStackListener;

	XmlElementValueListener * m_valueListener;

	XML_Parser m_parser;
};

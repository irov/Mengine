#	pragma once

#	include <list>

class XmlElementListener;
class XmlElementValueListener;

class XmlExpatParser
{
public:
	XmlExpatParser();
	~XmlExpatParser();

public:
	void * makeParser();
	void * makeBuffer( void * _parser, size_t _size );
	bool parseXML( void * _parser, size_t _size, XmlElementListener * _listener );	

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
};

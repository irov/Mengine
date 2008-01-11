#	pragma once

class XmlExpatParser;
class XmlElementListener;
class XmlElementValueListener;

class XmlElement
{
public:
	XmlElement( XmlExpatParser * _manager, const char * _title, const char ** _attrebutes );

public:
	const char * getTitle() const;

public:
	bool compareTitle( const char * _title );
	bool compareAttributeKey( const char * _title );

	void pushListener( XmlElementListener * _listener );
	void setValueListener( XmlElementValueListener * _listener );

public:
	void beginAttributes();
	bool validAttributes();
	void nextAttributes();

	const char * getAttributeValue();

protected:
	XmlExpatParser * m_expat;

	const char * m_title;
	const char ** m_attrebutes;
	size_t m_attrIterator;
};
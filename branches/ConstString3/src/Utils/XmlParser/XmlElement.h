#	pragma once

#	include "Config/Typedef.h"
#	include "../Core/ConstString.h"

class XmlExpatParser;
class XmlElementListener;
class XmlElementValueListener;

class XmlElement
{
public:
	XmlElement( XmlExpatParser * _manager, const Menge::TChar * _title, const Menge::TChar ** _attrebutes );

public:
	const Menge::TChar * getTitle() const;

public:
	bool compareTitle( const Menge::TChar * _title );
	bool compareAttributeKey( const Menge::TChar * _title );

	void pushListener( XmlElementListener * _listener );
	void setValueListener( XmlElementValueListener * _listener );

	Menge::ConstString getString( const Menge::String & _value ); 

public:
	void beginAttributes();
	bool validAttributes();
	void nextAttributes();

	const Menge::TChar * getAttributeValue();

protected:
	XmlExpatParser * m_expat;

	const Menge::TChar * m_title;
	const Menge::TChar ** m_attrebutes;
	unsigned int m_attrIterator;
};

#	include "XmlElement.h"
#	include "XmlExpatParser.h"

#	include <string.h>

//////////////////////////////////////////////////////////////////////////
XmlElement::XmlElement( XmlExpatParser * _expat, const Menge::TCharA * _title, const Menge::TCharA ** _attrebutes )
: m_expat(_expat)
, m_title(_title)
, m_attrebutes(_attrebutes)
, m_attrIterator(0)
{		
}
//////////////////////////////////////////////////////////////////////////
const Menge::TCharA * XmlElement::getTitle() const
{
	return m_title;
}
//////////////////////////////////////////////////////////////////////////
bool XmlElement::compareTitle( const Menge::TCharA * _title )
{
	return std::strcmp( m_title, _title ) == 0;
}
//////////////////////////////////////////////////////////////////////////
bool XmlElement::compareAttributeKey( const Menge::TCharA * _name )
{
	return std::strcmp( m_attrebutes[ m_attrIterator ], _name ) == 0;
}
//////////////////////////////////////////////////////////////////////////
void XmlElement::pushListener( XmlElementListener * _listener )
{
	m_expat->pushListener(_listener);
}
//////////////////////////////////////////////////////////////////////////
void XmlElement::setValueListener( XmlElementValueListener * _listener )
{
	m_expat->setValueListener( _listener );
}
//////////////////////////////////////////////////////////////////////////
void XmlElement::getString( Menge::ConstString & _out, const Menge::String & _value )
{
	Menge::ConstManager * constManager = 
		m_expat->getConstManager();

	_out = constManager->getString( _value );
}
//////////////////////////////////////////////////////////////////////////
void XmlElement::beginAttributes()
{
	m_attrIterator = 0;
}
//////////////////////////////////////////////////////////////////////////
bool XmlElement::validAttributes()
{
	return m_attrebutes[m_attrIterator] != 0;
}
//////////////////////////////////////////////////////////////////////////
void XmlElement::nextAttributes()
{
	m_attrIterator += 2;
}
//////////////////////////////////////////////////////////////////////////
const Menge::TCharA * XmlElement::getAttributeValue()
{
	return m_attrebutes[ m_attrIterator + 1 ];
}

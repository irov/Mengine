#	include "XmlElement.h"
#	include "XmlExpatParser.h"

#	include <string.h>

//////////////////////////////////////////////////////////////////////////
XmlElement::XmlElement( XmlExpatParser * _expat, const Menge::TChar * _title, const Menge::TChar ** _attrebutes )
: m_expat(_expat)
, m_title(_title)
, m_attrebutes(_attrebutes)
, m_attrIterator(0)
{		
}
//////////////////////////////////////////////////////////////////////////
const Menge::TChar * XmlElement::getTitle() const
{
	return m_title;
}
//////////////////////////////////////////////////////////////////////////
bool XmlElement::compareTitle( const Menge::TChar * _title )
{
	return STDSTRCMP( m_title, _title ) == 0;
}
//////////////////////////////////////////////////////////////////////////
bool XmlElement::compareAttributeKey( const Menge::TChar * _name )
{
	return STDSTRCMP( m_attrebutes[ m_attrIterator ], _name ) == 0;
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
const Menge::TChar * XmlElement::getAttributeValue()
{
	return m_attrebutes[ m_attrIterator + 1 ];
}

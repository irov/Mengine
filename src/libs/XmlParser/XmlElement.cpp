#	include "XmlElement.h"
#	include "XmlExpatParser.h"

#	include <string.h>

//////////////////////////////////////////////////////////////////////////
XmlElement::XmlElement( XmlExpatParser * _expat, const char * _title, const char ** _attrebutes )
: m_expat(_expat)
, m_title(_title)
, m_attrebutes(_attrebutes)
, m_attrIterator(0)
{		
}
//////////////////////////////////////////////////////////////////////////
const char * XmlElement::getTitle() const
{
	return m_title;
}
//////////////////////////////////////////////////////////////////////////
bool XmlElement::compareTitle( const char * _title )
{
	return strcmp( m_title, _title ) == 0;
}
//////////////////////////////////////////////////////////////////////////
bool XmlElement::compareAttributeKey( const char * _name )
{
	return strcmp( m_attrebutes[ m_attrIterator ], _name ) == 0;
}
//////////////////////////////////////////////////////////////////////////
void XmlElement::pushListener( XmlListener * _listener )
{
	m_expat->pushListener(_listener);
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
const char * XmlElement::getAttributeValue()
{
	return m_attrebutes[ m_attrIterator + 1 ];
}

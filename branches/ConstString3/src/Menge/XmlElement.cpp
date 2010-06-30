#	include "XmlElement.h"
#	include "XmlExpatParser.h"

#	include "Core/ConstString.h"

#	include <string.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	XmlElement::XmlElement( XmlExpatParser * _expat, const TCharA * _title, const TCharA ** _attrebutes )
		: m_expat(_expat)
		, m_title(_title)
		, m_attrebutes(_attrebutes)
		, m_attrIterator(0)
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	const TCharA * XmlElement::getTitle() const
	{
		return m_title;
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlElement::compareTitle( const TCharA * _title )
	{
		return std::strcmp( m_title, _title ) == 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlElement::compareAttributeKey( const TCharA * _name )
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
	const TCharA * XmlElement::getAttributeValue()
	{
		return m_attrebutes[ m_attrIterator + 1 ];
	}
}
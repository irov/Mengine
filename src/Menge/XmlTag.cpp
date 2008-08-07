#	include "XmlTag.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	XmlTag::XmlTag()
		: m_tag(TAG_BEGIN)
		, m_name(0)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void XmlTag::setup( XMLTag _tag, const char * _name )
	{
		m_tag = _tag;
		m_name = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	XmlTag & XmlTag::operator () ( XMLAttribute _attr, XMLType _type )
	{
		if( _attr >= m_tagAttributes.size() )
		{
			m_tagAttributes.resize( _attr );
		}

		TTagAttribute & attr = m_tagAttributes[ _attr ];

		attr.first = _attr;
		attr.second = _type;

		return *this;
	}
}
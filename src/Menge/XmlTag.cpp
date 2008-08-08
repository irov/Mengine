#	include "XmlTag.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	XmlTag::XmlTag()
		: m_tag(TAG_BEGIN)
	{
		m_tagAttributes.resize( ATTR_LAST );
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlTag::setup( XMLTag _tag, const std::string & _name )
	{
		m_tag = _tag;
		m_name = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	XmlTag & XmlTag::operator () ( XMLAttribute _attr, XMLType _type )
	{
		TTagAttribute & attr = m_tagAttributes[ _attr ];

		attr.first = _attr;
		attr.second = _type;

		return *this;
	}
}
#	include "XmlTag.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	XmlTag::XmlTag( XMLTag _tag, const String & _name, XmlTag * _parent )
		: m_tag(_tag)
		, m_name(_name)
		, m_parent(_parent)
	{
		TagAttribute attrInvalid;

		attrInvalid.attrId = ATTR_BEGIN;
		attrInvalid.attrType = TYPE_BEGIN;
		attrInvalid.valid = false;

		m_tagAttributes.resize( ATTR_LAST, attrInvalid );

		m_tags.resize( TAG_LAST, 0 );

		if( _parent )
		{
			_parent->tagChild_( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	XMLTag XmlTag::getId() const
	{
		return m_tag;
	}
	//////////////////////////////////////////////////////////////////////////
	const TagAttribute &  XmlTag::getAttribute( XMLAttribute _attr ) const
	{
		const TagAttribute & attr = m_tagAttributes[ _attr ];

		return attr;
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlTag::tagChild_( XmlTag * _child )
	{
		XMLTag tagId = _child->getId();
		m_tags[ tagId ] = _child;
	}
	//////////////////////////////////////////////////////////////////////////
	XmlTag * XmlTag::attribute( XMLAttribute _attr, XMLType _type )
	{
		TagAttribute & attr = m_tagAttributes[ _attr ];

		attr.attrId = _attr;
		attr.attrType = _type;
		attr.valid = true;

		return this;
	}
	//////////////////////////////////////////////////////////////////////////
	XmlTag * XmlTag::getTag( XMLTag _tagId ) const
	{
		return m_tags[ _tagId ];
	}
}
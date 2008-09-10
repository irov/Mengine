#	pragma once

#	include "Config/Typedef.h"

#	include "XmlEnum.h"

#	include <vector>

namespace Menge
{
	struct TagAttribute
	{
		XMLAttribute attrId;
		XMLType attrType;
		bool valid;
	};

	class XmlTag
	{
	public:
		XmlTag( XMLTag _tag, const String & _name, XmlTag * _parent );

	public:
		XmlTag * attribute( XMLAttribute _attr, XMLType _type );

	public:
		XMLTag getId() const;
		const TagAttribute & getAttribute( XMLAttribute _attr ) const;

		XmlTag * getTag( XMLTag _tagId ) const;

	protected:
		XMLTag m_tag;
		String m_name;

		XmlTag * m_parent;

		typedef std::vector<TagAttribute> TTagAttributes;
		TTagAttributes m_tagAttributes;

		typedef std::vector<XmlTag *> TVectorTags;
		TVectorTags m_tags;

	private:
		void tagChild_( XmlTag * _child );
	};
}
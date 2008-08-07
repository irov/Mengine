#	pragma once

#	include "XmlEnum.h"

#	include <vector>

namespace Menge
{
	class XmlTag
	{
	public:
		XmlTag();

	public:
		void setup( XMLTag _tag, const char * _name );

	public:
		XmlTag & operator () ( XMLAttribute _attr, XMLType _type );

	protected:
		XMLTag m_tag;
		const char * m_name;

		typedef std::pair<XMLAttribute, XMLType> TTagAttribute;
		typedef std::vector<TTagAttribute> TTagAttributes;
		TTagAttributes m_tagAttributes;
	};
}
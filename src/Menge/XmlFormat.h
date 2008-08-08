#	pragma once

#	include "XmlTag.h"

#	include <map>
#	include <string>

namespace Menge
{
	class XmlFormat
	{
	public:
		XmlFormat();

	public:
		XmlTag & tag( XMLTag _tag, const std::string & _name );

		void define( XMLDefine _def, const std::string & _name );
		void attribute( XMLAttribute _attr, const std::string & _name );
		void type( XMLType _type, const std::string & _name );

	protected:
		typedef std::map<std::string, XMLDefine> TMapDefines;
		TMapDefines m_mapDefines;

		typedef std::map<std::string, XMLAttribute> TMapAttributes;
		TMapAttributes m_mapAttributes;

		typedef std::map<std::string, XMLType> TMapTypes;
		TMapTypes m_mapTypes;
		
		typedef std::vector<XmlTag> TTags;
		TTags m_tags;

		typedef std::map<std::string, XMLTag> TMapTags;
		TMapTags m_mapTags;
	};
}
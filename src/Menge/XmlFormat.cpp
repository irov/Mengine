#	include "XmlFormat.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void XmlFormat::define( XMLDefine _def, const std::string & _name )
	{
		m_mapDefines[ _name ] = _def;
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlFormat::attribute( XMLAttribute _attr, const std::string & _name )
	{
		m_mapAttributes[ _name ] = _attr;
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlFormat::type( XMLType _type, const std::string & _name )
	{
		m_mapTypes[ _name ] = _type;
	}
	//////////////////////////////////////////////////////////////////////////
	XmlTag & XmlFormat::tag( XMLTag _tag, const std::string & _name )
	{
		if( _tag >= m_tags.size() )
		{
			m_tags.resize( _tag );
		}

		XmlTag & tag = m_tags[ _tag ];

		tag.setup( _tag, _name );

		m_mapTags.insert( std::make_pair( _name, _tag ) );

		return tag;
	}
}
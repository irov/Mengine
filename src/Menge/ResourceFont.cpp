#	include "ResourceFont.h"

#	include "RenderEngine.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceFont::ResourceFont( const std::string & _name )
		: ResourceImpl( _name )
		, m_font(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::loader( TiXmlElement * _xml )
	{
		ResourceImpl::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "Font", "Name", m_fontName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceFont::_compile()
	{
		m_font = Holder<RenderEngine>::hostage()->loadFont( m_fontName );

		if( m_font == 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::_release()
	{
		Holder<RenderEngine>::hostage()->releaseRenderFont( m_font );
		m_font = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderFontInterface * ResourceFont::getFont() const
	{
		return m_font;
	}
}
#	include "ResourceFont.h"
#	include "ResourceImplement.h"

#	include "RenderEngine.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceFont )
	//////////////////////////////////////////////////////////////////////////
	ResourceFont::ResourceFont( const std::string & _name )
		: ResourceImpl( _name )
		, m_font( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::loader( TiXmlElement * _xml )
	{
		ResourceImpl::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "File", "Path", m_fontName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceFont::_compile()
	{
		m_font = Holder<RenderEngine>::hostage()->loadFont( m_fontName );

		if( m_font == 0 )
		{
			printf("Warning: resource font '%s' not find fond file '%s'\n"
				, m_name.c_str()
				, m_fontName.c_str() 
				);

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
	float ResourceFont::getCharWidth( char _id ) const
	{
		if( m_font )
		{
			return m_font->getCharWidth( _id );
		}

		return 0.f;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderFontInterface * ResourceFont::getFont() const
	{
		return m_font;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceFont::getHeight() const
	{
		if( m_font )
		{
			return m_font->getHeight();
		}

		return 0.f;	
	}
}
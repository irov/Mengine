#	include "ResourceFont.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "FileEngine.h"

#	include "LogEngine.h"
#	include "Utils.h"

#	include "RenderEngine.h"
#	include "Texture.h"

#	include <cstdio>
#	include <string.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceFont );
	//////////////////////////////////////////////////////////////////////////
	ResourceFont::ResourceFont()
		: m_image( NULL )
		, m_outline( NULL )
		, m_whsRatio( 3.0f )
		, m_textureRatio( 1.0f )
		, m_imageInvSize( 0.0f, 0.0f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			//XML_CASE_ATTRIBUTE_NODE_METHOD( "File", "Path", &ResourceFont::setFontPath );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Fontdef", "Path", &ResourceFont::setFontdefPath_ );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Image", "Path", &ResourceFont::setImagePath_ );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "OutlineImage", "Path", &ResourceFont::setOutlineImagePath_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* ResourceFont::getImage()
	{
		return m_image;
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* ResourceFont::getOutlineImage()
	{
		return m_outline;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceFont::getUV( unsigned int _id ) const
	{
		TMapGlyph::const_iterator it = m_glyphs.find( _id );
		return it != m_glyphs.end() ? it->second.uv : mt::vec4f::zero_v4;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::setGlyph( unsigned int _id, const mt::vec4f& _uv, const mt::vec2f& _offset, float _ratio, const mt::vec2f& _size )
    {
		TMapGlyph::iterator it = m_glyphs.find( _id );

		if ( it != m_glyphs.end() )
		{
			it->second.uv = _uv;
			it->second.offset = _offset;
			it->second.ratio = _ratio;
			it->second.size = _size;
		}
		else
		{
			Glyph gl( _id, _uv, _offset, _ratio, _size );
		
			m_glyphs.insert( 
				std::make_pair( _id, gl ) 
				);
		}
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceFont::_compile()
	{
		m_image = Holder<RenderEngine>::hostage()
					->loadTexture( m_params.category, m_imageFile );

		if( m_image == NULL )
		{
			MENGE_LOG_ERROR( "Error while loading font image '%s'"
				, m_imageFile.c_str() 
				);

			return false;
		}

		m_textureRatio = static_cast<float>( m_image->getWidth() ) / m_image->getHeight();

		m_imageInvSize.x = 1.0f / m_image->getWidth();
		m_imageInvSize.y = 1.0f / m_image->getHeight();

		if( m_outlineImageFile.empty() == false )
		{
			m_outline = Holder<RenderEngine>::hostage()
							->loadTexture( m_params.category, m_outlineImageFile );


			if( m_outline == 0 )
			{
				MENGE_LOG_ERROR( "Image can't loaded '%s'"
					, m_outlineImageFile.c_str() 
					);
			}
		}

		if( Holder<XmlEngine>::hostage()
			->parseXmlFileM( m_params.category, m_fontdefFile, this, &ResourceFont::loaderFontdef_ ) == false )
		{
			MENGE_LOG_ERROR( "Problems parsing fondef '%s'"
				, m_fontdefFile.c_str() 
				);

			return false;
		}
		m_whsRatio = getCharRatio('A');

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::_release()
	{
		Holder<RenderEngine>::hostage()
			->releaseTexture( m_image );
		m_image = NULL;

		if( m_outline )
		{
			Holder<RenderEngine>::hostage()
				->releaseTexture( m_outline );
			m_outline = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceFont::getCharRatio( unsigned int _id ) const
	{
		TMapGlyph::const_iterator it = m_glyphs.find(_id);

		if( it == m_glyphs.end() )
		{
			return m_whsRatio;
		}

		return it->second.ratio;
	}
	//////////////////////////////////////////////////////////////////////////
	String ResourceFont::getFontDir( const String& _fontName )
	{
		String::size_type index = _fontName.find_last_of( "/" );

		String fontDir = _fontName;

		if( index != std::string::npos )
		{
			fontDir = fontDir.substr( 0, index + 1 );
		}

		return fontDir;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceFont::parseAttribute( const String& name, const String& params )
	{
		/*if ( name == "glyph" )
		{
			String::size_type pos = params.find( ' ', 1 );
			const char* glyph = params.substr( 0, pos ).c_str();
			uint32 uiGlyph = *((unsigned int*)(glyph));
			uint32 clearBits[4] = { 0x000000FF, 0x0000FFFF, 0x00FFFFFF, 0xFFFFFFFF };
			std::size_t len = strlen( glyph );
			uiGlyph &= clearBits[len-1];
			String params_ = params.substr( pos );
			float u1, v1, u2, v2;

			int err = std::sscanf( params_.c_str(), "%f %f %f %f", &u1, &v1, &u2, &v2 );

			if (err == 0)
			{
				MENGE_LOG_ERROR( "in parsing params: '%s'"
					, params.c_str() );
				return false;
			}

			setGlyph( uiGlyph, u1, v1, u2, v2 );
		}
		else if ( name == "source" )
		{
			m_fullname = m_fontDir + params;

			m_image = Holder<RenderEngine>::hostage()->loadImage( m_fullname, 1 );

			if( m_image == 0 )
			{
				MENGE_LOG_ERROR( "Image can't loaded '%s'"
					, m_fullname.c_str() );
				return false;
			}
		}
		else if ( name == "outline" )
		{
			m_fullname = m_fontDir + params;

			m_outline = Holder<RenderEngine>::hostage()->loadImage( m_fullname, 1 );

			if( m_outline == 0 )
			{
				MENGE_LOG_ERROR( "Image can't loaded '%s'"
					, m_fullname.c_str() );
			}
		}*/

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceFont::parseFontdef( DataStreamInterface * _stream )
	{
		/*String line = Utils::getLine( _stream );
		Utils::skipLine( _stream,  "{" );
		
		while( !_stream->eof() )
        {
			line = Utils::getLine( _stream );

			if( line.length() == 0 || line.substr( 0, 2 ) == "//" )
            {
                continue;
            }

			String::size_type start = line.find_first_of( "\t\n " );
			String name = line.substr( 0, start );
			String value = line.substr( start + 1, line.size() );
		
			bool result = parseAttribute( name, value );

			if( result == false )
			{
				return false;
			}
		}*/

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceFont::getInitSize() const
	{
		return m_initSize;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::loaderFontdef_( XmlElement* _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Font" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{					
					XML_CASE_ATTRIBUTE( "height", m_initSize );
				}
			}
			XML_CASE_NODE( "Char" )
			{
				String glyph, rect, offset;
				int width = 0;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "width", width );
					XML_CASE_ATTRIBUTE( "code", glyph );
					XML_CASE_ATTRIBUTE( "rect", rect );
					XML_CASE_ATTRIBUTE( "offset", offset );
				}
				addGlyph_( glyph, rect, offset, width );
			}
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::addGlyph_( const String& _glyph, const String& _rect, const String& _offset, int _width )
	{
		const char* glyph = _glyph.c_str();
		uint32 uiGlyph = *((unsigned int*)(glyph));
		uint32 clearBits[4] = { 0x000000FF, 0x0000FFFF, 0x00FFFFFF, 0xFFFFFFFF };
		std::size_t len = strlen( glyph );
		uiGlyph &= clearBits[len-1];

		int a, b, c, d, ox = 0, oy = 0;
		int err = sscanf( _rect.c_str(), "%d %d %d %d", &a, &b, &c, &d );

		if (err == 0)
		{
			MENGE_LOG_ERROR( "Error parsing params: '%s'"
				, _rect.c_str() 
				);
		}

		if( _offset.empty() == false )
		{
			err = sscanf( _offset.c_str(), "%d %d", &ox, &oy );

			if (err == 0)
			{
				MENGE_LOG_ERROR( "Error parsing params: '%s'"
					, _offset.c_str() 
					);
			}
		}


		mt::vec4f uv( a * m_imageInvSize.x, b * m_imageInvSize.y, (a+c)*m_imageInvSize.x, (b+d)*m_imageInvSize.y );
		mt::vec2f offset( ox, oy );
		mt::vec2f size( c, d );
		setGlyph( uiGlyph, uv, offset, _width / m_initSize, size );

		//setGlyph( uiGlyph, a * fontWInv, b * fontHInv, (a + _width - 1) * fontWInv, (b + m_initSize) * fontHInv );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::setFontdefPath_( const String& _path )
	{
		m_fontdefFile = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::setImagePath_( const String& _path )
	{
		m_imageFile = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::setOutlineImagePath_( const String& _path )
	{
		m_outlineImageFile = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& ResourceFont::getFontdefPath() const
	{
		return m_fontdefFile;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& ResourceFont::getImagePath() const
	{
		return m_imageFile;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& ResourceFont::getOutlineImagePath() const
	{
		return m_outlineImageFile;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& ResourceFont::getOffset( unsigned int _char ) const
	{
		TMapGlyph::const_iterator it = m_glyphs.find( _char );

		if( it == m_glyphs.end() )
		{
			return mt::vec2f::zero_v2;
		}

		return it->second.offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& ResourceFont::getSize( unsigned int _char ) const
	{
		TMapGlyph::const_iterator it = m_glyphs.find( _char );

		if( it == m_glyphs.end() )
		{
			return mt::vec2f::zero_v2;
		}

		return it->second.size;
	}
	//////////////////////////////////////////////////////////////////////////
}

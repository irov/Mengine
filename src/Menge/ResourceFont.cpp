#	include "ResourceFont.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

#	include "FileEngine.h"

#	include "LogEngine.h"
#	include "Utils.h"
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceFont );
	//////////////////////////////////////////////////////////////////////////
	ResourceFont::ResourceFont( const ResourceFactoryParam & _params )
		: ResourceReference( _params )
		, m_image( 0 )
		, m_outline( 0 )
		, m_whsRatio( 3.0f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::setFontPath( const String& _path )
	{
		m_filename = m_params.category + _path;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "File", "Path", &ResourceFont::setFontPath );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceFont::getImage() const
	{
		return m_image;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceFont::getOutlineImage() const
	{
		return m_outline;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceFont::getUV( unsigned int _id ) const
	{
		TMapGlyph::const_iterator it = m_glyphs.find( _id );
		return it != m_glyphs.end() ? it->second.rect : mt::vec4f::zero_v4;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::setGlyph( unsigned int _id, float _u1, float _v1, float _u2, float _v2 )
    {
		TMapGlyph::iterator it = m_glyphs.find( _id );

		float ratio = ( _u2 - _u1 )  / ( _v2 - _v1 );
		
		mt::vec4f rect( _u1, _v1, _u2, _v2 );

		if ( it != m_glyphs.end() )
		{
			it->second.rect = rect; 
			it->second.ratio = ratio;
		}
		else
		{
			Glyph gl( _id, rect, ratio );
		
			m_glyphs.insert( 
				std::make_pair( _id, gl ) 
				);
		}
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceFont::_compile()
	{
		DataStreamInterface * stream = Holder<FileEngine>::hostage()->openFile( m_filename );

		if( stream == 0 )
		{
			MENGE_LOG_ERROR << "Warning: resource font not find fond file " << m_filename;
			return false;
		}

		m_fontDir = getFontDir( m_filename );

		bool result = parseFontdef( stream );

		if( result == false )
		{
			return false;
		}
		
		m_whsRatio = getCharRatio('A');

		TMapGlyph::const_iterator it = m_glyphs.find('A');
		mt::vec4f rect = it->second.rect;
		m_initSize = ::floorf( ( rect[3] - rect[1] ) * m_image->getHeight() );

		Holder<FileEngine>::hostage()->closeStream( stream );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::_release()
	{
		Holder<RenderEngine>::hostage()->releaseImage( m_image );
		m_image = 0;

		if( m_outline )
		{
			Holder<RenderEngine>::hostage()->releaseImage( m_outline );
			m_outline = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceFont::getCharRatio( TChar _id ) const
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
		if ( name == "glyph" )
		{
			TChar r;
			float u1, v1, u2, v2;

			int err = std::sscanf( params.c_str(), "%c %f %f %f %f", &r, &u1, &v1, &u2, &v2 );

			if (err == 0)
			{
				MENGE_LOG_ERROR << "in parsing params: " << params;
				return false;
			}

			setGlyph( r, u1, v1, u2, v2 );
		}
		else if ( name == "source" )
		{
			m_fullname = m_fontDir + params;

			m_image = Holder<RenderEngine>::hostage()->loadImage( m_fullname, 1 );

			if( m_image == 0 )
			{
				MENGE_LOG_ERROR << "Image can't loaded " << m_fullname;
				return false;
			}
		}
		else if ( name == "outline" )
		{
			m_fullname = m_fontDir + params;

			m_outline = Holder<RenderEngine>::hostage()->loadImage( m_fullname, 1 );

			if( m_outline == 0 )
			{
				MENGE_LOG_ERROR << "Image can't loaded " << m_fullname;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceFont::parseFontdef( DataStreamInterface * _stream )
	{
		String line = Utils::getLine( _stream );
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
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceFont::getInitSize()
	{
		return m_initSize;
	}
	//////////////////////////////////////////////////////////////////////////
}
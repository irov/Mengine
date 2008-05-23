#	include "ResourceFont.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

#	include "FileEngine.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceFont );
	//////////////////////////////////////////////////////////////////////////
	ResourceFont::ResourceFont( const ResourceFactoryParam & _params )
		: ResourceReference( _params )
		, m_image( 0 )
		, m_whsRatio( 3.0f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::setFontPath( const std::string & _path )
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
		FileDataInterface * stream = Holder<FileEngine>::hostage()->openFile( m_filename );

		if( stream == 0 )
		{
			MENGE_LOG("Warning: resource font not find fond file '%s'\n"
				, m_filename.c_str() 
				);

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
		m_initSize = static_cast<int>( ( rect[3] - rect[1] ) * m_image->getHeight() );

		Holder<FileEngine>::hostage()->closeFile( stream );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::_release()
	{
		Holder<RenderEngine>::hostage()->releaseImage( m_image );
		m_image = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceFont::getCharRatio( char _id ) const
	{
		TMapGlyph::const_iterator it = m_glyphs.find(_id);

		if( it == m_glyphs.end() )
		{
			return m_whsRatio;
		}

		return it->second.ratio;
	}
	//////////////////////////////////////////////////////////////////////////
	std::string ResourceFont::getFontDir( const std::string & _fontName )
	{
		std::string::size_type index = _fontName.find_last_of("/");

		std::string fontDir = _fontName;

		if( index != std::string::npos )
		{
			fontDir = fontDir.substr( 0, index + 1 );
		}

		return fontDir;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceFont::parseAttribute( const std::string & name, const std::string & params )
	{
		if ( name == "glyph" )
		{
			char r;
			float u1, v1, u2, v2;

			int err = sscanf_s( params.c_str(), "%c %f %f %f %f", &r, 1, &u1, &v1, &u2, &v2 );

			if (err == 0)
			{
				MENGE_LOG( "Error: in parsing params: '%s'", params.c_str() );
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
				MENGE_LOG( "Error: Image can't loaded '%s'", m_fullname.c_str() );
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceFont::parseFontdef( FileDataInterface * _stream )
	{
		std::string line = _stream->getLine( true );
		_stream->skipLine("{");
		
		while( !_stream->eof() )
        {
            line = _stream->getLine( true );

			if( line.length() == 0 || line.substr( 0, 2 ) == "//" )
            {
                continue;
            }

			std::string::size_type start = line.find_first_of( "\t\n " );
			std::string name = line.substr( 0, start );
			std::string value = line.substr( start + 1, line.size() );
		
			bool result = parseAttribute( name, value );

			if( result == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	int ResourceFont::getInitSize()
	{
		return m_initSize;
	}
	//////////////////////////////////////////////////////////////////////////
}
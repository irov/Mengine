#	include "ResourceFont.h"
#	include "ResourceImplement.h"

#	include "RenderEngine.h"
#	include "FileEngine.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceFont )
	//////////////////////////////////////////////////////////////////////////
	ResourceFont::ResourceFont( const std::string & _name )
		: ResourceReference( _name )
		, m_image( 0 )
		, m_whsRatio( 3.0f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::loader( TiXmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "File", "Path", m_fontName );
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
		FileDataInterface * stream = Holder<FileEngine>::hostage()->openFile( m_fontName );

		if( stream == 0 )
		{
			printf("Warning: resource font not find fond file '%s'\n"
				, m_fontName.c_str() 
				);

			return false;
		}

		m_fontDir = getFontDir( m_fontName );

		bool result = parseFontdef( stream );

		if( result == false )
		{
			return false;
		}
		
		m_whsRatio = getCharRatio('A');

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
		size_t index = _fontName.find_last_of("/");

		std::string fontDir = _fontName;

		if( index != std::string::npos )
		{
			fontDir = fontDir.substr( 0, index + 1 );
		}

		return fontDir;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceFont::parseAttribute( const std::vector<std::string> & _params )
	{
		const std::string & attrib = _params[0];

		if ( attrib == "glyph" )
		{
			if ( _params.size() != 6 )
			{
				printf( "Error: error parsing params of uv \n");
				return false;
			}

			float u1;
			float v1;
			float u2;
			float v2;

			sscanf_s( _params[2].c_str(), "%f", &u1 );
			sscanf_s( _params[3].c_str(), "%f", &v1 );
			sscanf_s( _params[4].c_str(), "%f", &u2 );
			sscanf_s( _params[5].c_str(), "%f", &v2 );

			setGlyph( _params[1].at(0), u1, v1, u2, v2 );
		}
		else if ( attrib == "source" )
		{
			if ( _params.size() != 2 )
			{
				printf( "Error: error parsing params of uv \n");
				return false;
			}

			const std::string & imageName = _params[1];

			FileDataInterface * fileData = Holder<FileEngine>::hostage()->openFile( m_fontDir + imageName );

			if( fileData == 0 )
			{
				printf( "Error: Image can't open resource file '%s'", imageName.c_str() );
				return false;
			}

			std::vector<char> _buff;

			size_t buff_size = fileData->size();
			_buff.resize( buff_size );
			fileData->read( &_buff[0], fileData->size() );

			TextureDesc textureDesc;

			textureDesc.buffer = &_buff[0];
			textureDesc.size = buff_size;
			textureDesc.name = imageName.c_str();
			textureDesc.filter = 1;

			m_image = Holder<RenderEngine>::hostage()->loadImage( textureDesc );

			if( m_image == 0 )
			{
				printf( "Error: Image can't loaded '%s'", imageName.c_str() );
				return false;
			}

			Holder<FileEngine>::hostage()->closeFile( fileData );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceFont::parseFontdef( FileDataInterface * _stream )
	{
		std::string line = _stream->getLine( true );
		_stream->skipLine("{");
		
		std::vector<std::string> params;

		while( !_stream->eof() )
        {
            line = _stream->getLine( true );

			if( line.length() == 0 || line.substr( 0, 2 ) == "//" )
            {
                continue;
            }
		
			splitParams( params, line );

			bool result = parseAttribute( params );

			if( result == false )
			{
				return false;
			}

			params.clear();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::splitParams( std::vector<std::string> & _words, const std::string & _text, const std::string & _separator )
    {
		size_t n = _text.length();

		size_t start = _text.find_first_not_of( _separator );

		while ( (start >= 0) && (start < n) )
		{
			size_t stop = _text.find_first_of( _separator, start );
			if ( (stop < 0) || (stop > n) ) stop = n;
			_words.push_back( _text.substr( start, stop - start ) );
			start = _text.find_first_not_of( _separator, stop + 1 );
		}
    }
	//////////////////////////////////////////////////////////////////////////
}
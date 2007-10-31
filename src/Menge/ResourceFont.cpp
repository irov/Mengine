#	include "ResourceFont.h"
#	include "ResourceImplement.h"

#	include "RenderEngine.h"
#	include "FileEngine.h"
#	include "ErrorMessage.h"


#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceFont )
	//////////////////////////////////////////////////////////////////////////
	ResourceFont::ResourceFont( const std::string & _name )
		: ResourceReference( _name )
		, m_image( 0 )
		, m_whitespaceWidth( 3.0f )
		, m_height( 12.0f )
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
	ResourceFont::UVRect ResourceFont::getUV( unsigned int _id ) const
	{
		TMapGlyph::const_iterator it = m_glyphs.find( _id );

		if ( it != m_glyphs.end() )
		{
			return it->second.rect;
		}
		else
		{
			static UVRect nullRect(0.0, 0.0, 0.0, 0.0);
			return nullRect;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceFont::setGlyph( unsigned int _id, float _u1, float _v1, float _u2, float _v2 )
    {
		TMapGlyph::iterator it = m_glyphs.find( _id );

		float ratio = ( _u2 - _u1 )  / ( _v2 - _v1 );
		UVRect rect( _u1, _v1, _u2, _v2 );

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

		m_fontDir = getFontDir( m_fontName );

		if( stream == 0 )
		{
			printf("Warning: resource font not find fond file '%s'\n"
				, m_fontName.c_str() 
				);

			return false;
		}

		parseFontdef( stream );
		
		m_whitespaceWidth = getCharWidth('A');

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
	float ResourceFont::getCharWidth( char _id ) const
	{
		TMapGlyph::const_iterator it = m_glyphs.find(_id);

		if( it == m_glyphs.end() )
		{
			return m_whitespaceWidth;
		}

		return it->second.ratio * m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceFont::getHeight() const
	{
		return m_height;
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
	std::vector<std::string> ResourceFont::splitParams( const std::string& text, const std::string& separator )
    {
        std::vector<std::string> words;
     
		int n = text.length();

		int start, stop;

		start = text.find_first_not_of(separator);

		while ( (start >= 0) && (start < n) )
		{
			stop = text.find_first_of(separator, start);
			if ( (stop < 0) || (stop > n) ) stop = n;
			words.push_back(text.substr(start, stop-start));
			start = text.find_first_not_of(separator,stop+1);
		}

		return words;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceFont::parseAttribute( const std::vector<std::string> & _params )
	{
		const std::string & attrib = _params[0];

		if ( attrib == "source" )
		{
			if ( _params.size() != 2 )
			{
				return false;
			}

			const std::string & imageName = _params[1];

			FileDataInterface * fileData = Holder<FileEngine>::hostage()->openFile( m_fontDir + imageName );

			if( fileData == 0 )
			{
//				ErrorMessage( "Error: Image can't open resource file '%s'", imageName.c_str() );
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

			Holder<FileEngine>::hostage()->closeFile( fileData );
		}
		else if ( attrib == "glyph" )
		{
			if (_params.size() != 6)
			{
				return false;
			}

			setGlyph(_params[1].at(0), 
				atof(_params[2].c_str()),
				atof(_params[3].c_str()),
				atof(_params[4].c_str()),
				atof(_params[5].c_str())); 
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

			std::vector<std::string> params = splitParams( line );

			bool result = parseAttribute( params );

			if( result == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}
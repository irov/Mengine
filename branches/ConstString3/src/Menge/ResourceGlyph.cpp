#	include "ResourceGlyph.h"
#	include "ResourceImplement.h"

#	include <cstdio>

#	include "BinParser.h"

#	include "Logger/Logger.h"
#	include "Core/String.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceGlyph );
	//////////////////////////////////////////////////////////////////////////
	ResourceGlyph::ResourceGlyph()
		: m_initSize(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceGlyph::Glyph * ResourceGlyph::getGlyph( unsigned int _id ) const
	{
		TMapGlyph::const_iterator it_found = m_glyphs.find( _id );

		if( it_found == m_glyphs.end() )
		{
			return 0;
		}

		return &it_found->second;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceGlyph::getInitSize() const
	{
		return m_initSize;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceGlyph::loader( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::Font_height, m_initSize );

			BIN_CASE_NODE( Protocol::Char )
			{
				String glyph, rect, offset;
				int width = 0;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Char_width, width );
					BIN_CASE_ATTRIBUTE( Protocol::Char_code, glyph );
					BIN_CASE_ATTRIBUTE( Protocol::Char_rect, rect );
					BIN_CASE_ATTRIBUTE( Protocol::Char_offset, offset );
				}

				this->addGlyph_( glyph, rect, offset, width );
			}
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceGlyph::setGlyph_( unsigned int _id, const mt::vec4f& _uv, const mt::vec2f& _offset, float _ratio, const mt::vec2f& _size )
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
			Glyph gl = { _id, _uv, _offset, _ratio, _size };

			m_glyphs.insert( 
				std::make_pair( _id, gl ) 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceGlyph::addGlyph_( const String& _glyph, const String& _rect, const String& _offset, int _width )
	{
		const char* glyph = _glyph.c_str();
		uint32 uiGlyph = *((unsigned int*)(glyph));
		uint32 clearBits[4] = { 0x000000FF, 0x0000FFFF, 0x00FFFFFF, 0xFFFFFFFF };
		std::size_t len = strlen( glyph );
		uiGlyph &= clearBits[len-1];

		int a, b, c, d, ox = 0, oy = 0;
		int err = std::sscanf( _rect.c_str(), "%d %d %d %d", &a, &b, &c, &d );

		if( err != 4 )
		{
			MENGE_LOG_ERROR( "Error parsing params: '%s'"
				, _rect.c_str() 
				);

			return;
		}

		if( _offset.empty() == false )
		{
			err = sscanf( _offset.c_str(), "%d %d", &ox, &oy );

			if( err != 2 )
			{
				MENGE_LOG_ERROR( "Error parsing params: '%s'"
					, _offset.c_str() 
					);

				return;
			}
		}

		mt::vec4f uv( (float)a, (float)b, (float)(a+c), (float)(b+d) );
		mt::vec2f offset( (float)ox, (float)oy );
		mt::vec2f size( (float)c, (float)d );

		this->setGlyph_( uiGlyph, uv, offset, _width / m_initSize, size );
	}
}
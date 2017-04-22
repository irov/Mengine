#	include "TextLine.h"

#	include "Interface/RenderSystemInterface.h"

#	include "Logger/Logger.h"

#   include <utf8.h>

#	include <math.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextLine::TextLine( ServiceProviderInterface * _serviceProvider, float _charOffset )
		: m_serviceProvider(_serviceProvider)
		, m_length(0.f)
        , m_charOffset(_charOffset)
		, m_offset(0.f)
		, m_invalidateTextLine(true)		
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool TextLine::initialize( const TextFontInterfacePtr & _font, const U32String & _text )
	{
		U32String::size_type text_size = _text.length();
		m_charsData.reserve( text_size );
		
		bool successful = true;

		for( U32String::const_iterator
			it = _text.begin(),
			it_end = _text.end();
			it != it_end;
			++it )
		{
			GlyphCode glyphChar = *it;
			
			U32String::const_iterator it_kerning = it;
			std::advance( it_kerning, 1 );

			GlyphCode glyphCharNext = (it_kerning != _text.end()) ? *it_kerning : 0;

			Glyph glyph;
			if( _font->getGlyph( glyphChar, glyphCharNext, &glyph ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("TextLine for fontName %s invalid glyph %u next %u"
					, _font->getName().c_str()
					, glyphChar
					, glyphCharNext
					);

				mt::uv4_from_mask( glyph.uv, mt::vec4f( 0.f, 0.f, 0.f, 0.f ) );
				glyph.offset = mt::vec2f( 0.f, 0.f );
				glyph.advance = 0.f;
				glyph.size = mt::vec2f( 0.f, 0.f );

				successful = false;

				continue;
			}			
					
			CharData charData;

			charData.code = glyphChar;

			charData.uv = glyph.uv;

			charData.advance = glyph.advance;
			charData.offset = glyph.offset;
			
			charData.size = glyph.size;

			charData.texture = glyph.texture;

			m_charsData.push_back( charData );

			m_length += charData.advance + m_charOffset;
		}

		m_length -= m_charOffset;

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t TextLine::getCharsDataSize() const
	{
		return (uint32_t)m_charsData.size();
	}
	//////////////////////////////////////////////////////////////////////////
	float TextLine::getLength() const
	{
		return m_length;
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorCharData & TextLine::getCharData() const
	{
		return m_charsData;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::calcCharPosition( const CharData & _cd, const mt::vec2f & _offset, float _charScale, uint32_t _index, mt::vec3f & _pos ) const
	{
		mt::vec2f size = _cd.size * _charScale;

		mt::vec2f offset = _offset + _cd.offset * _charScale;

		_pos.x = offset.x + ((_index & 1) ? size.x : 0.f);
		_pos.y = offset.y + ((_index & 2) ? size.y : 0.f);
		_pos.z = 0.f;		
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::advanceCharOffset( const CharData & _cd, float _charScale, mt::vec2f & _offset ) const
	{
		_offset.x += (_cd.advance + m_charOffset) * _charScale;
	}
	//////////////////////////////////////////////////////////////////////////
}

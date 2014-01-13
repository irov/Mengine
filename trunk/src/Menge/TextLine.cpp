#	include "TextLine.h"

#	include "Interface/RenderSystemInterface.h"

#	include "Logger/Logger.h"

#   include <utf8.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextLine::TextLine( ServiceProviderInterface * _serviceProvider, float _charOffset )
		: m_serviceProvider(_serviceProvider)
        , m_charOffset(_charOffset)
		, m_length(0.f)
		, m_invalidateTextLine(true)
		, m_offset(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::initialize( const TextFontInterfacePtr & _font, const String& _text )
	{
		WString::size_type text_size = _text.length();
		m_charsData.reserve( text_size );

		float totalKerning = 0.0f;
		
        const char * text_str = _text.c_str();
        size_t text_len = _text.size();

        //const char * text_it = text_str;
        //const char * text_end = text_str + text_len + 1;

		const RenderTextureInterfacePtr & image = _font->getTextureFont();

		float image_width = (float)image->getWidth();
		float image_height = (float)image->getHeight();

		float inv_image_width = 1.f / image_width;
		float inv_image_height = 1.f / image_height;

		for( const char
            *text_it = text_str,
            *text_end = text_str + text_len + 1;
        text_it != text_end;
        )
		{			
			size_t code = 0;
			utf8::internal::utf_error err = utf8::internal::validate_next( text_it, text_end, code );
			
			if( err != utf8::internal::UTF8_OK )
			{
				LOGGER_ERROR(m_serviceProvider)( "TextLine for fontName %s invalid glyph |%s| err code %d"
					, _font->getName().c_str()
					, text_it
					, err
					);

				continue;
			}

            if( code == 0 )
            {
                continue;
            }

			GlyphCode glyphChar;
			glyphChar.setCode( code );

			size_t code_next = 0;
			const char * text_it_next = text_it + 1;
			utf8::internal::validate_next( text_it_next, text_end, code_next );

            GlyphCode glyphCharNext;
            glyphCharNext.setCode( code_next );

			Glyph glyph;
			if( _font->getGlyph( glyphChar, glyphCharNext, &glyph ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)( "TextLine for fontName %s invalid glyph %d"
					, _font->getName().c_str()
					, code
					);

				continue;
			}			
					
			CharData charData;

			charData.code = glyphChar;	

			mt::ident_v3( charData.renderVertex[0] );
			mt::ident_v3( charData.renderVertex[1] );
			mt::ident_v3( charData.renderVertex[2] );
			mt::ident_v3( charData.renderVertex[3] );

			charData.uv = glyph.uv;

			charData.uv.x *= inv_image_width;
			charData.uv.y *= inv_image_height;
			charData.uv.z *= inv_image_width;
			charData.uv.w *= inv_image_height;

			charData.advance = glyph.advance;
			charData.offset = glyph.offset;
			
			charData.size = glyph.size;
			
			totalKerning += glyph.kerning;
			
			charData.offset.x += totalKerning;
			m_charsData.push_back( charData );

			m_length += charData.advance + m_charOffset;
		}

		m_length -= m_charOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	int TextLine::getCharsDataSize() const
	{
		return m_charsData.size();
	}
	//////////////////////////////////////////////////////////////////////////
	float TextLine::getLength() const
	{
		return m_length;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::prepareRenderObject(	mt::vec2f & _offset		
		, const mt::vec4f & _uv
		, unsigned int _argb	
        , bool _pixelsnap
		, TVectorRenderVertex2D& _renderObject ) const
	{
		if( m_invalidateTextLine == true )
		{
			this->updateRenderLine_( _offset );
		}

		size_t renderObjectNum = _renderObject.size();

		_renderObject.resize( renderObjectNum + m_charsData.size() * 4 );

		for( TVectorCharData::const_iterator
			it_char = m_charsData.begin(), 
			it_char_end = m_charsData.end();
		it_char != it_char_end; 
		++it_char )
		{
            const CharData & data = *it_char;

			for( size_t i = 0; i != 4; ++i )
			{
				RenderVertex2D & renderVertex = _renderObject[renderObjectNum + i];
				const mt::vec3f & charVertex = data.renderVertex[i];
				

                if( _pixelsnap == true )
                {
                    renderVertex.pos.x = floorf( charVertex.x + 0.5f );
                    renderVertex.pos.y = floorf( charVertex.y + 0.5f );
                }
                else
                {
                    renderVertex.pos.x = charVertex.x;
                    renderVertex.pos.y = charVertex.y;
                }                

				renderVertex.pos.z = charVertex.z;

				renderVertex.color = _argb;

                renderVertex.uv2.x = 0.f;
                renderVertex.uv2.y = 0.f;
            }

			const mt::vec4f & char_uv = data.uv;

			mt::vec4f total_uv;
			total_uv.x = _uv.x + (_uv.z - _uv.x) * char_uv.x;
			total_uv.y = _uv.y + (_uv.w - _uv.y) * char_uv.y;
			total_uv.z = _uv.x + (_uv.z - _uv.x) * char_uv.z;
			total_uv.w = _uv.y + (_uv.w - _uv.y) * char_uv.w;

			_renderObject[renderObjectNum + 0].uv.x = total_uv.x;
			_renderObject[renderObjectNum + 0].uv.y = total_uv.y;
			_renderObject[renderObjectNum + 1].uv.x = total_uv.z;
			_renderObject[renderObjectNum + 1].uv.y = total_uv.y;
			_renderObject[renderObjectNum + 2].uv.x = total_uv.z;
			_renderObject[renderObjectNum + 2].uv.y = total_uv.w;
			_renderObject[renderObjectNum + 3].uv.x = total_uv.x;
			_renderObject[renderObjectNum + 3].uv.y = total_uv.w;

			renderObjectNum += 4;
		}

		_offset.x += m_offset;

		return;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextLine::updateRenderLine_( mt::vec2f& _offset ) const
	{
		for( TVectorCharData::iterator
			it_char = m_charsData.begin(), 
			it_char_end = m_charsData.end();
		it_char != it_char_end; 
		++it_char )
		{
			CharData & cd = *it_char;

			mt::vec2f size = cd.size;

			mt::vec2f offset = _offset + cd.offset;
			mt::vec3f v3_offset(offset.x, offset.y, 0.f);
			
            cd.renderVertex[0] = v3_offset + mt::vec3f(0.f, 0.0f, 0.f);;
			cd.renderVertex[1] = v3_offset + mt::vec3f(size.x, 0.0f, 0.f);
			cd.renderVertex[2] = v3_offset + mt::vec3f(size.x, size.y, 0.f);
			cd.renderVertex[3] = v3_offset + mt::vec3f(0.0f, size.y, 0.f);

			_offset.x += cd.advance + m_charOffset;
		}

		m_offset = _offset.x;

		m_invalidateTextLine = false;
	}
	//////////////////////////////////////////////////////////////////////////
}

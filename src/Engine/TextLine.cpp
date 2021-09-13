#include "TextLine.h"

#include "Kernel/Logger.h"

#include "Config/Iterator.h"

#include "utf8.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TextLine::TextLine( uint32_t _layout, float _charOffset )
        : m_layout( _layout )
        , m_length( 0.f )
        , m_charOffset( _charOffset )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TextLine::~TextLine()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextLine::initialize( uint32_t _fontId, const TextFontInterfacePtr & _font, const U32String & _text )
    {
        U32String::size_type text_size = _text.size();
        m_charsData.reserve( text_size );

        bool successful = true;

        for( U32String::const_iterator
            it = _text.begin(),
            it_end = _text.end();
            it != it_end;
            ++it )
        {
            Char32 c = *it;
            GlyphCode glyphChar = (GlyphCode)c;

            U32String::const_iterator it_kerning = it;
            Iterator::advance( it_kerning, 1 );

            GlyphCode glyphCharNext = (it_kerning != _text.end()) ? *it_kerning : 0;

            Glyph glyph;
            if( _font->getGlyph( m_layout, glyphChar, glyphCharNext, &glyph ) == false )
            {
                LOGGER_ERROR( "fontName '%s' invalid glyph %u next %u"
                    , _font->getName().c_str()
                    , glyphChar
                    , glyphCharNext
                );

                successful = false;

                continue;
            }

            CharData charData;
            charData.code = glyphChar;
            charData.advance = glyph.advance;
            charData.offset = glyph.offset;
            charData.size = glyph.size;
            charData.uv = glyph.uv;
            charData.fontId = _fontId;
            charData.texture = glyph.texture;

            m_charsData.emplace_back( charData );

            m_length += charData.advance + m_charOffset;
        }

        if( _text.empty() == false )
        {
            m_length -= m_charOffset;
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TextLine::getCharsDataSize() const
    {
        VectorCharData::size_type charsDataSize = m_charsData.size();

        return (uint32_t)charsDataSize;
    }
    //////////////////////////////////////////////////////////////////////////
    float TextLine::getLength() const
    {
        return m_length;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorCharData & TextLine::getCharsData() const
    {
        return m_charsData;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextLine::calcCharPosition( const CharData & _cd, const mt::vec2f & _offset, float _charScale, uint32_t _index, mt::vec3f * const _pos ) const
    {
        mt::vec2f size = _cd.size * _charScale;

        mt::vec2f offset = _offset + _cd.offset * _charScale;

        const float size_xi[] = {0.f, size.x, size.x, 0.f};
        const float size_yi[] = {0.f, 0.f, size.y, size.y};

        float size_x = size_xi[_index];
        float size_y = size_yi[_index];

        _pos->x = offset.x + size_x;
        _pos->y = offset.y + size_y;
        _pos->z = 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextLine::advanceCharOffset( const CharData & _cd, float _charScale, mt::vec2f * const _offset ) const
    {
        _offset->x += (_cd.advance + m_charOffset) * _charScale;
    }
    //////////////////////////////////////////////////////////////////////////
}

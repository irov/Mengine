#include "TextLine.h"

#include "Kernel/Logger.h"

#include "Config/StdIterator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TextLine::TextLine( uint32_t _layout, float _charOffset, float _justifyLength )
        : m_layout( _layout )
        , m_length( 0.f )
        , m_charOffset( _charOffset )
        , m_justifyLength( _justifyLength )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TextLine::~TextLine()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextLine::initialize( uint32_t _fontId, const FontInterfacePtr & _font, const U32String & _text )
    {
        U32String::size_type text_size = _text.size();
        m_charsData.reserve( text_size );

        bool successful = true;

        float spaceCount = 0.f;

        for( U32String::const_iterator
            it = _text.begin(),
            it_end = _text.end();
            it != it_end;
            ++it )
        {
            Char32 c = *it;

            GlyphCode glyphChar = (GlyphCode)c;

            U32String::const_iterator it_kerning = it;
            StdIterator::advance( it_kerning, 1 );

            GlyphCode glyphCharNext = (it_kerning != _text.end()) ? *it_kerning : 0;

            Glyph glyph;
            if( _font->getGlyph( m_layout, glyphChar, glyphCharNext, &glyph ) == false )
            {
                LOGGER_ERROR( "text line font '%s' invalid glyph %u next %u"
                    , _font->getName().c_str()
                    , glyphChar
                    , glyphCharNext
                );

                successful = false;

                continue;
            }

            TextCharData charData;
            charData.code = glyphChar;
            charData.advance = glyph.advance;
            charData.offset = glyph.offset;
            charData.size = glyph.size;
            charData.uv = glyph.uv;
            charData.fontId = _fontId;
            charData.texture = glyph.texture;

            m_charsData.emplace_back( charData );

            m_length += charData.advance;
            m_length += m_charOffset;

            if( c == ' ' )
            {
                spaceCount += 1.f;
            }
        }

        if( _text.empty() == false )
        {
            m_length -= m_charOffset;

            if( m_justifyLength >= 0.f && m_justifyLength > m_length )
            {
                float advanceSpaceLength = m_justifyLength - m_length;
                float advanceSpace = advanceSpaceLength / spaceCount;

                for( TextCharData & charData : m_charsData )
                {
                    if( charData.code != ' ' )
                    {
                        continue;
                    }

                    charData.advance += advanceSpace;
                }

                m_length = m_justifyLength;
            }
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TextLine::getCharsDataSize() const
    {
        VectorTextCharData::size_type charsDataSize = m_charsData.size();

        return (uint32_t)charsDataSize;
    }
    //////////////////////////////////////////////////////////////////////////
    float TextLine::getLength() const
    {
        return m_length;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorTextCharData & TextLine::getCharsData() const
    {
        return m_charsData;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextLine::calcCharPosition( const TextCharData & _cd, const mt::vec2f & _offset, float _charScale, uint32_t _index, mt::vec3f * const _position )
    {
        mt::vec2f size = _cd.size * _charScale;

        mt::vec2f offset = _offset + _cd.offset * _charScale;

        const float size_xi[] = {0.f, size.x, size.x, 0.f};
        const float size_yi[] = {0.f, 0.f, size.y, size.y};

        float size_x = size_xi[_index];
        float size_y = size_yi[_index];

        _position->x = offset.x + size_x;
        _position->y = offset.y + size_y;
        _position->z = 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextLine::advanceCharOffset( const TextCharData & _cd, float _charScale, mt::vec2f * const _offset ) const
    {
        _offset->x += (_cd.advance + m_charOffset) * _charScale;
    }
    //////////////////////////////////////////////////////////////////////////
}

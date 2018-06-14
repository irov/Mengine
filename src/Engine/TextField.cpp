#include "TextField.h" 

#include "Interface/RenderSystemInterface.h"
#include "Interface/ResourceInterface.h"
#include "Interface/TextInterface.h"
#include "Interface/ApplicationInterface.h"

#include "Logger/Logger.h"

#include "Core/String.h"
#include "Core/StringFormat.h"

#include "TextLine.h"

#include "math/box2.h"

#include "pybind/system.hpp"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TextField::TextField()
        : m_horizontAlign( ETFHA_LEFT )
        , m_verticalAlign( ETFVA_BOTTOM )
        , m_invalidateFont( true )
        , m_charScale( 1.f )
        , m_maxLength( 2048.f )
        , m_lineOffset( 0.f )
        , m_charOffset( 0.f )
        , m_fontParams( EFP_NONE )
        , m_maxCharCount( 0xFFFFFFFF )
        , m_charCount( 0 )
        , m_layoutCount( 0 )
        , m_textSize( 0.f, 0.f )
        , m_wrap( true )
        , m_pixelsnap( true )
        , m_debugMode( false )
        , m_invalidateVertices( true )
        , m_invalidateVerticesWM( true )
        , m_invalidateTextLines( true )
        , m_invalidateTextEntry( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TextField::~TextField()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::_activate()
    {
        if( Node::_activate() == false )
        {
            return false;
        }

        NOTIFICATION_SERVICE()
            ->addObserverMethod( NOTIFICATOR_CHANGE_LOCALE_POST, this, &TextField::notifyChangeLocale );

        NOTIFICATION_SERVICE()
            ->addObserverMethod( NOTIFICATOR_DEBUG_TEXT_MODE, this, &TextField::notifyDebugMode );

        this->invalidateTextLines();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::_deactivate()
    {
        Node::_deactivate();

        NOTIFICATION_SERVICE()
            ->removeObserver( NOTIFICATOR_CHANGE_LOCALE_POST, this );

        NOTIFICATION_SERVICE()
            ->removeObserver( NOTIFICATOR_DEBUG_TEXT_MODE, this );
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::_compile()
    {
        this->invalidateTextEntry();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::_release()
    {
        if( m_font != nullptr )
        {
            m_font->releaseFont();
            m_font = nullptr;
        }

        m_chunks.clear();

        for( const CacheFont & cache : m_cacheFonts )
        {
            const TextFontInterfacePtr & font = cache.font;

            font->releaseFont();
        }

        m_cacheFonts.clear();

        m_textEntry = nullptr;

        TVectorTextLineLayout layouts;
        m_layouts.swap( layouts );

        TVectorRenderVertex2D vertexDataText;
        m_vertexDataText.swap( vertexDataText );

        TVectorRenderVertex2D vertexDataTextWM;
        m_vertexDataTextWM.swap( vertexDataTextWM );
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::notifyChangeLocale( const ConstString & _prevLocale, const ConstString & _currentlocale )
    {
        (void)_prevLocale;
        (void)_currentlocale;

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::notifyDebugMode( bool _debugMode )
    {
        m_debugMode = _debugMode;

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    const TextField::TVectorTextLineLayout & TextField::getTextLayots() const
    {
        if( this->isInvalidateTextLines() == true )
        {
            this->updateTextLines_();
        }

        return m_layouts;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::_invalidateWorldMatrix()
    {
        Node::_invalidateWorldMatrix();

        this->invalidateVerticesWM_();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateVertexData_( const TextFontInterfacePtr & _font, const ColourValue & _color, TVectorRenderVertex2D & _vertexData )
    {
        _vertexData.clear();
        m_chunks.clear();

        const TVectorTextLineLayout & layouts = this->getTextLayots();

        if( layouts.empty() == true )
        {
            return;
        }

        float fontAscent = _font->getFontAscent();
        float fontHeight = _font->getFontHeight();

        float lineOffset = this->calcLineOffset();

        ETextVerticalAlign verticalAlign = this->calcVerticalAlign();

        mt::vec2f base_offset( 0.f, 0.f );

        switch( verticalAlign )
        {
        case ETFVA_BOTTOM:
            {
                base_offset.y = (fontAscent + lineOffset) * (float( m_layoutCount ) - 1.f) * 0.5f;
            }break;
        case ETFVA_CENTER:
            {
                base_offset.y = -(fontAscent + lineOffset) * (float( m_layoutCount ) - 2.f) * 0.5f;
            }break;
        case ETFVA_TOP:
            {
                base_offset.y = (fontAscent + lineOffset);
            }break;
        }

        float charScale = this->calcCharScale();

        EMaterial materialId = EM_DEBUG;

        bool premultiply = _font->getFontPremultiply();

        if( premultiply == false )
        {
            switch( m_blendMode )
            {
            case EMB_NORMAL:
                {
                    materialId = EM_TEXTURE_BLEND;
                }break;
            case EMB_ADD:
                {
                    materialId = EM_TEXTURE_INTENSIVE;
                }break;
            case EMB_SCREEN:
                {
                    materialId = EM_TEXTURE_SCREEN;
                }break;
            case EMB_MULTIPLY:
                {
                    materialId = EM_TEXTURE_BLEND;
                }break;
            default:
                break;
            };
        }
        else
        {
            switch( m_blendMode )
            {
            case EMB_NORMAL:
                {
                    materialId = EM_TEXTURE_BLEND_PREMULTIPLY;
                }break;
            case EMB_ADD:
                {
                    materialId = EM_TEXTURE_INTENSIVE_PREMULTIPLY;
                }break;
            case EMB_SCREEN:
                {
                    materialId = EM_TEXTURE_SCREEN_PREMULTIPLY;
                }break;
            case EMB_MULTIPLY:
                {
                    materialId = EM_TEXTURE_MULTIPLY_PREMULTIPLY;
                }break;
            default:
                break;
            };
        }

        uint32_t cacheFontARGB[16] = { 0 };

        const ColourValue & paramsFontColor = this->calcFontColor();
        ColourValue colorBaseFont = paramsFontColor * _color;

        if( premultiply == true )
        {
            colorBaseFont.premultiplyRGB();
        }

        cacheFontARGB[0] = colorBaseFont.getAsARGB();

        for( TVectorCacheFonts::size_type index = 1; index != m_cacheFonts.size(); ++index )
        {
            CacheFont & cache = m_cacheFonts[index];

            const TextFontInterfacePtr & font = cache.font;

            const ColourValue & fontColor = font->getFontColor();
            ColourValue totalFontColor = fontColor * _color;

            if( premultiply == true )
            {
                totalFontColor.premultiplyRGB();
            }

            cacheFontARGB[index] = totalFontColor.getAsARGB();
        }

        Chunk chunk;
        chunk.vertex_begin = 0;
        chunk.vertex_count = 0;
        chunk.material = nullptr;

        mt::vec2f offset = base_offset;

        uint32_t textLineAlignOffsetIterator = 0U;

        for( const TVectorTextLine2 & lines2 : layouts )
        {
            float alignOffsetX = m_textLineAlignOffsets[textLineAlignOffsetIterator++];
            offset.x = alignOffsetX;

            for( const TVectorTextLine & lines : lines2 )
            {
                mt::vec2f offset2;

                for( const TextLine & line : lines )
                {
                    offset2 = offset;

                    const TVectorCharData & charsData = line.getCharsData();

                    for( const CharData & cd : charsData )
                    {
                        if( cd.texture == nullptr )
                        {
                            line.advanceCharOffset( cd, charScale, offset2 );

                            continue;
                        }

                        uint32_t argb = cacheFontARGB[cd.fontId];

                        for( uint32_t i = 0; i != 4; ++i )
                        {
                            RenderVertex2D v;

                            line.calcCharPosition( cd, offset2, charScale, i, v.position );

                            v.color = argb;
                            v.uv[0] = cd.uv[i];

                            _vertexData.emplace_back( v );
                        }

                        line.advanceCharOffset( cd, charScale, offset2 );

                        RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
                            ->getMaterial3( materialId, PT_TRIANGLELIST, 1, &cd.texture );

                        if( chunk.material == material )
                        {
                            chunk.vertex_count += 4;
                        }
                        else if( chunk.material == nullptr )
                        {
                            chunk.vertex_begin = 0;
                            chunk.vertex_count = 4;
                            chunk.material = material;
                        }
                        else
                        {
                            m_chunks.emplace_back( chunk );

                            chunk.vertex_begin = chunk.vertex_begin + chunk.vertex_count;
                            chunk.vertex_count = 4;
                            chunk.material = material;
                        }
                    }
                }

                offset.x = offset2.x;
            }

            offset.y += fontHeight;
            offset.y += lineOffset;
        }

        if( chunk.vertex_count != 0 )
        {
            m_chunks.emplace_back( chunk );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::_render( RenderServiceInterface * _renderService, const RenderContext * _state )
    {
        if( m_key.empty() == true )
        {
            return;
        }

        if( APPLICATION_SERVICE()
            ->getTextEnable() == false )
        {
            return;
        }

        const TextFontInterfacePtr & font = this->getFont();
        
        if( font == nullptr )
        {
            return;
        }

        const TVectorRenderVertex2D & textVertices = this->getTextVertices( font );

        if( textVertices.empty() == true )
        {
            return;
        }

        const TVectorRenderVertex2D::value_type * vertices = &textVertices.front();

        const mt::box2f & bb = this->getBoundingBox();

        for( const Chunk & chunk : m_chunks )
        {
            const TVectorRenderVertex2D::value_type * chunk_vertices = vertices + chunk.vertex_begin;

            if( chunk_vertices[0].color == 16777215 )
            {
                continue;
            }

            _renderService
                ->addRenderQuad( _state, chunk.material, chunk_vertices, chunk.vertex_count, &bb, false );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TextField::getCharCount() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "TextField::getCharCount '%s' not compile"
                , m_name.c_str()
            );

            return 0;
        }

        if( this->isInvalidateTextLines() == true )
        {
            this->updateTextLines_();
        }

        return m_charCount;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setMaxLength( float _maxLength )
    {
        m_maxLength = _maxLength;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::getMaxLength() const
    {
        return m_maxLength;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setWrap( bool _wrap )
    {
        m_wrap = _wrap;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::getWrap() const
    {
        return m_wrap;
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::getFontHeight() const
    {
        const TextFontInterfacePtr & font = this->getFont();

        if( font == nullptr )
        {
            return 0.f;
        }

        float fontHeight = font->getFontHeight();

        return fontHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setFontName( const ConstString & _fontName )
    {
#ifndef NDEBUG
        if( _fontName.empty() == false )
        {
            TextFontInterfacePtr font;
            if( TEXT_SERVICE()
                ->existFont( _fontName, font ) == false )
            {
                LOGGER_ERROR( "TextField::setFontName %s not found font %s"
                    , m_name.c_str()
                    , _fontName.c_str()
                );

                return;
            }
        }
#endif

        m_fontName = _fontName;

        m_fontParams |= EFP_FONT;

        this->invalidateFont();
        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TextField::getFontName() const
    {
        return m_fontName;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setFontColor( const ColourValue & _color )
    {
        m_colorFont = _color;

        m_fontParams |= EFP_COLOR_FONT;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    const ColourValue& TextField::getFontColor() const
    {
        return m_colorFont;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setLineOffset( float _offset )
    {
        m_lineOffset = _offset;

        m_fontParams |= EFP_LINE_OFFSET;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::getLineOffset() const
    {
        return m_lineOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setCharOffset( float _offset )
    {
        m_charOffset = _offset;

        m_fontParams |= EFP_CHAR_OFFSET;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::getCharOffset() const
    {
        return m_charOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setCharScale( float _value )
    {
        m_charScale = _value;

        m_fontParams |= EFP_CHAR_SCALE;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::getCharScale() const
    {
        return m_charScale;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f& TextField::getTextSize() const
    {
        if( this->isInvalidateTextLines() == true )
        {
            this->updateTextLines_();
        }

        return m_textSize;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateTextLinesMaxCount_( TVectorTextLines & _textLines ) const
    {
        uint32_t charIterator = 0;
        for( TVectorTextLines::iterator
            it_lines = _textLines.begin(),
            it_lines_end = _textLines.end();
            it_lines != it_lines_end;
            ++it_lines )
        {
            TVectorTextChunks & chars = *it_lines;

            for( TVectorTextChunks::iterator
                it_chars = chars.begin(),
                it_chars_end = chars.end();
                it_chars != it_chars_end;
                ++it_chars )
            {
                TextChunk & chunk = *it_chars;

                uint32_t value_size = (uint32_t)chunk.value.size();

                if( charIterator + value_size < m_maxCharCount )
                {
                    charIterator += value_size;

                    continue;
                }

                chunk.value = chunk.value.substr( 0, m_maxCharCount - charIterator );

                TVectorTextChunks::iterator it_chars_erase = it_chars;
                std::advance( it_chars_erase, 1 );

                chars.erase( it_chars_erase, chars.end() );

                TVectorTextLines::iterator it_lines_erase = it_lines;
                std::advance( it_lines_erase, 1 );

                _textLines.erase( it_lines_erase, _textLines.end() );

                return;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateTextLinesDimension_( const TextFontInterfacePtr & _font, const TVectorTextLines & _textLines, mt::vec2f & _size, uint32_t & _charCount, uint32_t & _layoutCount ) const
    {
        float fontHeight = _font->getFontAscent();

        float charOffset = this->calcCharOffset();
        float lineOffset = this->calcLineOffset();

        TVectorTextLineLayout layouts;
        for( const TVectorTextChunks & textChunks : _textLines )
        {
            TVectorTextLine2 textLine2;
            for( const TextChunk & textChunk : textChunks )
            {
                const CacheFont & cache = m_cacheFonts[textChunk.fontId];

                const TextFontInterfacePtr & chunkFont = cache.font;

                uint32_t layoutCount = chunkFont->getLayoutCount();

                TVectorTextLine textLine;
                for( uint32_t layoutIndex = 0; layoutIndex != layoutCount; ++layoutIndex )
                {
                    TextLine tl( layoutIndex, charOffset );
                    if( tl.initialize( textChunk.fontId, chunkFont, textChunk.value ) == false )
                    {
                        LOGGER_ERROR( "TextField::updateTextLines_ %s textID %s invalid setup line"
                            , this->getName().c_str()
                            , m_key.c_str()
                        );

                        continue;
                    }

                    textLine.emplace_back( tl );
                }

                textLine2.emplace_back( textLine );
            }

            layouts.emplace_back( textLine2 );
        }

        _charCount = 0;

        m_textLineAlignOffsets.clear();

        float maxlen = 0.f;
        for( const TVectorTextLine2 & lines2 : layouts )
        {
            float line_length = 0.f;
            uint32_t line_chars = 0;

            for( const TVectorTextLine & lines : lines2 )
            {
                const TextLine & line = lines[0];

                line_length += line.getLength();
                line_chars += line.getCharsDataSize();
            }

            maxlen = (std::max)(maxlen, line_length);
            _charCount += line_chars;

            float alignOffsetX = this->getHorizontAlignOffset_( lines2 );

            m_textLineAlignOffsets.push_back( alignOffsetX );
        }

        _layoutCount = (uint32_t)layouts.size();

        _size.x = maxlen;

        TVectorTextLineLayout::size_type lineCount = layouts.size();

        if( lineCount > 0 )
        {
            _size.y = (lineOffset + fontHeight) * (lineCount - 1) + fontHeight;
        }
        else
        {
            _size.y = fontHeight;
        }
    }
    ////////////////////////////////////////////////////////////////////////
    void TextField::updateTextLines_() const
    {
        m_invalidateTextLines = false;

        m_layouts.clear();
        m_textSize.x = 0.f;
        m_textSize.y = 0.f;
        m_charCount = 0;

        if( m_key.empty() == true )
        {
            return;
        }

        const TextFontInterfacePtr & baseFont = this->getFont();

        if( baseFont == nullptr )
        {
            return;
        }

        U32String cacheText;
        if( this->updateTextCache_( cacheText ) == false )
        {
            LOGGER_ERROR( "TextField::updateTextLines_ '%s' invalid update text cache %s"
                , this->getName().c_str()
                , m_key.c_str()
            );

            return;
        }

        for( const CacheFont & cache : m_cacheFonts )
        {
            const TextFontInterfacePtr & font = cache.font;

            font->releaseFont();
        }

        m_cacheFonts.clear();

        CacheFont baseCacheFont;
        baseCacheFont.font = baseFont;

        m_cacheFonts.emplace_back( baseCacheFont );

        TVectorTextChunks textChars;
        Helper::test( textChars, cacheText, m_cacheFonts, 0 );

        for( const CacheFont & cache : m_cacheFonts )
        {
            const TextFontInterfacePtr & font = cache.font;

            if( font->compileFont() == false )
            {
                return;
            }
        }

        for( const TextChunk & tc : textChars )
        {
            const CacheFont & cache = m_cacheFonts[tc.fontId];

            const TextFontInterfacePtr & font = cache.font;

            if( font->prepareGlyph( tc.value ) == false )
            {
                return;
            }
        }

        TVectorU32String line_delims;
        line_delims.emplace_back( U"\n" );
        line_delims.emplace_back( U"\r\n" );
        line_delims.emplace_back( U"\n\r" );
        line_delims.emplace_back( U"\n\r\t" );

        TVectorTextLines textLines;
        Helper::split( textLines, textChars, line_delims );

        float charScale = this->calcCharScale();
        float charOffset = this->calcCharOffset();
        float maxLength = this->calcMaxLength();

        if( m_wrap == true )
        {
            U32String space_delim = U" ";

            TVectorU32String space_delims;
            if( m_wrap == true )
            {
                space_delims.emplace_back( space_delim );
                space_delims.emplace_back( U"\r" );
            }

            TVectorTextLines new_textLines;
            for( const TVectorTextChunks & textChunks : textLines )
            {
                float length = 0.f;

                TVectorTextChunks new_textChunks;
                for( const TextChunk & textChunk : textChunks )
                {
                    const U32String & text = textChunk.value;
                    const CacheFont & cache = m_cacheFonts[textChunk.fontId];

                    const TextFontInterfacePtr & font = cache.font;

                    TVectorU32String words;
                    Helper::u32split2( words, text, false, space_delims );

                    TextChunk new_textChunk;
                    for( const U32String & word : words )
                    {
                        float word_length = 0.f;

                        for( U32String::const_iterator
                            it = word.begin(),
                            it_end = word.end();
                            it != it_end;
                            ++it )
                        {
                            Char32 c = *it;
                            GlyphCode glyphChar = (GlyphCode)c;

                            U32String::const_iterator it_kerning = it;
                            std::advance( it_kerning, 1 );

                            GlyphCode glyphCharNext = (it_kerning != word.end()) ? *it_kerning : 0;

                            Glyph glyph;
                            if( font->getGlyph( 0, glyphChar, glyphCharNext, &glyph ) == false )
                            {
                                continue;
                            }

                            float char_length = (glyph.advance + charOffset) * charScale;

                            word_length += char_length;
                        }

                        if( length + word_length > maxLength )
                        {
                            length = word_length;

                            new_textChunks.emplace_back( new_textChunk );
                            new_textChunk.value.clear();

                            new_textLines.emplace_back( new_textChunks );
                            new_textChunks.clear();
                        }
                        else
                        {
                            length += word_length;
                        }

                        if( new_textChunk.value.empty() == true )
                        {
                            if( word.empty() == true )
                            {
                                new_textChunk.value.insert( new_textChunk.value.end(), space_delim.begin(), space_delim.end() );
                            }
                            else
                            {
                                new_textChunk.value.insert( new_textChunk.value.end(), word.begin(), word.end() );
                            }
                        }
                        else
                        {
                            new_textChunk.value.insert( new_textChunk.value.end(), space_delim.begin(), space_delim.end() );
                            new_textChunk.value.insert( new_textChunk.value.end(), word.begin(), word.end() );
                        }

                        new_textChunk.fontId = textChunk.fontId;
                    }

                    new_textChunks.emplace_back( new_textChunk );
                }

                new_textLines.emplace_back( new_textChunks );
            }

            textLines.swap( new_textLines );
        }

        this->updateTextLinesDimension_( baseFont, textLines, m_textSize, m_charCount, m_layoutCount );

        this->updateTextLinesMaxCount_( textLines );

        //if( m_debugMode == true )
        //{
        //    String s_key = m_key.c_str();
        //    String s_font = this->calcFontName().c_str();

        //    U32String u32_key( s_key.begin(), s_key.end() );
        //    U32String u32_font( s_font.begin(), s_font.end() );

        //    textLines.insert( textLines.begin(), u32_key );
        //    textLines.insert( textLines.begin(), u32_font );

        //    font->prepareText( s_key );
        //    font->prepareText( s_font );
        //}

        for( const TVectorTextChunks & textChunks : textLines )
        {
            TVectorTextLine2 textLine2;
            for( const TextChunk & textChunk : textChunks )
            {
                const CacheFont & cache = m_cacheFonts[textChunk.fontId];

                const TextFontInterfacePtr & chunkFont = cache.font;

                uint32_t layoutCount = chunkFont->getLayoutCount();

                TVectorTextLine textLine;
                for( uint32_t layoutIndex = 0; layoutIndex != layoutCount; ++layoutIndex )
                {
                    TextLine tl( layoutIndex, charOffset );
                    if( tl.initialize( textChunk.fontId, chunkFont, textChunk.value ) == false )
                    {
                        LOGGER_ERROR( "TextField::updateTextLines_ %s textID %s invalid setup line"
                            , this->getName().c_str()
                            , m_key.c_str()
                        );

                        continue;
                    }

                    textLine.emplace_back( tl );
                }

                textLine2.emplace_back( textLine );
            }

            m_layouts.emplace_back( textLine2 );
        }

        this->invalidateVertices_();
    }
    ////////////////////////////////////////////////////////////////////////
    void TextField::updateFont_() const
    {
        m_invalidateFont = false;

        ConstString fontName = this->calcFontName();

        if( fontName.empty() == true )
        {
            fontName = TEXT_SERVICE()
                ->getDefaultFontName();
        }

        if( m_font != nullptr )
        {
            const ConstString & currentFontName = m_font->getName();

            if( fontName == currentFontName )
            {
                return;
            }
            else
            {
                m_font->releaseFont();
                m_font = nullptr;
            }
        }

        if( fontName.empty() == true )
        {
            return;
        }

        m_font = TEXT_SERVICE()
            ->getFont( fontName );

        if( m_font == nullptr )
        {
            LOGGER_ERROR( "TextField::updateFont_ '%s' can't found font '%s'"
                , this->getName().c_str()
                , fontName.c_str()
            );

            return;
        }

        if( m_font->compileFont() == false )
        {
            LOGGER_ERROR( "TextField::updateFont_ '%s' invalid compile font '%s'"
                , this->getName().c_str()
                , fontName.c_str()
            );

            return;
        }

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateTextEntry_() const
    {
        m_invalidateTextEntry = false;

        m_textEntry = TEXT_SERVICE()
            ->getTextEntry( m_key );

        if( m_textEntry == nullptr )
        {
            LOGGER_ERROR( "TextField::updateTextEntry_ '%s' can't find text ID '%s'"
                , this->getName().c_str()
                , m_key.c_str()
            );

            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TextField::calcFontName() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( params & EFP_FONT )
            {
                const ConstString & fontName = textEntry->getFontName();

                return fontName;
            }
        }

        if( m_fontName.empty() == true )
        {
            const ConstString & fontName = TEXT_SERVICE()
                ->getDefaultFontName();

            return fontName;
        }

        return m_fontName;
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::calcLineOffset() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( params & EFP_LINE_OFFSET )
            {
                float value = textEntry->getLineOffset();

                return value;
            }
        }

        if( m_fontParams & EFP_LINE_OFFSET )
        {
            return m_lineOffset;
        }

        const TextFontInterfacePtr & font = this->getFont();

        if( font != nullptr )
        {
            uint32_t params = font->getFontParams();

            float fontHeight = font->getFontHeight();

            if( params & EFP_LINE_OFFSET )
            {
                float value = font->getLineOffset();

                return fontHeight + value;
            }
        }

        return m_lineOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::calcCharOffset() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( params & EFP_CHAR_OFFSET )
            {
                float value = textEntry->getCharOffset();

                return value;
            }
        }

        if( m_fontParams & EFP_CHAR_OFFSET )
        {
            return m_charOffset;
        }

        const TextFontInterfacePtr & font = this->getFont();

        if( font != nullptr )
        {
            uint32_t params = font->getFontParams();

            if( params & EFP_CHAR_OFFSET )
            {
                float value = font->getCharOffset();

                return value;
            }
        }

        return m_charOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::calcMaxLength() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( params & EFP_MAX_LENGTH )
            {
                float value = textEntry->getMaxLength();

                return value;
            }
        }

        return m_maxLength;
    }
    //////////////////////////////////////////////////////////////////////////
    const ColourValue & TextField::calcFontColor() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( params & EFP_COLOR_FONT )
            {
                const ColourValue & value = textEntry->getColorFont();

                return value;
            }
        }

        if( m_fontParams & EFP_COLOR_FONT )
        {
            return m_colorFont;
        }

        const TextFontInterfacePtr & font = this->getFont();

        if( font != nullptr )
        {
            uint32_t params = font->getFontParams();

            if( params & EFP_COLOR_FONT )
            {
                const ColourValue & value = font->getFontColor();

                return value;
            }
        }

        return m_colorFont;
    }
    //////////////////////////////////////////////////////////////////////////
    ETextHorizontAlign TextField::calcHorizontalAlign() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( params & EFP_HORIZONTAL_ALIGN )
            {
                ETextHorizontAlign value = textEntry->getHorizontAlign();

                return value;
            }
        }

        if( m_fontParams & EFP_HORIZONTAL_ALIGN )
        {
            return m_horizontAlign;
        }

        return ETFHA_LEFT;
    }
    //////////////////////////////////////////////////////////////////////////
    ETextVerticalAlign TextField::calcVerticalAlign() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( params & EFP_VERTICAL_ALIGN )
            {
                ETextVerticalAlign value = textEntry->getVerticalAlign();

                return value;
            }
        }

        if( m_fontParams & EFP_VERTICAL_ALIGN )
        {
            return m_verticalAlign;
        }

        return ETFVA_TOP;
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::calcCharScale() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( params & EFP_CHAR_SCALE )
            {
                float value = textEntry->getCharScale();

                return value;
            }
        }

        if( m_fontParams & EFP_CHAR_SCALE )
        {
            return m_charScale;
        }

        return 1.f;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::_updateBoundingBox( mt::box2f & _boundingBox ) const
    {
        Node::_updateBoundingBox( _boundingBox );

        //mt::vec2f offset = mt::zero_v2;

        //const mt::mat4f & wm = this->getWorldMatrix();

        //const TListTextLine & lines = this->getTextLines();

        //for( TListTextLine::const_iterator
        //    it_line = lines.begin(),
        //    it_line_end = lines.end();
        //    it_line != it_line_end;
        //    ++it_line )
        //{
        //    mt::vec2f alignOffset;

        //    this->updateAlignOffset_( *it_line, alignOffset );

        //    offset.x = alignOffset.x;
        //    offset.y += alignOffset.y;

        //    it_line->updateBoundingBox( offset, wm, _boundingBox );

        //    offset.y += m_lineOffset;
        //}
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::getHorizontAlignOffset_( const TVectorTextLine2 & _lines ) const
    {
        float length = 0.f;
        for( const TVectorTextLine & line : _lines )
        {
            length += line[0].getLength();
        }

        float offset = 0.f;

        ETextHorizontAlign horizontAlign = this->calcHorizontalAlign();

        switch( horizontAlign )
        {
        case ETFHA_LEFT:
            {
                offset = 0.f;
            }break;
        case ETFHA_CENTER:
            {
                offset = -length * 0.5f;
            }break;
        case ETFHA_RIGHT:
            {
                offset = -length;
            }break;
        }

        return offset;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::_invalidateColor()
    {
        Node::_invalidateColor();

        this->invalidateVertices_();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TextField::getMaxCharCount() const
    {
        return m_maxCharCount;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setMaxCharCount( uint32_t _maxCharCount )
    {
        m_maxCharCount = _maxCharCount;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setTextID( const ConstString & _key )
    {
        m_key = _key;

        m_textFormatArgs.clear();

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::removeTextID()
    {
        m_textEntry = nullptr;
        m_textFormatArgs.clear();

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TextField::getTextID() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTextEntry();

        if( textEntry == nullptr )
        {
            return ConstString::none();
        }

        const ConstString & key = textEntry->getKey();

        return key;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setTextFormatArgs( const TVectorString & _args )
    {
        if( m_textFormatArgs == _args )
        {
            return;
        }

        m_textFormatArgs = _args;

        this->invalidateFont();
        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::removeTextFormatArgs()
    {
        m_textFormatArgs.clear();

        this->invalidateFont();
        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    const TVectorString & TextField::getTextFormatArgs() const
    {
        return m_textFormatArgs;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TextField::getTextExpectedArgument() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTextEntry();

        if( textEntry == nullptr )
        {
            LOGGER_ERROR( "TextField::getTextExpectedArgument '%s:%s' not compile"
                , this->getName().c_str()
                , m_key.c_str()
            );

            return 0;
        }

        const String & textValue = textEntry->getValue();

        try
        {
            uint32_t expected_args = Helper::getStringFormatExpectedArgs( textValue );
            
            return expected_args;
        }
        catch( const std::exception & _ex )
        {
            LOGGER_ERROR( "TextField::getTextExpectedArgument '%s:%s' except error '%s'"
                , this->getName().c_str()
                , this->getTextID().c_str()
                , _ex.what()
            );
        }

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////	
    bool TextField::updateTextCache_( U32String & _cacheText ) const
    {
        const TextEntryInterfacePtr & textEntry = this->getTextEntry();

        if( textEntry == nullptr )
        {
            LOGGER_ERROR( "TextField::updateTextCache_ '%s:%s' invalid get text entry can't setup text ID"
                , this->getName().c_str()
                , m_key.c_str()
            );

            return false;
        }

        const TextFontInterfacePtr & font = this->getFont();

        if( font == nullptr )
        {
            return false;
        }

        const String & textValue = textEntry->getValue();
                
        try
        {
            String fmt = Helper::getStringFormat( textValue, m_textFormatArgs );

            _cacheText = font->prepareText( fmt );
        }
        catch( const std::exception & _ex )
        {
            LOGGER_ERROR( "TextField::updateTextCache_ '%s:%s' except error '%s'"
                , this->getName().c_str()
                , this->getTextID().c_str()
                , _ex.what()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setPixelsnap( bool _pixelsnap )
    {
        m_pixelsnap = _pixelsnap;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::getPixelsnap() const
    {
        return m_pixelsnap;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setVerticalBottomAlign()
    {
        m_verticalAlign = ETFVA_BOTTOM;

        m_fontParams |= EFP_VERTICAL_ALIGN;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::isVerticalBottomAlign() const
    {
        return m_verticalAlign == ETFVA_BOTTOM;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setVerticalCenterAlign()
    {
        m_verticalAlign = ETFVA_CENTER;

        m_fontParams |= EFP_VERTICAL_ALIGN;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::isVerticalCenterAlign() const
    {
        return m_verticalAlign == ETFVA_CENTER;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setHorizontalCenterAlign()
    {
        m_horizontAlign = ETFHA_CENTER;

        m_fontParams |= EFP_HORIZONTAL_ALIGN;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::isHorizontalCenterAlign() const
    {
        return m_horizontAlign == ETFHA_CENTER;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setHorizontalRightAlign()
    {
        m_horizontAlign = ETFHA_RIGHT;

        m_fontParams |= EFP_HORIZONTAL_ALIGN;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::isHorizontalRightAlign() const
    {
        return m_horizontAlign == ETFHA_RIGHT;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setHorizontalLeftAlign()
    {
        m_horizontAlign = ETFHA_LEFT;

        m_fontParams |= EFP_HORIZONTAL_ALIGN;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::isHorizontalLeftAlign() const
    {
        return m_horizontAlign == ETFHA_LEFT;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::invalidateVertices_() const
    {
        m_invalidateVertices = true;

        this->invalidateVerticesWM_();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::invalidateVerticesWM_() const
    {
        m_invalidateVerticesWM = true;

        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::invalidateTextLines() const
    {
        m_invalidateTextLines = true;

        this->invalidateVertices_();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::invalidateTextEntry() const
    {
        m_invalidateTextEntry = true;

        this->invalidateFont();
        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateVerticesWM_( const TextFontInterfacePtr & _font )
    {
        m_invalidateVerticesWM = false;

        if( m_invalidateVertices == true )
        {
            this->updateVertices_( _font );
        }

        this->updateVertexDataWM_( m_vertexDataTextWM, m_vertexDataText );
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateVertexDataWM_( TVectorRenderVertex2D & _outVertex, const TVectorRenderVertex2D & _fromVertex )
    {
        _outVertex.assign( _fromVertex.begin(), _fromVertex.end() );

        TVectorRenderVertex2D::const_iterator it = _fromVertex.begin();
        TVectorRenderVertex2D::const_iterator it_end = _fromVertex.end();

        TVectorRenderVertex2D::iterator it_w = _outVertex.begin();

        mt::mat4f wm = this->getWorldMatrix();

        if( m_pixelsnap == true )
        {
            wm.v3.x = ::floorf( wm.v3.x + 0.5f );
            wm.v3.y = ::floorf( wm.v3.y + 0.5f );
        }

        //float charScale = this->getCharScale();

        //if( mt::equal_f_1( charScale ) == false )
        //{
        //	mt::vec3f position;
        //	mt::vec3f origin;
        //	mt::vec3f scale;
        //	mt::vec2f skew;
        //	mt::vec3f orientation;
        //	this->getTransformation( position, origin, scale, skew, orientation );

        //	mt::mat4f wm;
        //	this->calcWorldMatrix( wm, position, origin, scale + charScale, skew, orientation );
        //}

        for( ; it != it_end; ++it, ++it_w )
        {
            const RenderVertex2D & vertex = *it;

            RenderVertex2D & vertex_w = *it_w;

            mt::mul_v3_v3_m4( vertex_w.position, vertex.position, wm );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateVertices_( const TextFontInterfacePtr & _font )
    {
        m_invalidateVertices = false;

        ColourValue colorNode;
        this->calcTotalColor( colorNode );

        this->updateVertexData_( _font, colorNode, m_vertexDataText );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr TextField::_updateMaterial() const
    {
        return nullptr;
    }
}

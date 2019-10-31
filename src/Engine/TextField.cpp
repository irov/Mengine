#include "TextField.h" 

#include "Interface/TextServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "Kernel/StringFormat.h"
#include "Kernel/StringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "TextLine.h"

#include "math/box2.h"

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
        , m_autoScaleFactor( 1.f )
        , m_lineOffset( 0.f )
        , m_charOffset( 0.f )
        , m_fontParams( EFP_NONE )
        , m_maxCharCount( ~0U )
        , m_charCount( 0 )
        , m_layoutCount( 0 )
        , m_textSize( 0.f, 0.f )
        , m_wrap( true )
        , m_autoScale( false )
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

        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_CHANGE_LOCALE_POST, this, &TextField::notifyChangeLocale );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_DEBUG_TEXT_MODE, this, &TextField::notifyDebugMode );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_CHANGE_TEXT_ALIAS, this, &TextField::notifyChangeTextAliasArguments );

        this->invalidateTextLines();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::_deactivate()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_LOCALE_POST );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEBUG_TEXT_MODE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_TEXT_ALIAS );

        Node::_deactivate();
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

        VectorTextLinesLayout layouts;
        m_layouts.swap( layouts );

        VectorRenderVertex2D vertexDataText;
        m_vertexDataText.swap( vertexDataText );

        VectorRenderVertex2D vertexDataTextWM;
        m_vertexDataTextWM.swap( vertexDataTextWM );
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::notifyChangeLocale( const ConstString & _prevLocale, const ConstString & _currentlocale )
    {
        MENGINE_UNUSED( _prevLocale );
        MENGINE_UNUSED( _currentlocale );

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::notifyDebugMode( bool _debugMode )
    {
        m_debugMode = _debugMode;

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::notifyChangeTextAliasArguments( const ConstString & _environment, const ConstString & _alias )
    {
        if( m_aliasEnvironment != _environment || m_textId != _alias )
        {
            return;
        }

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    const TextField::VectorTextLinesLayout & TextField::getTextLayots() const
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
    void TextField::updateVertexData_( const TextFontInterfacePtr & _font, const Color & _color, VectorRenderVertex2D & _vertexData ) const
    {
        _vertexData.clear();
        m_chunks.clear();

        const VectorTextLinesLayout & layouts = this->getTextLayots();

        if( layouts.empty() == true )
        {
            return;
        }

        float fontHeight = _font->getFontHeight();
        //float fontAscent = _font->getFontAscent();

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
                    materialId = EM_TEXTURE_MULTIPLY;
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

        const Color & paramsFontColor = this->calcFontColor();
        Color colorBaseFont = paramsFontColor * _color;

        if( premultiply == true )
        {
            colorBaseFont.premultiplyRGB();
        }

        cacheFontARGB[0] = colorBaseFont.getAsARGB();

        for( VectorCacheFonts::size_type index = 1; index != m_cacheFonts.size(); ++index )
        {
            CacheFont & cache = m_cacheFonts[index];

            const TextFontInterfacePtr & font = cache.font;

            const Color & fontColor = font->getFontColor();
            Color totalFontColor = fontColor * _color;

            if( premultiply == true )
            {
                totalFontColor.premultiplyRGB();
            }

            cacheFontARGB[index] = totalFontColor.getAsARGB();
        }

        float lineOffset = this->calcLineOffset();

        float linesOffset = this->calcLinesOffset( lineOffset, _font );

        mt::vec2f base_offset( 0.f, 0.f );
        base_offset.y = linesOffset * m_autoScaleFactor;

        TextRenderChunk chunk;
        chunk.vertex_begin = 0;
        chunk.vertex_count = 0;
        chunk.material = nullptr;

        mt::vec2f offset = base_offset;

        uint32_t textLineAlignOffsetIterator = 0U;

        for( const VectorTextLines2 & lines2 : layouts )
        {
            float alignOffsetX = m_textLineAlignOffsets[textLineAlignOffsetIterator++];
            offset.x = alignOffsetX * m_autoScaleFactor;

            for( const VectorTextLines & lines : lines2 )
            {
                mt::vec2f offset2;

                for( const TextLine & line : lines )
                {
                    offset2 = offset;

                    const VectorCharData & charsData = line.getCharsData();

                    for( const CharData & cd : charsData )
                    {
                        if( cd.texture == nullptr )
                        {
                            line.advanceCharOffset( cd, charScale * m_autoScaleFactor, offset2 );

                            continue;
                        }

                        uint32_t argb = cacheFontARGB[cd.fontId];

                        for( uint32_t i = 0; i != 4; ++i )
                        {
                            RenderVertex2D v;

                            line.calcCharPosition( cd, offset2, charScale * m_autoScaleFactor, i, v.position );

                            v.color = argb;
                            v.uv[0] = cd.uv[i];

                            _vertexData.emplace_back( v );
                        }

                        line.advanceCharOffset( cd, charScale * m_autoScaleFactor, offset2 );

                        RenderMaterialInterfacePtr material = this->getMaterial3( materialId, PT_TRIANGLELIST, 1, &cd.texture, MENGINE_DOCUMENT_FACTORABLE );

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
    void TextField::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        if( m_textId.empty() == true )
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

        bool invalidate = false;
        uint32_t enumerate = 0;
        for( const LambdaFormatArgsContext & context : m_textFormatArgContexts )
        {   
            if( context != nullptr )
            {
                String & arg = m_textFormatArgs[enumerate];
                invalidate |= context( &arg );
            }

            ++enumerate;
        }

        if( invalidate == true )
        {
            this->invalidateTextLines();
        }

        const VectorRenderVertex2D & textVertices = this->getTextVertices( font );

        if( textVertices.empty() == true )
        {
            return;
        }

        const VectorRenderVertex2D::value_type * vertices = textVertices.data();

        const mt::box2f * bb = this->getBoundingBox();

        for( const TextRenderChunk & chunk : m_chunks )
        {
            const VectorRenderVertex2D::value_type * chunk_vertices = vertices + chunk.vertex_begin;

            if( chunk_vertices[0].color == 16777215 )
            {
                continue;
            }

            _renderPipeline->addRenderQuad( _context, chunk.material, chunk_vertices, chunk.vertex_count, bb, false, MENGINE_DOCUMENT_FACTORABLE );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TextField::getCharCount() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' not compile"
                , this->getName().c_str()
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
    void TextField::setAutoScale( bool _autoScale )
    {
        m_autoScale = _autoScale;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::getAutoScale() const
    {
        return m_autoScale;
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
#ifdef MENGINE_DEBUG
        if( _fontName.empty() == false )
        {
            if( TEXT_SERVICE()
                ->existFont( _fontName, nullptr ) == false )
            {
                LOGGER_ERROR( "'%s' not found font '%s'"
                    , this->getName().c_str()
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
    void TextField::setFontColor( const Color & _color )
    {
        m_colorFont = _color;

        m_fontParams |= EFP_COLOR_FONT;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & TextField::getFontColor() const
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
    const mt::vec2f & TextField::getTextSize() const
    {
        if( this->isInvalidateTextLines() == true )
        {
            this->updateTextLines_();
        }

        return m_textSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::calcTextViewport( Viewport & _viewport ) const
    {
        if( m_textId.empty() == true )
        {
            mt::box2f box;
            mt::insideout_box( box );

            _viewport.begin = box.minimum;
            _viewport.end = box.maximum;

            return false;
        }

        const TextFontInterfacePtr & font = this->getFont();

        if( font == nullptr )
        {
            mt::box2f box;
            mt::insideout_box( box );

            _viewport.begin = box.minimum;
            _viewport.end = box.maximum;

            return false;
        }

        float lineOffset = this->calcLineOffset();

        float linesOffset = this->calcLinesOffset( lineOffset, font );

        const mt::vec2f & size = this->getTextSize();

        _viewport.begin = mt::vec2f( 0.f, linesOffset ) - size;
        _viewport.end = mt::vec2f( 0.f, linesOffset );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateTextLinesWrap_( VectorTextLineChunks2 & _textLines ) const
    {
        if( m_wrap == false )
        {
            return;
        }

        float charOffset = this->calcCharOffset();
        float charScale = this->calcCharScale();
        float maxLength = this->calcMaxLength();

        U32String space_delim = U" ";

        VectorU32String space_delims;
        space_delims.emplace_back( space_delim );
        space_delims.emplace_back( U"\r" );

        VectorTextLineChunks2 new_textLines;
        for( const VectorTextLineChunks & textChunks : _textLines )
        {
            float length = 0.f;

            VectorTextLineChunks new_textChunks;
            for( const TextLineChunk & textChunk : textChunks )
            {
                const U32String & text = textChunk.value;
                const CacheFont & cache = m_cacheFonts[textChunk.fontId];

                const TextFontInterfacePtr & font = cache.font;

                VectorU32String words;
                Helper::u32split2( words, text, false, space_delims );

                TextLineChunk new_textChunk;
                new_textChunk.fontId = textChunk.fontId;

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

                    if( length + word_length > maxLength && new_textChunk.value.empty() == false )
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
                }

                new_textChunks.emplace_back( new_textChunk );
            }

            new_textLines.emplace_back( new_textChunks );
        }

        _textLines.swap( new_textLines );
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateTextLinesMaxCount_( VectorTextLineChunks2 & _textLines ) const
    {
        if( m_maxCharCount == ~0U )
        {
            return;
        }

        uint32_t charIterator = 0;
        for( VectorTextLineChunks2::iterator
            it_lines = _textLines.begin(),
            it_lines_end = _textLines.end();
            it_lines != it_lines_end;
            ++it_lines )
        {
            VectorTextLineChunks & chars = *it_lines;

            for( VectorTextLineChunks::iterator
                it_chars = chars.begin(),
                it_chars_end = chars.end();
                it_chars != it_chars_end;
                ++it_chars )
            {
                TextLineChunk & chunk = *it_chars;

                uint32_t value_size = (uint32_t)chunk.value.size();

                if( charIterator + value_size < m_maxCharCount )
                {
                    charIterator += value_size;

                    continue;
                }

                chunk.value = chunk.value.substr( 0, m_maxCharCount - charIterator );

                VectorTextLineChunks::iterator it_chars_erase = it_chars;
                std::advance( it_chars_erase, 1 );

                chars.erase( it_chars_erase, chars.end() );

                VectorTextLineChunks2::iterator it_lines_erase = it_lines;
                std::advance( it_lines_erase, 1 );

                _textLines.erase( it_lines_erase, _textLines.end() );

                return;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::updateTextLinesDimension_( const TextFontInterfacePtr & _font, const VectorTextLineChunks2 & _textLines, mt::vec2f * _size, uint32_t * _charCount, uint32_t * _layoutCount ) const
    {
        float fontHeight = _font->getFontAscent();

        float charOffset = this->calcCharOffset();
        float lineOffset = this->calcLineOffset();

        VectorTextLinesLayout layouts;
        for( const VectorTextLineChunks & textChunks : _textLines )
        {
            VectorTextLines2 textLine2;
            for( const TextLineChunk & textChunk : textChunks )
            {
                const CacheFont & cache = m_cacheFonts[textChunk.fontId];

                const TextFontInterfacePtr & chunkFont = cache.font;

                uint32_t layoutCount = chunkFont->getLayoutCount();

                VectorTextLines textLine;
                for( uint32_t layoutIndex = 0; layoutIndex != layoutCount; ++layoutIndex )
                {
                    TextLine tl( layoutIndex, charOffset );
                    if( tl.initialize( textChunk.fontId, chunkFont, textChunk.value ) == false )
                    {
                        LOGGER_ERROR( "'%s' textID '%s' invalid setup line"
                            , this->getName().c_str()
                            , m_textId.c_str()
                        );

                        return false;
                    }

                    textLine.emplace_back( tl );
                }

                textLine2.emplace_back( textLine );
            }

            layouts.emplace_back( textLine2 );
        }

        uint32_t charCount = 0;

        m_textLineAlignOffsets.clear();

        float maxlen = 0.f;
        for( const VectorTextLines2 & lines2 : layouts )
        {
            float line_length = 0.f;
            uint32_t line_chars = 0;

            for( const VectorTextLines & lines : lines2 )
            {
                const TextLine & line = lines[0];

                line_length += line.getLength();
                line_chars += line.getCharsDataSize();
            }

            maxlen = (std::max)(maxlen, line_length);
            charCount += line_chars;

            float alignOffsetX = this->getHorizontAlignOffset_( lines2 );

            m_textLineAlignOffsets.push_back( alignOffsetX );
        }

        *_charCount = charCount;

        *_layoutCount = (uint32_t)layouts.size();

        mt::vec2f size;
        size.x = maxlen;

        VectorTextLinesLayout::size_type lineCount = layouts.size();

        if( lineCount > 0 )
        {
            size.y = (lineOffset + fontHeight) * (lineCount - 1) + fontHeight;
        }
        else
        {
            size.y = fontHeight;
        }

        *_size = size;

        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    bool TextField::updateTextLines_() const
    {
        m_invalidateTextLines = false;

        m_layouts.clear();
        m_textSize.x = 0.f;
        m_textSize.y = 0.f;
        m_charCount = 0;

        if( m_textId.empty() == true )
        {
            return true;
        }

        const TextFontInterfacePtr & baseFont = this->getFont();

        if( baseFont == nullptr )
        {
            return false;
        }

        U32String cacheText;
        if( this->updateTextCache_( cacheText ) == false )
        {
            LOGGER_ERROR( "font '%s' invalid update text cache '%s'"
                , this->getName().c_str()
                , m_textId.c_str()
            );

            return false;
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

        VectorTextLineChunks textChars;
        Helper::test( textChars, cacheText, m_cacheFonts, 0 );

        for( const CacheFont & cache : m_cacheFonts )
        {
            const TextFontInterfacePtr & font = cache.font;

            if( font->compileFont() == false )
            {
                return false;
            }
        }

        for( const TextLineChunk & tc : textChars )
        {
            const CacheFont & cache = m_cacheFonts[tc.fontId];

            const TextFontInterfacePtr & font = cache.font;

            if( font->prepareGlyph( tc.value ) == false )
            {
                return false;
            }
        }

        VectorU32String line_delims;
        line_delims.emplace_back( U"\n" );
        line_delims.emplace_back( U"\r\n" );
        line_delims.emplace_back( U"\n\r" );
        line_delims.emplace_back( U"\n\r\t" );

        VectorTextLineChunks2 textLines;
        Helper::split( textLines, textChars, line_delims );

        MENGINE_ASSERTION( !(m_autoScale == true && (m_wrap == true || m_maxCharCount != ~0)), "text '%s' invalid enable together attributes 'wrap' and 'scaleFactor'"
            , this->getName().c_str()
        );

        this->updateTextLinesWrap_( textLines );

        if( this->updateTextLinesDimension_( baseFont, textLines, &m_textSize, &m_charCount, &m_layoutCount ) == false )
        {
            return false;
        }

        m_autoScaleFactor = 1.f;

        if( m_autoScale == true )
        {
            if( m_textSize.x > m_maxLength )
            {
                m_autoScaleFactor = m_maxLength / m_textSize.x;
            }
        }

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

        float charOffset = this->calcCharOffset();

        for( const VectorTextLineChunks & textChunks : textLines )
        {
            VectorTextLines2 textLine2;
            for( const TextLineChunk & textChunk : textChunks )
            {
                const CacheFont & cache = m_cacheFonts[textChunk.fontId];

                const TextFontInterfacePtr & chunkFont = cache.font;

                uint32_t layoutCount = chunkFont->getLayoutCount();

                VectorTextLines textLine;
                for( uint32_t layoutIndex = 0; layoutIndex != layoutCount; ++layoutIndex )
                {
                    TextLine tl( layoutIndex, charOffset );
                    if( tl.initialize( textChunk.fontId, chunkFont, textChunk.value ) == false )
                    {
                        LOGGER_ERROR( "'%s' textID '%s' invalid setup line"
                            , this->getName().c_str()
                            , m_textId.c_str()
                        );

                        return false;
                    }

                    textLine.emplace_back( tl );
                }

                textLine2.emplace_back( textLine );
            }

            m_layouts.emplace_back( textLine2 );
        }

        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    bool TextField::updateFont_() const
    {
        m_invalidateFont = false;

        ConstString fontName = this->calcFontName();

        if( m_font != nullptr )
        {
            const ConstString & currentFontName = m_font->getName();

            if( fontName == currentFontName )
            {
                return true;
            }
            else
            {
                m_font->releaseFont();
                m_font = nullptr;
            }
        }

        if( fontName.empty() == true )
        {
            LOGGER_ERROR( "font '%s' invalid set font (no default?)"
                , this->getName().c_str()
            );

            return false;
        }

        const TextFontInterfacePtr & font = TEXT_SERVICE()
            ->getFont( fontName );

        MENGINE_ASSERTION_MEMORY_PANIC( font, false, "font '%s' can't found font '%s'"
            , this->getName().c_str()
            , fontName.c_str()
        );

        if( font->compileFont() == false )
        {
            LOGGER_ERROR( "font '%s' invalid compile font '%s'"
                , this->getName().c_str()
                , fontName.c_str()
            );

            return false;
        }

        m_font = font;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateTextEntry_() const
    {
        m_invalidateTextEntry = false;

        const ConstString & aliasTestId = TEXT_SERVICE()
            ->getTextAlias( m_aliasEnvironment, m_textId );

        TextEntryInterfacePtr textEntry = TEXT_SERVICE()
            ->getTextEntry( aliasTestId );

        MENGINE_ASSERTION_MEMORY_PANIC_VOID( textEntry, "'%s' can't find text ID '%s'"
            , this->getName().c_str()
            , aliasTestId.c_str()
        );

        m_textEntry = textEntry;
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

            if( params & EFP_LINE_OFFSET )
            {
                float fontHeight = font->getFontHeight();
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
    const Color & TextField::calcFontColor() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( params & EFP_COLOR_FONT )
            {
                const Color & value = textEntry->getColorFont();

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
                const Color & value = font->getFontColor();

                return value;
            }
        }

        return m_colorFont;
    }
    //////////////////////////////////////////////////////////////////////////
    ETextHorizontAlign TextField::calcHorizontAlign() const
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
    float TextField::calcLinesOffset( float _lineOffset, const TextFontInterfacePtr & _font ) const
    {
        float fontAscent = _font->getFontAscent();
        //float fontDescent = _font->getFontDescent();
        float fontHeight = _font->getFontHeight();
        float fontBearingYA = _font->getFontBearingYA();

        ETextVerticalAlign verticalAlign = this->calcVerticalAlign();

        float offset = 0.f;

        if( m_layoutCount == 1 )
        {
            switch( verticalAlign )
            {
            case ETFVA_BOTTOM:
                {
                    offset = fontAscent;
                }break;
            case ETFVA_CENTER:
                {
                    offset = fontBearingYA * 0.5f;
                }break;
            case ETFVA_NONE:
            case ETFVA_TOP:
                {
                    offset = 0.f;
                }break;
            }
        }
        else
        {
            float layoutCountf = float( m_layoutCount );
            float layoutCount1f = float( m_layoutCount - 1 );

            switch( verticalAlign )
            {
            case ETFVA_BOTTOM:
                {
                    offset = fontHeight - (fontHeight - fontAscent);
                }break;
            case ETFVA_CENTER:
                {
                    offset = fontHeight - (fontHeight - fontAscent) - fontHeight * layoutCountf * 0.5f - layoutCount1f * _lineOffset * 0.5f;
                }break;
            case ETFVA_NONE:
            case ETFVA_TOP:
                {
                    offset = fontHeight - fontHeight * layoutCountf + layoutCount1f * _lineOffset;
                }break;
            }
        }

        return offset;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::_updateBoundingBox( mt::box2f & _boundingBox, mt::box2f ** _boundingBoxCurrent ) const
    {
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

        MENGINE_UNUSED( _boundingBox );

        *_boundingBoxCurrent = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::getHorizontAlignOffset_( const VectorTextLines2 & _lines ) const
    {
        float length = 0.f;
        for( const VectorTextLines & line : _lines )
        {
            length += line[0].getLength();
        }

        float offset = 0.f;

        ETextHorizontAlign horizontAlign = this->calcHorizontAlign();

        switch( horizontAlign )
        {
        case ETFHA_NONE:
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
        m_textId = _key;

        m_textFormatArgs.clear();

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TextField::getTextID() const
    {
        return m_textId;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::removeTextID()
    {
        m_textEntry = nullptr;
        m_textFormatArgs.clear();

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TextField::getTextEntryID() const
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
    void TextField::setTextAliasEnvironment( const ConstString & _aliasEnvironment )
    {
        m_aliasEnvironment = _aliasEnvironment;

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TextField::getTextAliasEnvironment() const
    {
        return m_aliasEnvironment;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setTextFormatArgs( const VectorString & _args )
    {
        if( m_textFormatArgs == _args )
        {
            return;
        }

        m_textFormatArgs = _args;

        m_textFormatArgContexts.resize( _args.size() );
            
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
    const VectorString & TextField::getTextFormatArgs() const
    {
        return m_textFormatArgs;
    }
    //////////////////////////////////////////////////////////////////////////    
    void TextField::setTextFormatArgsContext( uint32_t _index, const LambdaFormatArgsContext & _context )
    {
        m_textFormatArgContexts[_index] = _context;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TextField::getTextExpectedArgument() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTextEntry();

        MENGINE_ASSERTION_MEMORY_PANIC( textEntry, 0, "'%s:%s' not compile"
            , this->getName().c_str()
            , m_textId.c_str()
        );

        size_t textSize;
        const Char * textValue = textEntry->getValue( &textSize );

        try
        {
            uint32_t expected_args = Helper::getStringFormatExpectedArgs( textValue, textSize );

            return expected_args;
        }
        catch( const std::exception & _ex )
        {
            LOGGER_ERROR( "'%s:%s' except error '%s'"
                , this->getName().c_str()
                , this->getTextEntryID().c_str()
                , _ex.what()
            );
        }

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////	
    bool TextField::updateTextCache_( U32String & _cacheText ) const
    {
        const TextEntryInterfacePtr & textEntry = this->getTextEntry();

        MENGINE_ASSERTION_MEMORY_PANIC( textEntry, false, "'%s:%s' invalid get text entry can't setup text ID"
            , this->getName().c_str()
            , m_textId.c_str()
        );

        const TextFontInterfacePtr & font = this->getFont();

        if( font == nullptr )
        {
            return false;
        }

        size_t textSize;
        const Char * textValue = textEntry->getValue( &textSize );

        TEXT_SERVICE()
            ->getTextAliasArguments( m_aliasEnvironment, m_textId, m_textFormatArgs );

        String fmt;
        if( Helper::getStringFormat( fmt, textValue, textSize, m_textFormatArgs ) == false )
        {
            LOGGER_ERROR( "invalid string '%s:%s' format with args '%d'"
                , this->getName().c_str()
                , this->getTextEntryID().c_str()
                , m_textFormatArgs.size()
            );

            return false;
        }

        _cacheText = font->prepareText( fmt.c_str(), fmt.size() );

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
    void TextField::setHorizontAlign( ETextHorizontAlign _horizontAlign )
    {
        m_horizontAlign = _horizontAlign;

        if( m_horizontAlign == ETFHA_NONE )
        {
            m_fontParams &= ~(EFP_HORIZONTAL_ALIGN);
        }
        else
        {
            m_fontParams |= EFP_HORIZONTAL_ALIGN;
        }

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    ETextHorizontAlign TextField::getHorizontAlign() const
    {
        return m_horizontAlign;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setVerticalAlign( ETextVerticalAlign _verticalAlign )
    {
        m_verticalAlign = _verticalAlign;

        if( m_verticalAlign == ETFVA_NONE )
        {
            m_fontParams &= ~(EFP_VERTICAL_ALIGN);
        }
        else
        {
            m_fontParams |= EFP_VERTICAL_ALIGN;
        }

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    ETextVerticalAlign TextField::getVerticalAlign() const
    {
        return m_verticalAlign;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setVerticalTopAlign()
    {
        m_verticalAlign = ETFVA_TOP;

        m_fontParams |= EFP_VERTICAL_ALIGN;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::isVerticalTopAlign() const
    {
        return m_verticalAlign == ETFVA_TOP;
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
    void TextField::updateVerticesWM_( const TextFontInterfacePtr & _font ) const
    {
        m_invalidateVerticesWM = false;

        if( m_invalidateVertices == true )
        {
            this->updateVertices_( _font );
        }

        this->updateVertexDataWM_( m_vertexDataTextWM, m_vertexDataText );
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateVertexDataWM_( VectorRenderVertex2D & _outVertex, const VectorRenderVertex2D & _fromVertex ) const
    {
        _outVertex.assign( _fromVertex.begin(), _fromVertex.end() );

        VectorRenderVertex2D::const_iterator it = _fromVertex.begin();
        VectorRenderVertex2D::const_iterator it_end = _fromVertex.end();

        VectorRenderVertex2D::iterator it_w = _outVertex.begin();

        if( m_pixelsnap == true )
        {
            mt::mat4f wm = this->getWorldMatrix();
            wm.v3.x = ::floorf( wm.v3.x + 0.5f );
            wm.v3.y = ::floorf( wm.v3.y + 0.5f );

            for( ; it != it_end; ++it, ++it_w )
            {
                const RenderVertex2D & vertex = *it;

                RenderVertex2D & vertex_w = *it_w;

                mt::mul_v3_v3_m4( vertex_w.position, vertex.position, wm );
            }
        }
        else
        {
            const mt::mat4f & wm = this->getWorldMatrix();

            for( ; it != it_end; ++it, ++it_w )
            {
                const RenderVertex2D & vertex = *it;

                RenderVertex2D & vertex_w = *it_w;

                mt::mul_v3_v3_m4( vertex_w.position, vertex.position, wm );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateVertices_( const TextFontInterfacePtr & _font ) const
    {
        m_invalidateVertices = false;

        Color colorNode;
        this->calcTotalColor( colorNode );

        this->updateVertexData_( _font, colorNode, m_vertexDataText );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr TextField::_updateMaterial() const
    {
        return nullptr;
    }
}

#include "TextField.h" 

#include "Interface/TextServiceInterface.h"
#include "Interface/FontServiceInterface.h"
#include "Interface/ApplicationInterface.h"

#include "TextLine.h"

#include "Kernel/StringFormat.h"
#include "Kernel/StringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/DocumentableHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionObservable.h"
#include "Kernel/NotificationHelper.h"

#include "Config/StdIntTypes.h"
#include "Config/StdIO.h"
#include "Config/Algorithm.h"
#include "Config/Iterator.h"
#include "Config/StdMath.h"

#include "math/box2.h"

#ifndef MENGINE_TEXT_FIELD_MAX_TEXT
#define MENGINE_TEXT_FIELD_MAX_TEXT 2048
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TextField::TextField()
        : m_horizontAlign( ETFHA_NONE )
        , m_verticalAlign( ETFVA_NONE )
        , m_invalidateFont( true )
        , m_extraThickness( 0.f )
        , m_charScale( 1.f )
        , m_maxLength( 2048.f )
        , m_autoScaleFactor( 1.f )
        , m_lineOffset( 0.f )
        , m_charOffset( 0.f )
        , m_anchorPercent( 0.f, 0.f )
        , m_textParams( EFP_NONE )
        , m_maxCharCount( MENGINE_UNKNOWN_SIZE )
        , m_charCount( 0 )
        , m_layoutCount( 0 )
        , m_textSize( 0.f, 0.f )
        , m_textOffset( 0.f, 0.f )
        , m_anchorHorizontalAlign( true )
        , m_anchorVerticalAlign( true )
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
        MENGINE_ASSERTION_OBSERVABLE( this, "text field '%s'"
            , this->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::_activate()
    {
        if( Node::_activate() == false )
        {
            return false;
        }

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_LOCALE_PREPARE, &TextField::notifyChangeLocalePrepare_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_LOCALE_POST, &TextField::notifyChangeLocalePost_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_DEBUG_TEXT_MODE, &TextField::notifyDebugMode_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_TEXT_ALIAS, &TextField::notifyChangeTextAliasArguments_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE, &TextField::notifyRenderDeviceLostPrepare_, MENGINE_DOCUMENT_FACTORABLE );

        this->invalidateTextLines();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::_deactivate()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_LOCALE_PREPARE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_LOCALE_POST );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEBUG_TEXT_MODE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_TEXT_ALIAS );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE );

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
        if( m_totalFont != nullptr )
        {
            m_totalFont->releaseFont();
            m_totalFont = nullptr;
        }

        m_chunks.clear();

        for( const CacheFont & cache : m_cacheFonts )
        {
            const FontInterfacePtr & font = cache.font;

            font->releaseFont();
        }

        m_cacheFonts.clear();

        m_totalTextEntry = nullptr;

        m_cacheTextChars.clear();
        m_cacheTextLines.clear();

        VectorTextLinesLayout layouts;
        m_layouts.swap( layouts );

        VectorRenderVertex2D vertexDataText;
        m_vertexDataText.swap( vertexDataText );

        VectorRenderVertex2D vertexDataTextWM;
        m_vertexDataTextWM.swap( vertexDataTextWM );
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::notifyChangeLocalePrepare_( const ConstString & _prevLocale, const ConstString & _currentlocale )
    {
        MENGINE_UNUSED( _prevLocale );
        MENGINE_UNUSED( _currentlocale );

        if( m_totalFont != nullptr )
        {
            m_totalFont->releaseFont();
            m_totalFont = nullptr;
        }

        for( const CacheFont & cache : m_cacheFonts )
        {
            const FontInterfacePtr & font = cache.font;

            font->releaseFont();
        }

        m_cacheFonts.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::notifyChangeLocalePost_( const ConstString & _prevLocale, const ConstString & _currentlocale )
    {
        MENGINE_UNUSED( _prevLocale );
        MENGINE_UNUSED( _currentlocale );

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::notifyDebugMode_( bool _debugMode )
    {
        m_debugMode = _debugMode;

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::notifyChangeTextAliasArguments_( const ConstString & _environment, const ConstString & _alias )
    {
        if( m_aliasEnvironment != _environment || m_textId != _alias )
        {
            return;
        }

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::notifyRenderDeviceLostPrepare_()
    {
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
    void TextField::_invalidateWorldMatrix() const
    {
        this->invalidateVerticesWM_();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateVertexData_( const FontInterfacePtr & _font, const Color & _color, VectorRenderVertex2D * const _vertexData ) const
    {
        _vertexData->clear();
        m_chunks.clear();

        const VectorTextLinesLayout & layouts = this->getTextLayots();

        if( layouts.empty() == true )
        {
            return;
        }

        float fontAscent = _font->getFontHeight();
        //float fontAscent = _font->getFontAscent();

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

        ColorValue_ARGB cacheFontARGB[16] = {'\0'};

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

            const FontInterfacePtr & font = cache.font;

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

        mt::vec2f baseOffset;
        baseOffset.x = 0.f;
        baseOffset.y = linesOffset * m_autoScaleFactor;

        TextRenderChunk chunk;
        chunk.vertex_begin = 0;
        chunk.vertex_count = 0;
        chunk.material = nullptr;

        mt::vec2f offset = baseOffset;

        float charScale = this->calcCharScale();

        float charScaleTotal = charScale * m_autoScaleFactor;

        for( const VectorTextLines2 & lines2 : layouts )
        {
            float alignOffsetX = this->getHorizontAlignOffset_( lines2 );

            offset.x = alignOffsetX * charScaleTotal;

            for( const VectorTextLines & lines : lines2 )
            {
                mt::vec2f offset2( 0.f, 0.f );

                for( const TextLine & line : lines )
                {
                    offset2 = offset;

                    const VectorTextCharData & charsData = line.getCharsData();

                    for( const TextCharData & cd : charsData )
                    {
                        if( cd.texture == nullptr )
                        {
                            line.advanceCharOffset( cd, charScaleTotal, &offset2 );

                            continue;
                        }

                        uint32_t argb = cacheFontARGB[cd.fontId];

                        for( uint32_t i = 0; i != 4; ++i )
                        {
                            RenderVertex2D v;

                            line.calcCharPosition( cd, offset2, charScaleTotal, i, &v.position );

                            v.color = argb;
                            v.uv[0] = cd.uv[i];

                            _vertexData->emplace_back( v );
                        }

                        line.advanceCharOffset( cd, charScaleTotal, &offset2 );

                        RenderMaterialInterfacePtr material = this->getMaterial3( materialId, PT_TRIANGLELIST, 1, &cd.texture, MENGINE_DOCUMENT_FORWARD );

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

            offset.y += fontAscent;
            offset.y += lineOffset;
        }

        if( chunk.vertex_count != 0 )
        {
            m_chunks.emplace_back( chunk );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateContext_() const
    {
        bool context_invalidate = false;
        uint32_t enumerate = 0;
        for( const LambdaFormatArgsContext & context : m_textFormatArgContexts )
        {
            if( context != nullptr )
            {
                String & arg = m_textFormatArgs[enumerate];
                context_invalidate |= context( &arg );
            }

            ++enumerate;
        }

        if( context_invalidate == true )
        {
            this->invalidateTextLines();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        if( m_textId.empty() == true && m_text.empty() == true )
        {
            return;
        }

        if( APPLICATION_SERVICE()
            ->getTextEnable() == false )
        {
            return;
        }

        const FontInterfacePtr & font = this->getTotalFont();

        if( font == nullptr )
        {
            return;
        }

        this->updateContext_();

        const VectorRenderVertex2D & textVertices = this->getTextVerticesWM( font );

        if( textVertices.empty() == true )
        {
            return;
        }

        const VectorRenderVertex2D::value_type * vertices = textVertices.data();

        const mt::box2f * bb = this->getBoundingBox();

        for( const TextRenderChunk & chunk : m_chunks )
        {
            const VectorRenderVertex2D::value_type * chunk_vertices = vertices + chunk.vertex_begin;

            if( (chunk_vertices[0].color & 0xFF000000) == 0 )
            {
                continue;
            }

            _renderPipeline->addRenderQuad( _context, chunk.material, chunk_vertices, chunk.vertex_count, bb, false, MENGINE_DOCUMENT_FORWARD );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TextField::getCharCount() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "text field '%s' not compile"
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
        if( m_maxLength == _maxLength )
        {
            return;
        }

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
        if( m_autoScale == _autoScale )
        {
            return;
        }

        m_autoScale = _autoScale;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::getAutoScale() const
    {
        return m_autoScale;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setAnchorHorizontalAlign( bool _anchorHorizontalAlign )
    {
        if( m_anchorHorizontalAlign == _anchorHorizontalAlign )
        {
            return;
        }

        m_anchorHorizontalAlign = _anchorHorizontalAlign;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::getAnchorHorizontalAlign() const
    {
        return m_anchorHorizontalAlign;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setAnchorVerticalAlign( bool _anchorVerticalAlign )
    {
        if( m_anchorVerticalAlign == _anchorVerticalAlign )
        {
            return;
        }

        m_anchorVerticalAlign = _anchorVerticalAlign;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::getAnchorVerticalAlign() const
    {
        return m_anchorVerticalAlign;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setAnchorPercent( const mt::vec2f & _anchorPercent )
    {
        if( m_anchorPercent == _anchorPercent )
        {
            return;
        }

        m_anchorPercent = _anchorPercent;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & TextField::getAnchorPercent() const
    {
        return m_anchorPercent;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setWrap( bool _wrap )
    {
        if( m_wrap == _wrap )
        {
            return;
        }

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
        const FontInterfacePtr & font = this->getTotalFont();

        if( font == nullptr )
        {
            return 0.f;
        }

        float fontHeight = font->getFontHeight();

        return fontHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setFont( const FontInterfacePtr & _font )
    {
        if( m_font == _font )
        {
            return;
        }

        m_font = _font;

        if( m_font != nullptr )
        {
            m_textParams |= EFP_FONT;
        }
        else
        {
            m_textParams &= ~EFP_FONT;
        }

        this->invalidateFont();
        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    const FontInterfacePtr & TextField::getFont() const
    {
        return m_font;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setFontColor( const Color & _color )
    {
        if( m_colorFont == _color )
        {
            return;
        }

        m_colorFont = _color;

        m_textParams |= EFP_COLOR_FONT;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & TextField::getFontColor() const
    {
        return m_colorFont;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::hasFontColor() const
    {
        return (m_textParams & EFP_COLOR_FONT) == EFP_COLOR_FONT;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::removeFontColor()
    {
        if( (m_textParams & EFP_COLOR_FONT) == 0 )
        {
            return;
        }

        m_textParams &= ~EFP_COLOR_FONT;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setLineOffset( float _offset )
    {
        if( m_lineOffset == _offset )
        {
            return;
        }

        m_lineOffset = _offset;

        m_textParams |= EFP_LINE_OFFSET;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::getLineOffset() const
    {
        return m_lineOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::hasLineOffset() const
    {
        return (m_textParams & EFP_LINE_OFFSET) == EFP_LINE_OFFSET;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::removeLineOffset()
    {
        if( (m_textParams & EFP_LINE_OFFSET) == 0 )
        {
            return;
        }

        m_textParams &= ~EFP_LINE_OFFSET;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setCharOffset( float _offset )
    {
        if( m_charOffset == _offset )
        {
            return;
        }

        m_charOffset = _offset;

        m_textParams |= EFP_CHAR_OFFSET;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::getCharOffset() const
    {
        return m_charOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::hasCharOffset() const
    {
        return (m_textParams & EFP_CHAR_OFFSET) == EFP_CHAR_OFFSET;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::removeCharOffset()
    {
        if( (m_textParams & EFP_CHAR_OFFSET) == 0 )
        {
            return;
        }

        m_textParams &= ~EFP_CHAR_OFFSET;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setCharScale( float _value )
    {
        if( m_charScale == _value )
        {
            return;
        }

        m_charScale = _value;

        m_textParams |= EFP_CHAR_SCALE;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::getCharScale() const
    {
        return m_charScale;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::hasCharScale() const
    {
        return (m_textParams & EFP_CHAR_SCALE) == EFP_CHAR_SCALE;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::removeCharScale()
    {
        if( (m_textParams & EFP_CHAR_SCALE) == 0 )
        {
            return;
        }

        m_textParams &= ~EFP_CHAR_SCALE;

        this->invalidateTextLines();
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
    const mt::vec2f & TextField::getTextOffset() const
    {
        if( this->isInvalidateTextLines() == true )
        {
            this->updateTextLines_();
        }

        return m_textOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::calcTextViewport( Viewport * const _viewport ) const
    {
        if( m_textId.empty() == true && m_text.empty() == true )
        {
            return false;
        }

        const FontInterfacePtr & font = this->getTotalFont();

        if( font == nullptr )
        {
            return false;
        }

        this->updateContext_();

        const VectorRenderVertex2D & textVertices = this->getTextVertices( font );

        if( textVertices.empty() == true )
        {
            return false;
        }

        mt::box2f box;
        mt::insideout_box( &box );

        for( const RenderVertex2D & vertex : textVertices )
        {
            mt::add_internal_point( &box, vertex.position.x, vertex.position.y );
        }

        _viewport->begin = box.minimum;
        _viewport->end = box.maximum;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::calcTotalTextSize( mt::vec2f * const _textSize ) const
    {
        if( this->isInvalidateTextLines() == true )
        {
            this->updateTextLines_();
        }

        *_textSize = m_textSize * m_autoScaleFactor;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateTextLinesWrap_( VectorTextLineChunks2 * const _textLines ) const
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
        for( const VectorTextLineChunks & textChunks : *_textLines )
        {
            float length = 0.f;

            VectorTextLineChunks new_textChunks;
            for( const TextLineChunk & textChunk : textChunks )
            {
                const U32String & text = textChunk.value;
                const CacheFont & cache = m_cacheFonts[textChunk.fontId];

                const FontInterfacePtr & font = cache.font;

                VectorU32String words;
                Helper::u32split2( &words, text, false, space_delims );

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
                        Iterator::advance( it_kerning, 1 );

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

        _textLines->swap( new_textLines );
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateTextLinesMaxCount_( VectorTextLineChunks2 * const _textLines ) const
    {
        if( m_maxCharCount == MENGINE_UNKNOWN_SIZE )
        {
            return;
        }

        uint32_t charIterator = 0;
        for( VectorTextLineChunks2::iterator
            it_lines = _textLines->begin(),
            it_lines_end = _textLines->end();
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
                Iterator::advance( it_chars_erase, 1 );

                chars.erase( it_chars_erase, chars.end() );

                VectorTextLineChunks2::iterator it_lines_erase = it_lines;
                Iterator::advance( it_lines_erase, 1 );

                _textLines->erase( it_lines_erase, _textLines->end() );

                return;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::updateTextLinesDimension_( const FontInterfacePtr & _font, const VectorTextLineChunks2 & _textLines ) const
    {
        float charOffset = this->calcCharOffset();
        float lineOffset = this->calcLineOffset();

        VectorTextLinesLayout layouts;
        for( const VectorTextLineChunks & textChunks : _textLines )
        {
            VectorTextLines2 textLines2;
            for( const TextLineChunk & textChunk : textChunks )
            {
                const CacheFont & cache = m_cacheFonts[textChunk.fontId];

                const FontInterfacePtr & chunkFont = cache.font;

                uint32_t layoutCount = chunkFont->getLayoutCount();

                VectorTextLines textLines;
                for( uint32_t layoutIndex = 0; layoutIndex != layoutCount; ++layoutIndex )
                {
                    TextLine tl( layoutIndex, charOffset );
                    if( tl.initialize( textChunk.fontId, chunkFont, textChunk.value ) == false )
                    {
                        LOGGER_ERROR( "text field '%s' textId '%s' text '%s' invalid setup line"
                            , this->getName().c_str()
                            , m_textId.c_str()
                            , m_text.c_str()
                        );

                        return false;
                    }

                    textLines.emplace_back( tl );
                }

                textLines2.emplace_back( textLines );
            }

            layouts.emplace_back( textLines2 );
        }

        if( layouts.empty() == true )
        {
            m_charCount = 0;
            m_layoutCount = 0;
            m_textSize = mt::vec2f( 0.f, 0.f );
            m_textOffset = mt::vec2f( 0.f, 0.f );

            return true;
        }

        m_layoutCount = (uint32_t)layouts.size();

        uint32_t charCount = 0;

        const VectorTextLines2 & layout0 = layouts[0];

        float text_length = 0.f;
        float text_offset = this->getHorizontAlignOffset_( layout0 );

        for( const VectorTextLines2 & lines2 : layouts )
        {
            float line_length = 0.f;
            uint32_t line_chars = 0;

            for( const VectorTextLines & lines : lines2 )
            {
                const TextLine & line0 = lines[0];

                line_length += line0.getLength();
                line_chars += line0.getCharsDataSize();
            }

            float line_offset = this->getHorizontAlignOffset_( lines2 );

            text_length = MENGINE_MAX( text_length, line_length );
            text_offset = MENGINE_MIN( text_offset, line_offset );
            charCount += line_chars;
        }

        m_charCount = charCount;

        mt::vec2f textSize;
        textSize.x = text_length;

        float fontHeight = _font->getFontHeight();
        //float fontAscent = _font->getFontAscent();

        if( m_layoutCount == 0 )
        {
            textSize.y = 0.f;
        }
        else if( m_layoutCount == 1 )
        {
            textSize.y = fontHeight + m_extraThickness * 2.f;
        }
        else
        {
            textSize.y = (lineOffset + fontHeight) * (m_layoutCount - 1) + fontHeight + m_extraThickness * 2.f;
        }

        m_textSize = textSize;

        float lines_offset = this->calcLinesOffset( lineOffset, _font );

        mt::vec2f textOffset;
        textOffset.x = text_offset;
        textOffset.y = lines_offset * m_autoScaleFactor;

        m_textOffset = textOffset;

        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    bool TextField::updateTextLines_() const
    {
        m_invalidateTextLines = false;

        m_layouts.clear();
        m_textSize.x = 0.f;
        m_textSize.y = 0.f;
        m_textOffset.x = 0.f;
        m_textOffset.y = 0.f;
        m_charCount = 0;

        if( m_textId.empty() == true && m_text.empty() == true )
        {
            return true;
        }

        const FontInterfacePtr & baseFont = this->getTotalFont();

        if( baseFont == nullptr )
        {
            return false;
        }

        m_cacheText.clear();

        if( this->updateTextCache_( &m_cacheText ) == false )
        {
            LOGGER_ERROR( "text field '%s' invalid update text cache id '%s' text '%s'"
                , this->getName().c_str()
                , m_textId.c_str()
                , m_text.c_str()
            );

            return false;
        }

        for( const CacheFont & cache : m_cacheFonts )
        {
            const FontInterfacePtr & font = cache.font;

            font->releaseFont();
        }

        m_cacheFonts.clear();

        CacheFont baseCacheFont;
        baseCacheFont.font = baseFont;

        m_cacheFonts.emplace_back( baseCacheFont );

        m_cacheTextChars.clear();
        Helper::test( &m_cacheTextChars, m_cacheText, &m_cacheFonts, 0 );

        for( const CacheFont & cache : m_cacheFonts )
        {
            const FontInterfacePtr & font = cache.font;

            if( font->compileFont() == false )
            {
                return false;
            }
        }

        for( const TextLineChunk & tc : m_cacheTextChars )
        {
            uint32_t fontId = tc.fontId;

            const CacheFont & cache = m_cacheFonts[fontId];

            const FontInterfacePtr & font = cache.font;

            if( font->prepareGlyph( tc.value, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                return false;
            }
        }

        const VectorU32String & lineDelims = TEXT_SERVICE()
            ->getLineDelims();

        m_cacheTextLines.clear();
        Helper::split( &m_cacheTextLines, m_cacheTextChars, lineDelims );

        MENGINE_ASSERTION( !(m_autoScale == true && (m_wrap == true || m_maxCharCount != MENGINE_UNKNOWN_SIZE)), "text '%s' invalid enable together attributes 'wrap' and 'autoScale'"
            , this->getName().c_str()
        );

        this->updateTextLinesWrap_( &m_cacheTextLines );

        if( this->updateTextLinesDimension_( baseFont, m_cacheTextLines ) == false )
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

        this->updateTextLinesMaxCount_( &m_cacheTextLines );

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

        for( const VectorTextLineChunks & textChunks : m_cacheTextLines )
        {
            VectorTextLines2 textLine2;
            for( const TextLineChunk & textChunk : textChunks )
            {
                uint32_t fontId = textChunk.fontId;

                const CacheFont & cache = m_cacheFonts[fontId];

                const FontInterfacePtr & chunkFont = cache.font;

                uint32_t fontLayoutCount = chunkFont->getLayoutCount();

                VectorTextLines textLine;
                for( uint32_t layoutIndex = 0; layoutIndex != fontLayoutCount; ++layoutIndex )
                {
                    TextLine tl( layoutIndex, charOffset );
                    if( tl.initialize( fontId, chunkFont, textChunk.value ) == false )
                    {
                        LOGGER_ERROR( "text field '%s' textId '%s' text '%s' invalid setup line"
                            , this->getName().c_str()
                            , m_textId.c_str()
                            , m_text.c_str()
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

        const FontInterfacePtr & font = this->calcFont();

        if( m_totalFont != nullptr )
        {
            if( font == m_totalFont )
            {
                return true;
            }
            else
            {
                m_totalFont->releaseFont();
                m_totalFont = nullptr;
            }
        }

        if( font == nullptr )
        {
            LOGGER_ERROR( "text field '%s' invalid set font (no default?)"
                , this->getName().c_str()
            );

            return false;
        }

        if( font->compileFont() == false )
        {
            LOGGER_ERROR( "text field '%s' invalid compile font '%s'"
                , this->getName().c_str()
                , font->getName().c_str()
            );

            return false;
        }

        m_totalFont = font;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateTextEntry_() const
    {
        m_invalidateTextEntry = false;

        const ConstString & aliasTestId = TEXT_SERVICE()
            ->getTextAlias( m_aliasEnvironment, m_textId );

        if( aliasTestId.empty() == true )
        {
            m_totalTextEntry = nullptr;

            return;
        }

        const TextEntryInterfacePtr & textEntry = TEXT_SERVICE()
            ->getTextEntry( aliasTestId );

        MENGINE_ASSERTION_MEMORY_PANIC( textEntry, "text field '%s' can't find textId '%s' (doc: %s)"
            , this->getName().c_str()
            , aliasTestId.c_str()
            , MENGINE_DOCUMENTABLE_STR( this, "TextField" )
        );

        m_totalTextEntry = textEntry;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TextField::getTextFieldParams() const
    {
        return m_textParams;
    }
    //////////////////////////////////////////////////////////////////////////
    const FontInterfacePtr & TextField::calcFont() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTotalTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( (params & EFP_FONT) == EFP_FONT )
            {
                const FontInterfacePtr & font = textEntry->getFont();

                return font;
            }
        }

        if( m_font == nullptr )
        {
            const FontInterfacePtr & defaultFont = FONT_SERVICE()
                ->getDefaultFont();

            return defaultFont;
        }

        return m_font;
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::calcLineOffset() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTotalTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( (params & EFP_LINE_OFFSET) == EFP_LINE_OFFSET )
            {
                float value = textEntry->getLineOffset();

                return value;
            }
        }

        if( (m_textParams & EFP_LINE_OFFSET) == EFP_LINE_OFFSET )
        {
            return m_lineOffset;
        }

        const FontInterfacePtr & font = this->getTotalFont();

        if( font != nullptr )
        {
            uint32_t params = font->getFontParams();

            if( (params & EFP_LINE_OFFSET) == EFP_LINE_OFFSET )
            {
                float lineOffset = font->getLineOffset();

                return lineOffset;
            }
        }

        return 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::calcCharOffset() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTotalTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( (params & EFP_CHAR_OFFSET) == EFP_CHAR_OFFSET )
            {
                float value = textEntry->getCharOffset();

                return value;
            }
        }

        if( (m_textParams & EFP_CHAR_OFFSET) == EFP_CHAR_OFFSET )
        {
            return m_charOffset;
        }

        const FontInterfacePtr & font = this->getTotalFont();

        if( font != nullptr )
        {
            uint32_t params = font->getFontParams();

            if( (params & EFP_CHAR_OFFSET) == EFP_CHAR_OFFSET )
            {
                float value = font->getCharOffset();

                return value;
            }
        }

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::calcMaxLength() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTotalTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( (params & EFP_MAX_LENGTH) == EFP_MAX_LENGTH )
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
        const TextEntryInterfacePtr & textEntry = this->getTotalTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( (params & EFP_COLOR_FONT) == EFP_COLOR_FONT )
            {
                const Color & value = textEntry->getColorFont();

                return value;
            }
        }

        if( (m_textParams & EFP_COLOR_FONT) == EFP_COLOR_FONT )
        {
            return m_colorFont;
        }

        const FontInterfacePtr & font = this->getTotalFont();

        if( font != nullptr )
        {
            uint32_t params = font->getFontParams();

            if( (params & EFP_COLOR_FONT) == EFP_COLOR_FONT )
            {
                const Color & value = font->getFontColor();

                return value;
            }
        }

        return Color::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    ETextHorizontAlign TextField::calcHorizontAlign() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTotalTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( (params & EFP_HORIZONTAL_ALIGN) == EFP_HORIZONTAL_ALIGN )
            {
                ETextHorizontAlign value = textEntry->getHorizontAlign();

                return value;
            }
        }

        if( (m_textParams & EFP_HORIZONTAL_ALIGN) == EFP_HORIZONTAL_ALIGN )
        {
            return m_horizontAlign;
        }

        return ETFHA_NONE;
    }
    //////////////////////////////////////////////////////////////////////////
    ETextVerticalAlign TextField::calcVerticalAlign() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTotalTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( (params & EFP_VERTICAL_ALIGN) == EFP_VERTICAL_ALIGN )
            {
                ETextVerticalAlign value = textEntry->getVerticalAlign();

                return value;
            }
        }

        if( (m_textParams & EFP_VERTICAL_ALIGN) == EFP_VERTICAL_ALIGN )
        {
            return m_verticalAlign;
        }

        return ETFVA_NONE;
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::calcCharScale() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTotalTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( (params & EFP_CHAR_SCALE) == EFP_CHAR_SCALE )
            {
                float value = textEntry->getCharScale();

                return value;
            }
        }

        if( (m_textParams & EFP_CHAR_SCALE) == EFP_CHAR_SCALE )
        {
            return m_charScale;
        }

        return 1.f;
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::calcLinesOffset( float _lineOffset, const FontInterfacePtr & _font ) const
    {
        float fontAscender = _font->getFontAscender();
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
                    offset = fontAscender + m_extraThickness * 2.f;
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
            float layoutCountf = (float)m_layoutCount;

            switch( verticalAlign )
            {
            case ETFVA_BOTTOM:
                {
                    offset = fontAscender + m_extraThickness * 2.f;
                }break;
            case ETFVA_CENTER:
                {
                    offset = fontAscender - (fontHeight + _lineOffset) * layoutCountf * 0.5f;

                    if( m_anchorVerticalAlign == false )
                    {
                        offset += m_textSize.y * 0.5f;
                    }
                }break;
            case ETFVA_NONE:
            case ETFVA_TOP:
                {
                    offset = -(fontHeight + _lineOffset) * layoutCountf;

                    if( m_anchorVerticalAlign == false )
                    {
                        offset += m_textSize.y;
                    }
                }break;
            }
        }

        return offset;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::_updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const
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

                if( m_anchorHorizontalAlign == false )
                {
                    offset += m_textSize.x * 0.5f;
                }
            }break;
        case ETFHA_RIGHT:
            {
                offset = -length;

                if( m_anchorHorizontalAlign == false )
                {
                    offset += m_textSize.x;
                }
            }break;
        }

        return offset;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::_invalidateColor() const
    {
        this->invalidateVertices_();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::_dispose()
    {
        m_textFormatArgs.clear();
        m_textFormatArgContexts.clear();

        m_font = nullptr;

        m_layouts.clear();
        m_chunks.clear();

        for( const CacheFont & cache : m_cacheFonts )
        {
            const FontInterfacePtr & font = cache.font;

            font->releaseFont();
        }

        m_cacheFonts.clear();

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TextField::getMaxCharCount() const
    {
        return m_maxCharCount;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setExtraThickness( float _extraThickness )
    {
        m_extraThickness = _extraThickness;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::getExtraThickness() const
    {
        return m_extraThickness;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setMaxCharCount( uint32_t _maxCharCount )
    {
        if( m_maxCharCount == _maxCharCount )
        {
            return;
        }

        m_maxCharCount = _maxCharCount;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setTextId( const ConstString & _textId )
    {
        if( m_textId == _textId )
        {
            return;
        }

        m_textId = _textId;

        m_text.clear();
        m_textFormatArgs.clear();

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TextField::getTextId() const
    {
        return m_textId;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::removeTextId()
    {
        m_textId.clear();

        m_totalTextEntry = nullptr;
        m_textFormatArgs.clear();

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TextField::getTotalTextId() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTotalTextEntry();

        if( textEntry == nullptr )
        {
            return ConstString::none();
        }

        const ConstString & key = textEntry->getKey();

        return key;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setText( const String & _text )
    {
        if( m_text == _text )
        {
            return;
        }

        m_text = _text;

        m_textId = ConstString::none();
        m_textFormatArgs.clear();

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    const String & TextField::getText() const
    {
        return m_text;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setTextFormat( const Char * _format, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        Char str[MENGINE_TEXT_FIELD_MAX_TEXT] = {'\0'};
        int32_t size_vsnprintf = MENGINE_VSNPRINTF( str, MENGINE_TEXT_FIELD_MAX_TEXT, _format, args );

        MENGINE_VA_LIST_END( args );

        if( size_vsnprintf < 0 )
        {
            LOGGER_ERROR( "text field '%s' invalid set text format '%s'"
                , this->getName().c_str()
                , _format
            );

            return;
        }

        this->setText( String( str, size_vsnprintf ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::removeText()
    {
        if( m_text.empty() == true )
        {
            return;
        }

        m_text.clear();

        m_textId = ConstString::none();
        m_textFormatArgs.clear();

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::calcText( String * const _text ) const
    {
        const TextEntryInterfacePtr & textEntry = this->getTotalTextEntry();

        size_t textSize;
        const Char * textValue;

        VectorString textFormatArgs;

        if( textEntry == nullptr )
        {
            textSize = m_text.size();
            textValue = m_text.c_str();
        }
        else
        {
            textValue = textEntry->getValue( &textSize );

            if( TEXT_SERVICE()
                ->getTextAliasArguments( m_aliasEnvironment, m_textId, &textFormatArgs ) == false )
            {
                textFormatArgs = m_textFormatArgs;
            }
        }

        String fmt;
        if( Helper::fillStringFormat( textValue, textSize, textFormatArgs, &fmt ) == false )
        {
            LOGGER_ERROR( "text field '%s' textId '%s' (base '%s') invalid formating string text '%s' format with args %" MENGINE_PRIuPTR " [alias env: %s]"
                , this->getName().c_str()
                , this->getTotalTextId().c_str()
                , m_textId.c_str()
                , textValue
                , textFormatArgs.size()
                , m_aliasEnvironment.c_str()
            );

            for( const String & arg : m_textFormatArgs )
            {
                LOGGER_ERROR( "text field '%s' arg: %s"
                    , this->getName().c_str()
                    , arg.c_str()
                );
            }

            return false;
        }

        const Char * tag_id = "id";

        if( fmt.find( "<id>" ) == String::npos )
        {
            *_text = fmt;

            return true;
        }

        for( ;; )
        {
            bool fmtTagMatched;
            if( Helper::fillStringTag( fmt, tag_id, [this]( const String & _value, String * const _out )
            {
                ConstString tagTextId = Helper::stringizeString( _value );

                const ConstString & aliasTestId = TEXT_SERVICE()
                    ->getTextAlias( m_aliasEnvironment, tagTextId );

                if( aliasTestId.empty() == true )
                {
                    return false;
                }

                const TextEntryInterfacePtr & textEntry = TEXT_SERVICE()
                    ->getTextEntry( aliasTestId );

                size_t textSize;
                const Char * textValue = textEntry->getValue( &textSize );

                _out->assign( textValue, textSize );

                return true;
            }, _text, &fmtTagMatched ) == false )
            {
                LOGGER_ERROR( "text field '%s' textId '%s' (base '%s') invalid tagging string format '%s' with tag '%s' [alias env: %s]"
                    , this->getName().c_str()
                    , this->getTotalTextId().c_str()
                    , m_textId.c_str()
                    , fmt.c_str()
                    , tag_id
                    , m_aliasEnvironment.c_str()
                );

                return false;
            }

            if( fmtTagMatched == false )
            {
                break;
            }

            fmt = *_text;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setTextAliasEnvironment( const ConstString & _aliasEnvironment )
    {
        if( m_aliasEnvironment == _aliasEnvironment )
        {
            return;
        }

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

        VectorString::size_type textFormatArgsSize = m_textFormatArgs.size();

        m_textFormatArgContexts.resize( textFormatArgsSize );

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
    void TextField::removeTextFormatArgsContexts()
    {
        m_textFormatArgContexts.clear();

        VectorString::size_type textFormatArgsSize = m_textFormatArgs.size();

        m_textFormatArgContexts.resize( textFormatArgsSize );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TextField::getTextExpectedArgument() const
    {
        size_t textSize;
        const Char * textValue;

        const TextEntryInterfacePtr & textEntry = this->getTotalTextEntry();

        if( textEntry == nullptr )
        {
            textSize = m_text.size();
            textValue = m_text.c_str();
        }
        else
        {
            textValue = textEntry->getValue( &textSize );
        }

        try
        {
            uint32_t expected_args = Helper::getStringFormatExpectedArgs( textValue, textSize );

            return expected_args;
        }
        catch( const std::exception & _ex )
        {
            LOGGER_ERROR( "text field '%s' textId '%s' text '%s' except error '%s'"
                , this->getName().c_str()
                , this->getTotalTextId().c_str()
                , m_text.c_str()
                , _ex.what()
            );
        }

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::updateTextCache_( U32String * const _cacheText ) const
    {
        const FontInterfacePtr & font = this->getTotalFont();

        if( font == nullptr )
        {
            return false;
        }

        if( m_debugMode == true )
        {
            const ConstString & textId = this->getTextId();

            if( textId == ConstString::none() )
            {
                const Char notextId[] = "[NONE]";

                if( font->prepareText( notextId, MENGINE_STATIC_STRING_LENGTH( notextId ), _cacheText ) == false )
                {
                    LOGGER_ERROR( "text field '%s' textId '%s' (base '%s') invalid prepare text '%s' [alias env: %s]"
                        , this->getName().c_str()
                        , this->getTotalTextId().c_str()
                        , m_textId.c_str()
                        , notextId
                        , m_aliasEnvironment.c_str()
                    );

                    return false;
                }
            }
            else
            {
                if( font->prepareText( textId.c_str(), textId.size(), _cacheText ) == false )
                {
                    LOGGER_ERROR( "text field '%s' textId '%s' (base '%s') invalid prepare text '%s' [alias env: %s]"
                        , this->getName().c_str()
                        , this->getTotalTextId().c_str()
                        , m_textId.c_str()
                        , textId.c_str()
                        , m_aliasEnvironment.c_str()
                    );

                    return false;
                }
            }

            return true;
        }

        String text;
        if( this->calcText( &text ) == false )
        {
            return false;
        }

        const Char * text_str = text.c_str();
        size_t text_size = text.size();

        if( font->prepareText( text_str, text_size, _cacheText ) == false )
        {
            LOGGER_ERROR( "text field '%s' textId '%s' (base '%s') invalid prepare text '%s' [alias env: %s]"
                , this->getName().c_str()
                , this->getTotalTextId().c_str()
                , m_textId.c_str()
                , text.c_str()
                , m_aliasEnvironment.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setPixelsnap( bool _pixelsnap )
    {
        if( m_pixelsnap == _pixelsnap )
        {
            return;
        }

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
        if( m_horizontAlign == _horizontAlign )
        {
            return;
        }

        m_horizontAlign = _horizontAlign;

        if( m_horizontAlign == ETFHA_NONE )
        {
            m_textParams &= ~(EFP_HORIZONTAL_ALIGN);
        }
        else
        {
            m_textParams |= EFP_HORIZONTAL_ALIGN;
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
        if( m_verticalAlign == _verticalAlign )
        {
            return;
        }

        m_verticalAlign = _verticalAlign;

        if( m_verticalAlign == ETFVA_NONE )
        {
            m_textParams &= ~(EFP_VERTICAL_ALIGN);
        }
        else
        {
            m_textParams |= EFP_VERTICAL_ALIGN;
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
        if( m_verticalAlign == ETFVA_TOP )
        {
            return;
        }

        m_verticalAlign = ETFVA_TOP;

        m_textParams |= EFP_VERTICAL_ALIGN;

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
        if( m_verticalAlign == ETFVA_CENTER )
        {
            return;
        }

        m_verticalAlign = ETFVA_CENTER;

        m_textParams |= EFP_VERTICAL_ALIGN;

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
        if( m_verticalAlign == ETFVA_BOTTOM )
        {
            return;
        }

        m_verticalAlign = ETFVA_BOTTOM;

        m_textParams |= EFP_VERTICAL_ALIGN;

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
        if( m_horizontAlign == ETFHA_CENTER )
        {
            return;
        }

        m_horizontAlign = ETFHA_CENTER;

        m_textParams |= EFP_HORIZONTAL_ALIGN;

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
        if( m_horizontAlign == ETFHA_RIGHT )
        {
            return;
        }

        m_horizontAlign = ETFHA_RIGHT;

        m_textParams |= EFP_HORIZONTAL_ALIGN;

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
        if( m_horizontAlign == ETFHA_LEFT )
        {
            return;
        }

        m_horizontAlign = ETFHA_LEFT;

        m_textParams |= EFP_HORIZONTAL_ALIGN;

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
    void TextField::updateVerticesWM_( const FontInterfacePtr & _font ) const
    {
        m_invalidateVerticesWM = false;

        if( m_invalidateVertices == true )
        {
            this->updateVertices_( _font );
        }

        this->updateVertexDataWM_( &m_vertexDataTextWM, m_vertexDataText );
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateVertexDataWM_( VectorRenderVertex2D * const _outVertex, const VectorRenderVertex2D & _fromVertex ) const
    {
        _outVertex->assign( _fromVertex.begin(), _fromVertex.end() );

        VectorRenderVertex2D::const_iterator it = _fromVertex.begin();
        VectorRenderVertex2D::const_iterator it_end = _fromVertex.end();

        VectorRenderVertex2D::iterator it_w = _outVertex->begin();

        if( m_pixelsnap == true )
        {
            mt::mat4f wm = this->getWorldMatrix();
            wm.v3.x = MENGINE_FLOORF( wm.v3.x + 0.5f );
            wm.v3.y = MENGINE_FLOORF( wm.v3.y + 0.5f );

            for( ; it != it_end; ++it, ++it_w )
            {
                const RenderVertex2D & vertex = *it;

                RenderVertex2D & vertex_w = *it_w;

                mt::mul_v3_v3_m4( &vertex_w.position, vertex.position, wm );
            }
        }
        else
        {
            const mt::mat4f & wm = this->getWorldMatrix();

            for( ; it != it_end; ++it, ++it_w )
            {
                const RenderVertex2D & vertex = *it;

                RenderVertex2D & vertex_w = *it_w;

                mt::mul_v3_v3_m4( &vertex_w.position, vertex.position, wm );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateVertices_( const FontInterfacePtr & _font ) const
    {
        m_invalidateVertices = false;

        Color colorNode;
        this->calcTotalColor( &colorNode );

        this->updateVertexData_( _font, colorNode, &m_vertexDataText );

        if( m_anchorPercent.x != 0.f || m_anchorPercent.y != 0.f )
        {
            mt::vec2f anchor = m_textSize * m_autoScaleFactor * m_anchorPercent;

            for( RenderVertex2D & vertex : m_vertexDataText )
            {
                vertex.position.x -= anchor.x;
                vertex.position.y -= anchor.y;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr TextField::_updateMaterial() const
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}

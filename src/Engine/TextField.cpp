#include "TextField.h" 

#include "Interface/TextServiceInterface.h"
#include "Interface/FontServiceInterface.h"
#include "Interface/ApplicationInterface.h"

#include "TextLine.h"

#include "Kernel/StringFormat.h"
#include "Kernel/StringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionObservable.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/DocumentableHelper.h"
#include "Kernel/VectorHelper.h"

#include "Config/StdIntTypes.h"
#include "Config/StdIO.h"
#include "Config/StdAlgorithm.h"
#include "Config/StdIterator.h"
#include "Config/StdMath.h"

#include "math/box2.h"

#ifndef MENGINE_TEXT_FIELD_MAX_TEXT
#define MENGINE_TEXT_FIELD_MAX_TEXT 2048
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TextField::TextField()
        : m_horizontAlign( ETFHA_LEFT )
        , m_verticalAlign( ETFVA_BOTTOM )
        , m_extraThickness( 0.f )
        , m_charScale( 1.f )
        , m_maxLength( -1.f )
        , m_maxHeight( -1.f )
        , m_autoScaleFactor( 1.f )
        , m_lineOffset( 0.f )
        , m_charOffset( 0.f )
        , m_anchorPercent( 0.f, 0.f )
        , m_textParams( EFP_NONE )
        , m_maxCharCount( MENGINE_UNKNOWN_SIZE )
        , m_cacheCharCount( 0 )
        , m_cacheLayoutCount( 0 )
        , m_cacheTextSize( 0.f, 0.f )
        , m_cacheTextOffset( 0.f, 0.f )
        , m_anchorHorizontalAlign( true )
        , m_anchorVerticalAlign( true )
        , m_wrap( true )
        , m_autoScale( false )
        , m_justify( false )
        , m_pixelsnap( true )
        , m_debugMode( false )
        , m_invalidateVertices( true )
        , m_invalidateVerticesWM( true )
        , m_invalidateFont( true )
        , m_invalidateTextLines( true )
        , m_invalidateTextEntry( true )
        , m_invalidateTextId( true )
        , m_invalidateTextArguments( true )
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
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_TEXT_ALIAS_ID, &TextField::notifyChangeTextAliasId_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_TEXT_ALIAS_ARGUMENTS, &TextField::notifyChangeTextAliasArguments_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE, &TextField::notifyRenderDeviceLostPrepare_, MENGINE_DOCUMENT_FACTORABLE );

        this->invalidateTextId();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::_deactivate()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_LOCALE_PREPARE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_LOCALE_POST );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEBUG_TEXT_MODE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_TEXT_ALIAS_ID );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_TEXT_ALIAS_ARGUMENTS );
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

        this->invalidateTextId();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::notifyDebugMode_( bool _debugMode )
    {
        m_debugMode = _debugMode;

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::notifyChangeTextAliasId_( const ConstString & _environment, const ConstString & _textId )
    {
        if( m_aliasEnvironment != _environment || m_textId != _textId )
        {
            return;
        }

        this->invalidateTextId();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::notifyChangeTextAliasArguments_( const ConstString & _environment, const ConstString & _textId )
    {
        if( m_aliasEnvironment != _environment || m_textId != _textId )
        {
            return;
        }

        this->invalidateTextArguments();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::notifyRenderDeviceLostPrepare_()
    {
        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    const TextField::VectorTextLinesLayout & TextField::getTextLayots() const
    {
        if( m_invalidateTextLines == true )
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

        ColorValue_ARGB cacheFontARGB[16 + 1] = {'\0'};

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
                            TextLine::calcCharPosition( cd, offset2, charScaleTotal, i, &v.position );

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

        const VectorTextArguments & textArguments = this->getTotalTextArguments();

        for( const TextArgumentInterfacePtr & argument : textArguments )
        {
            context_invalidate |= argument->updateContext();
        }

        if( context_invalidate == true )
        {
            this->invalidateTextLines();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        const ConstString & textId = this->getTotalTextId();

        if( textId.empty() == true && m_text.empty() == true )
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

        if( m_invalidateTextLines == true )
        {
            this->updateTextLines_();
        }

        return m_cacheCharCount;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setMaxLength( float _maxLength )
    {
        MENGINE_ASSERTION_FATAL( _maxLength > 0.f, "text field '%s' max length '%f' must be >= 0.f"
            , this->getName().c_str()
            , _maxLength
        );

        if( m_maxLength == _maxLength )
        {
            return;
        }

        m_maxLength = _maxLength;

        m_textParams |= EFP_MAX_LENGTH;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::getMaxLength() const
    {
        return m_maxLength;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::hasMaxLength() const
    {
        return (m_textParams & EFP_MAX_LENGTH) == EFP_MAX_LENGTH;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::removeMaxLength()
    {
        if( (m_textParams & EFP_MAX_LENGTH) != EFP_MAX_LENGTH )
        {
            return;
        }

        m_maxLength = -1.f;

        m_textParams &= ~EFP_MAX_LENGTH;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setMaxHeight( float _maxHeight )
    {
        MENGINE_ASSERTION_FATAL( _maxHeight > 0.f, "text field '%s' max height '%f' must be >= 0.f"
            , this->getName().c_str()
            , _maxHeight
        );

        if( m_maxHeight == _maxHeight )
        {
            return;
        }

        m_maxHeight = _maxHeight;

        m_textParams |= EFP_MAX_HEIGHT;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::getMaxHeight() const
    {
        return m_maxHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::hasMaxHeight() const
    {
        return (m_textParams & EFP_MAX_HEIGHT) == EFP_MAX_HEIGHT;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::removeMaxHeight()
    {
        if( (m_textParams & EFP_MAX_HEIGHT) != EFP_MAX_HEIGHT )
        {
            return;
        }

        m_maxHeight = -1.f;

        m_textParams &= ~EFP_MAX_HEIGHT;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setAutoScale( bool _autoScale )
    {
        if( m_autoScale == _autoScale )
        {
            return;
        }

        m_autoScale = _autoScale;

        m_textParams |= EFP_AUTO_SCALE;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::getAutoScale() const
    {
        return m_autoScale;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::hasAutoScale() const
    {
        return (m_textParams & EFP_AUTO_SCALE) == EFP_AUTO_SCALE;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::removeAutoScale()
    {
        if( (m_textParams & EFP_AUTO_SCALE) != EFP_AUTO_SCALE )
        {
            return;
        }

        m_autoScale = false;

        m_textParams &= ~EFP_AUTO_SCALE;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setJustify( bool _justify )
    {
        if( m_justify == _justify )
        {
            return;
        }

        m_justify = _justify;

        m_textParams |= EFP_JUSTIFY;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::getJustify() const
    {
        return m_justify;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::hasJustify() const
    {
        return (m_textParams & EFP_JUSTIFY) == EFP_JUSTIFY;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::removeJustify()
    {
        if( (m_textParams & EFP_JUSTIFY) != EFP_JUSTIFY )
        {
            return;
        }

        m_justify = false;

        m_textParams &= ~EFP_JUSTIFY;

        this->invalidateTextLines();
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
        MENGINE_ASSERTION_FATAL( _font != nullptr, "text field '%s' set font is nullptr"
            , this->getName().c_str()
        );

        if( m_font == _font )
        {
            return;
        }

        m_font = _font;

        m_textParams |= EFP_FONT;

        this->invalidateFont();
        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    const FontInterfacePtr & TextField::getFont() const
    {
        return m_font;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::hasFont() const
    {
        return (m_textParams & EFP_FONT) == EFP_FONT;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::removeFont()
    {
        if( (m_textParams & EFP_FONT) != EFP_FONT )
        {
            return;
        }

        m_font = nullptr;

        m_textParams &= ~EFP_FONT;

        this->invalidateFont();
        this->invalidateTextLines();
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
        if( (m_textParams & EFP_COLOR_FONT) != EFP_COLOR_FONT )
        {
            return;
        }

        m_colorFont.clear();

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
        if( (m_textParams & EFP_LINE_OFFSET) != EFP_LINE_OFFSET )
        {
            return;
        }

        m_lineOffset = 0.f;

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
        if( (m_textParams & EFP_CHAR_OFFSET) != EFP_CHAR_OFFSET )
        {
            return;
        }

        m_charOffset = 0.f;

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
        if( (m_textParams & EFP_CHAR_SCALE) != EFP_CHAR_SCALE )
        {
            return;
        }

        m_charScale = 1.f;

        m_textParams &= ~EFP_CHAR_SCALE;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & TextField::getTextSize() const
    {
        if( m_invalidateTextLines == true )
        {
            this->updateTextLines_();
        }

        return m_cacheTextSize;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & TextField::getTextOffset() const
    {
        if( m_invalidateTextLines == true )
        {
            this->updateTextLines_();
        }

        return m_cacheTextOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::calcTotalTextViewport( Viewport * const _viewport ) const
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
        mt::box2_insideout( &box );

        for( const RenderVertex2D & vertex : textVertices )
        {
            mt::box2_add_internal_point( &box, vertex.position.x, vertex.position.y );
        }

        _viewport->begin = box.minimum;
        _viewport->end = box.maximum;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::calcTotalTextSize( mt::vec2f * const _textSize ) const
    {
        const mt::vec2f & textSize = this->getTextSize();

        *_textSize = textSize * m_autoScaleFactor;
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

                float space_length = 0.f;

                Glyph space_glyph;
                if( font->getGlyph( 0, ' ', 0, &space_glyph ) == true )
                {
                    space_length = (space_glyph.advance + charOffset) * charScale;
                }

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
                        StdIterator::advance( it_kerning, 1 );

                        GlyphCode glyphCharNext = (it_kerning != word.end()) ? *it_kerning : 0;

                        Glyph glyph;
                        if( font->getGlyph( 0, glyphChar, glyphCharNext, &glyph ) == false )
                        {
                            continue;
                        }

                        float char_length = (glyph.advance + charOffset) * charScale;

                        word_length += char_length;
                    }

                    if( word.empty() == false )
                    {
                        word_length -= charOffset * charScale;
                    }

                    if( maxLength >= 0.f && length + word_length > maxLength && new_textChunk.value.empty() == false )
                    {
                        length = 0.f;

                        new_textChunks.emplace_back( new_textChunk );
                        new_textChunk.value.clear();

                        new_textLines.emplace_back( new_textChunks );
                        new_textChunks.clear();
                    }

                    length += word_length;
                    length += space_length;

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

                if( words.empty() == false )
                {
                    length -= space_length;
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
                StdIterator::advance( it_chars_erase, 1 );

                chars.erase( it_chars_erase, chars.end() );

                VectorTextLineChunks2::iterator it_lines_erase = it_lines;
                StdIterator::advance( it_lines_erase, 1 );

                _textLines->erase( it_lines_erase, _textLines->end() );

                return;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::updateTextLinesDimension_( const FontInterfacePtr & _font, const VectorTextLineChunks2 & _textLines, float _justifyLength, uint32_t * const _charCount, uint32_t * const _layoutCount, mt::vec2f * const _textOffset, mt::vec2f * const _textSize ) const
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
                    TextLine tl( layoutIndex, charOffset, _justifyLength );
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
            *_layoutCount = 0;
            *_charCount = 0;
            *_textSize = mt::vec2f( 0.f, 0.f );
            *_textOffset = mt::vec2f( 0.f, 0.f );

            return true;
        }

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

        mt::vec2f textSize;
        textSize.x = text_length;

        float fontHeight = _font->getFontHeight();
        //float fontAscent = _font->getFontAscent();

        if( m_cacheLayoutCount == 0 )
        {
            textSize.y = 0.f;
        }
        else if( m_cacheLayoutCount == 1 )
        {
            textSize.y = fontHeight + m_extraThickness * 2.f;
        }
        else
        {
            textSize.y = (lineOffset + fontHeight) * (m_cacheLayoutCount - 1) + fontHeight + m_extraThickness * 2.f;
        }

        float lines_offset = this->calcLinesOffset( lineOffset, _font );

        mt::vec2f textOffset;
        textOffset.x = text_offset;
        textOffset.y = lines_offset * m_autoScaleFactor;

        *_layoutCount = (uint32_t)layouts.size();
        *_charCount = charCount;
        *_textSize = textSize;
        *_textOffset = textOffset;

        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    bool TextField::updateTextLines_() const
    {
        m_invalidateTextLines = false;

        m_layouts.clear();

        m_cacheCharCount = 0;
        m_cacheLayoutCount = 0;
        m_cacheTextOffset = mt::vec2f( 0.f, 0.f );
        m_cacheTextSize = mt::vec2f( 0.f, 0.f );

        const ConstString & totalTextId = this->getTotalTextId();

        if( totalTextId.empty() == true && m_text.empty() == true )
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
                , totalTextId.c_str()
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

        bool autoScale = this->calcAutoScale();

        MENGINE_ASSERTION( !(autoScale == true && (m_wrap == true || m_maxCharCount != MENGINE_UNKNOWN_SIZE)), "text '%s' invalid enable together attributes 'wrap' and 'autoScale'"
            , this->getName().c_str()
        );

        float maxLength = this->calcMaxLength();

        MENGINE_ASSERTION( !(autoScale == true && maxLength == -1.f), "text '%s' invalid enable 'autoScale' and not setup 'maxLength'"
            , this->getName().c_str()
        );

        float maxHeight = this->calcMaxHeight();

        MENGINE_ASSERTION( !(maxLength == -1.f && maxHeight != -1.f), "text '%s' invalid enable 'maxHeight' and not setup 'maxLength'"
            , this->getName().c_str()
        );

        float justifyLength = -1.f;

        bool justify = this->calcJustify();

        if( justify == true )
        {
            justifyLength = maxLength;
        }

        this->updateTextLinesWrap_( &m_cacheTextLines );

        uint32_t charCount;
        uint32_t layoutCount;
        mt::vec2f textOffset;
        mt::vec2f textSize;

        if( this->updateTextLinesDimension_( baseFont, m_cacheTextLines, justifyLength, &charCount, &layoutCount, &textOffset, &textSize ) == false )
        {
            return false;
        }

        m_cacheCharCount = charCount;
        m_cacheLayoutCount = layoutCount;
        m_cacheTextOffset = textOffset;
        m_cacheTextSize = textSize;

        m_autoScaleFactor = 1.f;

        if( autoScale == true && maxHeight == -1.f )
        {
            if( m_cacheTextSize.x > maxLength )
            {
                m_autoScaleFactor = maxLength / m_cacheTextSize.x;
            }
        }

        this->updateTextLinesMaxCount_( &m_cacheTextLines );

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
                    TextLine tl( layoutIndex, charOffset, justifyLength );
                    if( tl.initialize( fontId, chunkFont, textChunk.value ) == false )
                    {
                        LOGGER_ERROR( "text field '%s' textId '%s' text '%s' invalid setup line"
                            , this->getName().c_str()
                            , totalTextId.c_str()
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
    void TextField::updateTextId_() const
    {
        m_invalidateTextId = false;

        const ConstString & textId = TEXT_SERVICE()
            ->getTextAlias( m_aliasEnvironment, m_textId );

        m_totalTextId = textId;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateTextEntry_() const
    {
        m_invalidateTextEntry = false;

        const ConstString & totalTextId = this->getTotalTextId();

        if( totalTextId == ConstString::none() )
        {
            m_totalTextEntry = nullptr;

            return;
        }

        const TextEntryInterfacePtr & textEntry = TEXT_SERVICE()
            ->getTextEntry( totalTextId );

        MENGINE_ASSERTION_MEMORY_PANIC( textEntry, "text field '%s' can't find textId '%s' (doc: %s)"
            , this->getName().c_str()
            , totalTextId.c_str()
            , MENGINE_DOCUMENTABLE_STR( this, "TextField" )
        );

        m_totalTextEntry = textEntry;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::updateTextArguments_() const
    {
        m_invalidateTextArguments = false;

        if( TEXT_SERVICE()
            ->getTextAliasArguments( m_aliasEnvironment, m_textId, &m_totalTextArguments ) == false )
        {
            m_totalTextArguments = m_textArguments;
        }
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

        if( (m_textParams & EFP_MAX_LENGTH) == EFP_MAX_LENGTH )
        {
            return m_maxLength;
        }

        return -1.f;
    }
    //////////////////////////////////////////////////////////////////////////
    float TextField::calcMaxHeight() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTotalTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( (params & EFP_MAX_HEIGHT) == EFP_MAX_HEIGHT )
            {
                float value = textEntry->getMaxHeight();

                return value;
            }
        }

        if( (m_textParams & EFP_MAX_HEIGHT) == EFP_MAX_HEIGHT )
        {
            return m_maxHeight;
        }

        return -1.f;
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

        return ETFHA_LEFT;
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

        return ETFVA_BOTTOM;
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
    bool TextField::calcAutoScale() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTotalTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( (params & EFP_AUTO_SCALE) == EFP_AUTO_SCALE )
            {
                bool value = textEntry->getAutoScale();

                return value;
            }
        }

        if( (m_textParams & EFP_AUTO_SCALE) == EFP_AUTO_SCALE )
        {
            return m_autoScale;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::calcJustify() const
    {
        const TextEntryInterfacePtr & textEntry = this->getTotalTextEntry();

        if( textEntry != nullptr )
        {
            uint32_t params = textEntry->getFontParams();

            if( (params & EFP_JUSTIFY) == EFP_JUSTIFY )
            {
                bool value = textEntry->getJustify();

                return value;
            }
        }

        if( (m_textParams & EFP_JUSTIFY) == EFP_JUSTIFY )
        {
            return m_justify;
        }

        return false;
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

        if( m_cacheLayoutCount == 1 )
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
            case ETFVA_TOP:
                {
                    offset = 0.f;
                }break;
            }
        }
        else
        {
            float layoutCountf = (float)m_cacheLayoutCount;

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
                        offset += m_cacheTextSize.y * 0.5f;
                    }
                }break;
            case ETFVA_TOP:
                {
                    offset = -(fontHeight + _lineOffset) * layoutCountf;

                    if( m_anchorVerticalAlign == false )
                    {
                        offset += m_cacheTextSize.y;
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
        case ETFHA_LEFT:
            {
                offset = 0.f;
            }break;
        case ETFHA_CENTER:
            {
                offset = -length * 0.5f;

                if( m_anchorHorizontalAlign == false )
                {
                    offset += m_cacheTextSize.x * 0.5f;
                }
            }break;
        case ETFHA_RIGHT:
            {
                offset = -length;

                if( m_anchorHorizontalAlign == false )
                {
                    offset += m_cacheTextSize.x;
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
        m_textArguments.clear();
        m_totalTextArguments.clear();

        m_totalTextEntry = nullptr;

        m_font = nullptr;
        m_totalFont = nullptr;

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
        MENGINE_ASSERTION_FATAL( _textId.empty() == false, "text field '%s' set text id is empty"
            , this->getName().c_str()
        );

        if( m_textId == _textId )
        {
            return;
        }

        m_textId = _textId;

        m_text.clear();
        m_textArguments.clear();

        this->invalidateTextId();
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
        m_textArguments.clear();

        this->invalidateTextId();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setText( const String & _text )
    {
        if( m_text == _text )
        {
            return;
        }

        m_text = _text;
        m_textArguments.clear();

        m_textId.clear();

        this->invalidateTextId();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setTextFormat( const Char * _format, ... )
    {
        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        Char str[MENGINE_TEXT_FIELD_MAX_TEXT + 1] = {'\0'};
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
    const String & TextField::getText() const
    {
        return m_text;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::removeText()
    {
        if( m_text.empty() == true )
        {
            return;
        }

        m_text.clear();
        m_textArguments.clear();

        this->invalidateTextEntry();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::calcText( String * const _text ) const
    {
        const TextEntryInterfacePtr & textEntry = this->getTotalTextEntry();

        const Char * textValue;
        size_t textValueLen;

        if( textEntry == nullptr )
        {
            textValueLen = m_text.size();
            textValue = m_text.c_str();
        }
        else
        {
            const String & value = textEntry->getValue();

            textValue = value.c_str();
            textValueLen = value.size();
        }

        const VectorTextArguments & textArguments = this->getTotalTextArguments();

        String fmt;
        if( Helper::fillStringFormat( textValue, textValueLen, textArguments, &fmt ) == false )
        {
            LOGGER_ERROR( "text field '%s' textId '%s' (base '%s') invalid formating string text '%s' format with args %" MENGINE_PRIuPTR " [alias env '%s']"
                , this->getName().c_str()
                , this->getTotalTextId().c_str()
                , m_textId.c_str()
                , textValue
                , textArguments.size()
                , m_aliasEnvironment.c_str()
            );

            LOGGER_ERROR( "text field '%s' args '%s'"
                , this->getName().c_str()
                , Helper::vectorTextArgumentsToString( textArguments ).c_str()
            );

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

                const String & text = textEntry->getValue();

                *_out = text;

                return true;
            }, _text, &fmtTagMatched ) == false )
            {
                LOGGER_ERROR( "text field '%s' textId '%s' (base '%s') invalid tagging string format '%s' with tag '%s' [alias env '%s']"
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

        this->invalidateTextId();
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TextField::getTextAliasEnvironment() const
    {
        return m_aliasEnvironment;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setTextArguments( const VectorTextArguments & _arguments )
    {
        if( m_textArguments == _arguments )
        {
            return;
        }

        m_textArguments = _arguments;

        this->invalidateTextArguments();
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorTextArguments & TextField::getTextArguments() const
    {
        return m_textArguments;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::removeTextArguments()
    {
        if( m_textArguments.empty() == true )
        {
            return;
        }

        m_textArguments.clear();

        this->invalidateTextArguments();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TextField::getTextExpectedArgument() const
    {
        const Char * textValue;
        size_t textValueLen;

        const TextEntryInterfacePtr & textEntry = this->getTotalTextEntry();

        if( textEntry == nullptr )
        {
            textValue = m_text.c_str();
            textValueLen = m_text.size();
        }
        else
        {
            const String & value = textEntry->getValue();

            textValue = value.c_str();
            textValueLen = value.size();
        }

        try
        {
            uint32_t expected_args = Helper::getStringFormatExpectedArgs( textValue, textValueLen );

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
                    LOGGER_ERROR( "text field '%s' textId '%s' (base '%s') invalid prepare text '%s' [alias env '%s']"
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
                    LOGGER_ERROR( "text field '%s' textId '%s' (base '%s') invalid prepare text '%s' [alias env '%s']"
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
            LOGGER_ERROR( "text field '%s' textId '%s' (base '%s') invalid prepare text '%s' [alias env '%s']"
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

        m_textParams |= EFP_HORIZONTAL_ALIGN;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    ETextHorizontAlign TextField::getHorizontAlign() const
    {
        return m_horizontAlign;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::hasHorizontAlign() const
    {
        return (m_textParams & EFP_HORIZONTAL_ALIGN) == EFP_HORIZONTAL_ALIGN;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::removeHorizontAlign()
    {
        if( (m_textParams & EFP_HORIZONTAL_ALIGN) != EFP_HORIZONTAL_ALIGN )
        {
            return;
        }

        m_horizontAlign = ETFHA_LEFT;

        m_textParams &= ~EFP_HORIZONTAL_ALIGN;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::setVerticalAlign( ETextVerticalAlign _verticalAlign )
    {
        if( m_verticalAlign == _verticalAlign )
        {
            return;
        }

        m_verticalAlign = _verticalAlign;

        m_textParams |= EFP_VERTICAL_ALIGN;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    ETextVerticalAlign TextField::getVerticalAlign() const
    {
        return m_verticalAlign;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextField::hasVerticalAlign() const
    {
        return (m_textParams & EFP_VERTICAL_ALIGN) == EFP_VERTICAL_ALIGN;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::removeVerticalAlign()
    {
        if( (m_textParams & EFP_VERTICAL_ALIGN) != EFP_VERTICAL_ALIGN )
        {
            return;
        }

        m_verticalAlign = ETFVA_BOTTOM;

        m_textParams &= ~EFP_VERTICAL_ALIGN;

        this->invalidateTextLines();
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
    void TextField::invalidateFont() const
    {
        m_invalidateFont = true;

        this->invalidateTextLines();
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
    void TextField::invalidateTextArguments() const
    {
        m_invalidateTextArguments = true;

        this->invalidateFont();
        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
    void TextField::invalidateTextId() const
    {
        m_invalidateTextId = true;

        this->invalidateTextArguments();
        this->invalidateTextEntry();
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
            wm.v3.x = StdMath::floorf( wm.v3.x + 0.5f );
            wm.v3.y = StdMath::floorf( wm.v3.y + 0.5f );

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
            mt::vec2f anchor = m_cacheTextSize * m_autoScaleFactor * m_anchorPercent;

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

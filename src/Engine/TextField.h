#pragma once

#include "Interface/TextEntryInterface.h"

#include "TextRenderChunk.h"
#include "TextChar.h"
#include "TextLine.h"

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/Observable.h"
#include "Kernel/Materialable.h"

#include "Kernel/Color.h"
#include "Kernel/ValueInterpolator.h"
#include "Kernel/ConstString.h"
#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/Vector.h"

#include "math/vec4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TextField
        : public Node
        , public Materialable
        , public Observable
        , protected BaseRender
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( TextField );
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        TextField();
        ~TextField() override;

    public:
        void setMaxLength( float _maxLength );
        float getMaxLength() const;

    public:
        void setAutoScale( bool _autoScale );
        bool getAutoScale() const;

    public:
        void setAnchorHorizontalAlign( bool _anchorHorizontalAlign );
        bool getAnchorHorizontalAlign() const;

    public:
        void setAnchorVerticalAlign( bool _anchorVerticalAlign );
        bool getAnchorVerticalAlign() const;

    public:
        void setAnchorPercent( const mt::vec2f & _anchorPercent );
        const mt::vec2f & getAnchorPercent() const;

    public:
        void setWrap( bool _wrap );
        bool getWrap() const;

    public:
        void setTextId( const ConstString & _textId );
        const ConstString & getTextId() const;
        void removeTextId();

        const ConstString & getTotalTextId() const;

    public:
        void setText( const String & _text );
        void setTextFormat( MENGINE_CHECK_FORMAT_STRING( const Char * _format ), ... ) MENGINE_ATTRIBUTE_FORMAT_STRING( 2, 3 );
        void removeText();

    public:
        bool calcText( String * const _text ) const;

    public:
        void setTextAliasEnvironment( const ConstString & _aliasEnvironment );
        const ConstString & getTextAliasEnvironment() const;

        void setTextFormatArgs( const VectorString & _args );
        void removeTextFormatArgs();
        const VectorString & getTextFormatArgs() const;

        typedef Lambda<bool( String * )> LambdaFormatArgsContext;
        void setTextFormatArgsContext( uint32_t _index, const LambdaFormatArgsContext & _context );
        void removeTextFormatArgsContexts();

    public:
        uint32_t getTextExpectedArgument() const;

    protected:
        bool updateTextCache_( U32String * const _cacheText ) const;

    public:
        float getFontHeight() const;

    public:
        void setFont( const TextFontInterfacePtr & _font );
        const TextFontInterfacePtr & getFont() const;

    public:
        void setFontColor( const Color & _color );
        const Color & getFontColor() const;
        bool hasFontColor() const;
        void removeFontColor();

        void setLineOffset( float _offset );
        float getLineOffset() const;
        bool hasLineOffset() const;
        void removeLineOffset();

        void setCharOffset( float _offset );
        float getCharOffset() const;
        bool hasCharOffset() const;
        void removeCharOffset();

        void setCharScale( float _value );
        float getCharScale() const;
        bool hasCharScale() const;
        void removeCharScale();

    public:
        void setHorizontAlign( ETextHorizontAlign _horizontAlign );
        ETextHorizontAlign getHorizontAlign() const;

        void setVerticalAlign( ETextVerticalAlign _verticalAlign );
        ETextVerticalAlign getVerticalAlign() const;

    public:
        void setHorizontalCenterAlign();
        bool isHorizontalCenterAlign() const;

        void setHorizontalRightAlign();
        bool isHorizontalRightAlign() const;

        void setHorizontalLeftAlign();
        bool isHorizontalLeftAlign() const;

        void setVerticalTopAlign();
        bool isVerticalTopAlign() const;

        void setVerticalCenterAlign();
        bool isVerticalCenterAlign() const;

        void setVerticalBottomAlign();
        bool isVerticalBottomAlign() const;

    public:
        void setMaxCharCount( uint32_t _maxCharCount );
        uint32_t getMaxCharCount() const;

        void setPixelsnap( bool _pixelsnap );
        bool getPixelsnap() const;

    public:
        const mt::vec2f & getTextSize() const;
        bool calcTextViewport( Viewport * const _viewport ) const;
        void calcTotalTextSize( mt::vec2f * const _textSize ) const;

    public:
        uint32_t getCharCount() const;

    public:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        bool _activate() override;
        void _deactivate() override;
        bool _compile() override;
        void _release() override;

    protected:
        void _invalidateWorldMatrix() const override;
        void _updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const override;
        void _invalidateColor() const override;

    protected:
        void _dispose() override;

    protected:
        void notifyChangeLocalePrepare_( const ConstString & _prevLocale, const ConstString & _currentlocale );
        void notifyChangeLocalePost_( const ConstString & _prevLocale, const ConstString & _currentlocale );
        void notifyDebugMode_( bool _debugMode );
        void notifyChangeTextAliasArguments_( const ConstString & _environment, const ConstString & _alias );
        void notifyRenderDeviceLostPrepare_();

    protected:
        void updateVertices_( const TextFontInterfacePtr & _font ) const;
        void invalidateVertices_() const;

        void updateVerticesWM_( const TextFontInterfacePtr & _font ) const;
        void updateVertexDataWM_( VectorRenderVertex2D * const _outVertex, const VectorRenderVertex2D & _fromVertex ) const;

        void invalidateVerticesWM_() const;

        MENGINE_INLINE const VectorRenderVertex2D & getTextVertices( const TextFontInterfacePtr & _font ) const;
        MENGINE_INLINE const VectorRenderVertex2D & getTextVerticesWM( const TextFontInterfacePtr & _font ) const;

        void updateVertexData_( const TextFontInterfacePtr & _font, const Color & _color, VectorRenderVertex2D * const _vertexData ) const;

    protected:
        void invalidateTextLines() const;
        void invalidateTextEntry() const;
        MENGINE_INLINE bool isInvalidateTextLines() const;

        bool updateTextLines_() const;
        void updateTextLinesWrap_( VectorTextLineChunks2 * const _textLines ) const;
        void updateTextLinesMaxCount_( VectorTextLineChunks2 * const _textLines ) const;
        bool updateTextLinesDimension_( const TextFontInterfacePtr & _font, const VectorTextLineChunks2 & _textLines, mt::vec2f * const _textSize, uint32_t * const _charCount, uint32_t * const _layoutCount ) const;

    public:
        MENGINE_INLINE const TextEntryInterfacePtr & getTotalTextEntry() const;
        MENGINE_INLINE const TextFontInterfacePtr & getTotalFont() const;

    protected:
        MENGINE_INLINE void invalidateFont() const;
        bool updateFont_() const;
        void updateTextEntry_() const;

    public:
        uint32_t getTextFieldParams() const;

    public:
        const TextFontInterfacePtr & calcFont() const;
        float calcLineOffset() const;
        float calcCharOffset() const;
        float calcMaxLength() const;

        const Color & calcFontColor() const;

        ETextHorizontAlign calcHorizontAlign() const;
        ETextVerticalAlign calcVerticalAlign() const;

        float calcCharScale() const;
        float calcLinesOffset( float _lineOffset, const TextFontInterfacePtr & _font ) const;

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    protected:
        void updateContext_() const;

    protected:
        ConstString m_textId;
        ConstString m_aliasEnvironment;

        String m_text;

        mutable TextEntryInterfacePtr m_totalTextEntry;
        mutable VectorString m_textFormatArgs;

        typedef Vector<LambdaFormatArgsContext> VectorFormatArgsContext;
        VectorFormatArgsContext m_textFormatArgContexts;

        ETextHorizontAlign m_horizontAlign;
        ETextVerticalAlign m_verticalAlign;

        mutable TextFontInterfacePtr m_totalFont;
        mutable bool m_invalidateFont;

        float m_charScale;

        float m_maxLength;
        mutable float m_autoScaleFactor;

        TextFontInterfacePtr m_font;

        float m_lineOffset;
        float m_charOffset;

        mt::vec2f m_anchorPercent;

        Color m_colorFont;

        uint32_t m_textParams;

        uint32_t m_maxCharCount;

        mutable U32String m_cacheText;
        mutable VectorTextLineChunks m_cacheTextChars;
        mutable VectorTextLineChunks2 m_cacheTextLines;

        mutable uint32_t m_charCount;
        mutable uint32_t m_layoutCount;
        mutable mt::vec2f m_textSize;

        bool m_anchorHorizontalAlign;
        bool m_anchorVerticalAlign;
        bool m_wrap;
        bool m_autoScale;
        bool m_pixelsnap;

        bool m_debugMode;

        typedef Vector<TextLine> VectorTextLines;
        typedef Vector<VectorTextLines> VectorTextLines2;
        typedef Vector<VectorTextLines2> VectorTextLinesLayout;
        mutable VectorTextLinesLayout m_layouts;

        typedef Vector<TextRenderChunk> VectorChunks;
        mutable VectorChunks m_chunks;

        mutable VectorCacheFonts m_cacheFonts;

        mutable VectorRenderVertex2D m_vertexDataText;
        mutable VectorRenderVertex2D m_vertexDataTextWM;

        mutable bool m_invalidateVertices;
        mutable bool m_invalidateVerticesWM;

        mutable bool m_invalidateTextLines;
        mutable bool m_invalidateTextEntry;

    protected:
        const VectorTextLinesLayout & getTextLayots() const;
        float getHorizontAlignOffset_( const VectorTextLines2 & _lines ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<TextField> TextFieldPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const VectorRenderVertex2D & TextField::getTextVertices( const TextFontInterfacePtr & _font ) const
    {
        if( m_invalidateVertices == true )
        {
            this->updateVertices_( _font );
        }

        return m_vertexDataText;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const VectorRenderVertex2D & TextField::getTextVerticesWM( const TextFontInterfacePtr & _font ) const
    {
        if( m_invalidateVerticesWM == true )
        {
            this->updateVerticesWM_( _font );
        }

        return m_vertexDataTextWM;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool TextField::isInvalidateTextLines() const
    {
        return m_invalidateTextLines;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const TextFontInterfacePtr & TextField::getTotalFont() const
    {
        if( m_invalidateFont == true )
        {
            if( this->updateFont_() == false )
            {
                return TextFontInterfacePtr::none();
            }
        }

        return m_totalFont;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const TextEntryInterfacePtr & TextField::getTotalTextEntry() const
    {
        if( m_invalidateTextEntry == true )
        {
            this->updateTextEntry_();
        }

        return m_totalTextEntry;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void TextField::invalidateFont() const
    {
        m_invalidateFont = true;

        this->invalidateTextLines();
    }
    //////////////////////////////////////////////////////////////////////////
}

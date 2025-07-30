#pragma once

#include "Interface/TextEntryInterface.h"

#include "Engine/TextRenderChunk.h"
#include "Engine/TextChar.h"
#include "Engine/TextLine.h"

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
#include "Kernel/VectorTextArguments.h"

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

    public:
        void setText( const String & _text );
        void setTextFormat( const Char * _format, ... ) MENGINE_ATTRIBUTE_FORMAT_STRING( 2, 3 );
        const String & getText() const;
        void removeText();

    public:
        bool calcText( String * const _text ) const;

    public:
        void setTextAliasEnvironment( const ConstString & _aliasEnvironment );
        const ConstString & getTextAliasEnvironment() const;

        void setTextArguments( const VectorTextArguments & _arguments );
        const VectorTextArguments & getTextArguments() const;
        void removeTextArguments();

    public:
        uint32_t getTextExpectedArgument() const;

    protected:
        bool updateTextCache_( U32String * const _cacheText ) const;

    public:
        float getFontHeight() const;

    public:
        void setFont( const FontInterfacePtr & _font );
        const FontInterfacePtr & getFont() const;
        bool hasFont() const;
        void removeFont();

        void setMaxLength( float _maxLength );
        float getMaxLength() const;
        bool hasMaxLength() const;
        void removeMaxLength();

        void setMaxHeight( float _maxHeight );
        float getMaxHeight() const;
        bool hasMaxHeight() const;
        void removeMaxHeight();

        void setAutoScale( bool _autoScale );
        bool getAutoScale() const;
        bool hasAutoScale() const;
        void removeAutoScale();

        void setJustify( bool _justify );
        bool getJustify() const;
        bool hasJustify() const;
        void removeJustify();

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

        void setHorizontAlign( ETextHorizontAlign _horizontAlign );
        ETextHorizontAlign getHorizontAlign() const;
        bool hasHorizontAlign() const;
        void removeHorizontAlign();

        void setVerticalAlign( ETextVerticalAlign _verticalAlign );
        ETextVerticalAlign getVerticalAlign() const;
        bool hasVerticalAlign() const;
        void removeVerticalAlign();

    public:
        void setExtraThickness( float _extraThickness );
        float getExtraThickness() const;

    public:
        void setMaxCharCount( uint32_t _maxCharCount );
        uint32_t getMaxCharCount() const;

        void setPixelsnap( bool _pixelsnap );
        bool getPixelsnap() const;

    public:
        const mt::vec2f & getTextSize() const;
        const mt::vec2f & getTextOffset() const;
        bool calcTotalTextViewport( Viewport * const _viewport ) const;
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
        void notifyChangeTextAliasId_( const ConstString & _environment, const ConstString & _textId );
        void notifyChangeTextAliasArguments_( const ConstString & _environment, const ConstString & _textId );
        void notifyRenderDeviceLostPrepare_();

    protected:
        void updateVertices_( const FontInterfacePtr & _font ) const;
        void invalidateVertices_() const;

        void updateVerticesWM_( const FontInterfacePtr & _font ) const;
        void updateVertexDataWM_( VectorRenderVertex2D * const _outVertex, const VectorRenderVertex2D & _fromVertex ) const;

        void invalidateVerticesWM_() const;

        MENGINE_INLINE const VectorRenderVertex2D & getTextVertices( const FontInterfacePtr & _font ) const;
        MENGINE_INLINE const VectorRenderVertex2D & getTextVerticesWM( const FontInterfacePtr & _font ) const;

        void updateVertexData_( const FontInterfacePtr & _font, const Color & _color, VectorRenderVertex2D * const _vertexData ) const;

    protected:
        void invalidateFont() const;
        void invalidateTextLines() const;
        void invalidateTextEntry() const;
        void invalidateTextArguments() const;
        void invalidateTextId() const;


        bool updateTextLines_() const;
        void updateTextLinesWrap_( VectorTextLineChunks2 * const _textLines ) const;
        void updateTextLinesMaxCount_( VectorTextLineChunks2 * const _textLines ) const;
        bool updateTextLinesDimension_( const FontInterfacePtr & _font, const VectorTextLineChunks2 & _textLines, float _justifyLength, uint32_t * const _charCount, uint32_t * const _layoutCount, mt::vec2f * const _textOffset, mt::vec2f * const _textSize ) const;

    public:
        MENGINE_INLINE const ConstString & getTotalTextId() const;
        MENGINE_INLINE const TextEntryInterfacePtr & getTotalTextEntry() const;
        MENGINE_INLINE const VectorTextArguments & getTotalTextArguments() const;
        MENGINE_INLINE const FontInterfacePtr & getTotalFont() const;

    protected:
        bool updateFont_() const;
        void updateTextId_() const;
        void updateTextEntry_() const;
        void updateTextArguments_() const;

    public:
        uint32_t getTextFieldParams() const;

    public:
        const FontInterfacePtr & calcFont() const;
        float calcLineOffset() const;
        float calcCharOffset() const;
        float calcMaxLength() const;
        float calcMaxHeight() const;
        const Color & calcFontColor() const;
        ETextHorizontAlign calcHorizontAlign() const;
        ETextVerticalAlign calcVerticalAlign() const;
        float calcCharScale() const;
        bool calcAutoScale() const;
        bool calcJustify() const;

        float calcLinesOffset( float _lineOffset, const FontInterfacePtr & _font ) const;

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    protected:
        void updateContext_() const;

    protected:
        String m_text;

        ConstString m_textId;
        mutable ConstString m_totalTextId;

        ConstString m_aliasEnvironment;

        mutable TextEntryInterfacePtr m_totalTextEntry;

        VectorTextArguments m_textArguments;
        mutable VectorTextArguments m_totalTextArguments;

        ETextHorizontAlign m_horizontAlign;
        ETextVerticalAlign m_verticalAlign;

        float m_extraThickness;

        float m_charScale;

        float m_maxLength;
        float m_maxHeight;
        mutable float m_autoScaleFactor;

        FontInterfacePtr m_font;
        mutable FontInterfacePtr m_totalFont;

        float m_lineOffset;
        float m_charOffset;

        mt::vec2f m_anchorPercent;

        Color m_colorFont;

        uint32_t m_textParams;

        uint32_t m_maxCharCount;

        mutable U32String m_cacheText;
        mutable VectorTextLineChunks m_cacheTextChars;
        mutable VectorTextLineChunks2 m_cacheTextLines;

        mutable uint32_t m_cacheCharCount;
        mutable uint32_t m_cacheLayoutCount;
        mutable mt::vec2f m_cacheTextOffset;
        mutable mt::vec2f m_cacheTextSize;

        bool m_anchorHorizontalAlign;
        bool m_anchorVerticalAlign;
        bool m_wrap;
        bool m_autoScale;
        bool m_justify;
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

        mutable bool m_invalidateFont;

        mutable bool m_invalidateTextLines;
        mutable bool m_invalidateTextEntry;
        mutable bool m_invalidateTextId;
        mutable bool m_invalidateTextArguments;

    protected:
        const VectorTextLinesLayout & getTextLayots() const;
        float getHorizontAlignOffset_( const VectorTextLines2 & _lines ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<TextField> TextFieldPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const VectorRenderVertex2D & TextField::getTextVertices( const FontInterfacePtr & _font ) const
    {
        if( m_invalidateVertices == true )
        {
            this->updateVertices_( _font );
        }

        return m_vertexDataText;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const VectorRenderVertex2D & TextField::getTextVerticesWM( const FontInterfacePtr & _font ) const
    {
        if( m_invalidateVerticesWM == true )
        {
            this->updateVerticesWM_( _font );
        }

        return m_vertexDataTextWM;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const FontInterfacePtr & TextField::getTotalFont() const
    {
        if( m_invalidateFont == true )
        {
            if( this->updateFont_() == false )
            {
                return FontInterfacePtr::none();
            }
        }

        return m_totalFont;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const ConstString & TextField::getTotalTextId() const
    {
        if( m_invalidateTextId == true )
        {
            this->updateTextId_();
        }

        return m_totalTextId;
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
    MENGINE_INLINE const VectorTextArguments & TextField::getTotalTextArguments() const
    {
        if( m_invalidateTextArguments == true )
        {
            this->updateTextArguments_();
        }

        return m_totalTextArguments;
    }
    //////////////////////////////////////////////////////////////////////////
}

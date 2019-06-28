#pragma once

#include "Interface/TextEntryInterface.h"

#include "TextRenderChunk.h"
#include "TextChar.h"
#include "TextLine.h"

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/Observable.h"
#include "Kernel/Materialable.h"

#include "Kernel/Color.h"
#include "Kernel/ValueInterpolator.h"
#include "Kernel/ConstString.h"
#include "Kernel/VectorRenderVertex2D.h"

#include "Config/Vector.h"

#include "math/vec4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TextField
        : public Node
        , public BaseRender
        , public Materialable
        , public Observable
    {
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();

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
        void setWrap( bool _wrap );
        bool getWrap() const;

    public:
        void setTextID( const ConstString& _key );
        void removeTextID();

        const ConstString & getTextID() const;

        void setTextAliasEnvironment( const ConstString & _aliasEnvironment );
        const ConstString & getTextAliasEnvironment() const;

        void setTextFormatArgs( const VectorString & _args );
        void removeTextFormatArgs();
        const VectorString & getTextFormatArgs() const;

    public:
        uint32_t getTextExpectedArgument() const;

    protected:
        bool updateTextCache_( U32String & _cacheText ) const;

    public:
        void setFontName( const ConstString & _fontName );
        const ConstString & getFontName() const;

    public:
        float getFontHeight() const;

    public:
        void setFontColor( const Color & _color );
        const Color & getFontColor() const;

        void setLineOffset( float _offset );
        float getLineOffset() const;

        void setCharOffset( float _offset );
        float getCharOffset() const;

        void setCharScale( float _value );
        float getCharScale() const;

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
        void calcTextViewport( Viewport & _viewport ) const;

    public:
        uint32_t getCharCount() const;

    public:
        void render( const RenderContext * _state ) const override;

    protected:
        bool _activate() override;
        void _deactivate() override;
        bool _compile() override;
        void _release() override;

        void _invalidateWorldMatrix() override;
        void _updateBoundingBox( mt::box2f & _boundingBox, mt::box2f ** _boundingBoxCurrent ) const override;
        void _invalidateColor() override;

    protected:
        void notifyChangeLocale( const ConstString & _prevLocale, const ConstString & _currentlocale );
        void notifyDebugMode( bool _debugMode );
        void notifyChangeTextAliasArguments( const ConstString & _environment, const ConstString & _alias );

    protected:
        void updateVertices_( const TextFontInterfacePtr & _font ) const;
        void invalidateVertices_() const;

        void updateVerticesWM_( const TextFontInterfacePtr & _font ) const;
        void updateVertexDataWM_( VectorRenderVertex2D & _outVertex, const VectorRenderVertex2D & _fromVertex ) const;

        void invalidateVerticesWM_() const;

        inline const VectorRenderVertex2D & getTextVertices( const TextFontInterfacePtr & _font ) const;

        void updateVertexData_( const TextFontInterfacePtr & _font, const Color & _color, VectorRenderVertex2D& _vertexData ) const;

    protected:
        void invalidateTextLines() const;
        void invalidateTextEntry() const;
        inline bool isInvalidateTextLines() const;

        bool updateTextLines_() const;
        void updateTextLinesWrap_( VectorTextLineChunks2 & _textLines ) const;
        void updateTextLinesMaxCount_( VectorTextLineChunks2 & _textLines ) const;
        bool updateTextLinesDimension_( const TextFontInterfacePtr & _font, const VectorTextLineChunks2 & _textLines, mt::vec2f * _size, uint32_t * _charCount, uint32_t * _layoutCount ) const;

    public:
        inline const TextEntryInterfacePtr & getTextEntry() const;
        inline const TextFontInterfacePtr & getFont() const;

    protected:
        inline void invalidateFont() const;
        bool updateFont_() const;
        void updateTextEntry_() const;

    public:
        const ConstString & calcFontName() const;
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
        ConstString m_textId;
        ConstString m_aliasEnvironment;

        mutable TextEntryInterfacePtr m_textEntry;
        mutable VectorString m_textFormatArgs;

        ETextHorizontAlign m_horizontAlign;
        ETextVerticalAlign m_verticalAlign;

        mutable TextFontInterfacePtr m_font;
        mutable bool m_invalidateFont;

        float m_charScale;

        float m_maxLength;
        mutable float m_autoScaleFactor;

        ConstString m_fontName;

        float m_lineOffset;
        float m_charOffset;

        Color m_colorFont;

        uint32_t m_fontParams;

        uint32_t m_maxCharCount;

        mutable uint32_t m_charCount;
        mutable uint32_t m_layoutCount;
        mutable mt::vec2f m_textSize;

        bool m_wrap;
        bool m_autoScale;
        bool m_pixelsnap;

        bool m_debugMode;

        typedef Vector<TextLine> VectorTextLines;
        typedef Vector<VectorTextLines> VectorTextLines2;
        typedef Vector<VectorTextLines2> VectorTextLinesLayout;
        mutable VectorTextLinesLayout m_layouts;

        typedef Vector<float> VectorTextLineAlignOffset;
        mutable VectorTextLineAlignOffset m_textLineAlignOffsets;

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
    typedef IntrusivePtr<TextField> TextFieldPtr;
    //////////////////////////////////////////////////////////////////////////
    inline const VectorRenderVertex2D & TextField::getTextVertices( const TextFontInterfacePtr & _font ) const
    {
        if( m_invalidateVerticesWM == true )
        {
            this->updateVerticesWM_( _font );
        }

        return m_vertexDataTextWM;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool TextField::isInvalidateTextLines() const
    {
        return m_invalidateTextLines;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const TextFontInterfacePtr & TextField::getFont() const
    {
        if( m_invalidateFont == true )
        {
            if( this->updateFont_() == false )
            {
                return TextFontInterfacePtr::none();
            }
        }

        return m_font;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const TextEntryInterfacePtr & TextField::getTextEntry() const
    {
        if( m_invalidateTextEntry == true )
        {
            this->updateTextEntry_();
        }

        return m_textEntry;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void TextField::invalidateFont() const
    {
        m_invalidateFont = true;

        this->invalidateTextLines();
    }
}

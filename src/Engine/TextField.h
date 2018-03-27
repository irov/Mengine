# pragma once

#include "Interface/NotificationServiceInterface.h"
#include "Interface/TextInterface.h"

#include "Kernel/Node.h"
#include "Kernel/Materialable.h"

#include "Core/ColourValue.h"
#include "Core/ValueInterpolator.h"
#include "Core/ConstString.h"
#include "Core/RenderVertex2D.h"

#include "math/vec4.h"

namespace Mengine
{
    class TextLine;
    typedef stdex::vector<TextLine> TVectorTextLine;
    typedef stdex::vector<TVectorTextLine> TVectorTextLineLayout;

    class TextField
        : public Node
        , public Materialable
    {
    public:
        TextField();
        ~TextField() override;

    public:
        void setMaxLength( float _len );
        float getMaxLength() const;

    public:
        void setWrap( bool _wrap );
        bool getWrap() const;

    public:
        void setTextID( const ConstString& _key );
        void removeTextID();

        const ConstString & getTextID() const;

        void setTextFormatArgs( const TVectorString & _args );
        void removeTextFormatArgs();
        const TVectorString & getTextFormatArgs() const;

    public:
        size_t getTextExpectedArgument() const;

    protected:
        bool updateTextCache_() const;

    public:
        void setFontName( const ConstString & _name );
        const ConstString & getFontName() const;

    public:
        float getFontHeight() const;

    public:
        void setFontColor( const ColourValue & _color );
        const ColourValue& getFontColor() const;

        void enableOutline( bool _value );
        bool isOutline() const;

        void setOutlineColor( const ColourValue & _color );
        const ColourValue& getOutlineColor() const;

        void setLineOffset( float _offset );
        float getLineOffset() const;

        void setCharOffset( float _offset );
        float getCharOffset() const;

        void setCharScale( float _value );
        float getCharScale() const;

    public:
        void setHorizontalCenterAlign();
        bool isHorizontalCenterAlign() const;

        void setHorizontalRightAlign();
        bool isHorizontalRightAlign() const;

        void setHorizontalLeftAlign();
        bool isHorizontalLeftAlign() const;

        void setVerticalBottomAlign();
        bool isVerticalBottomAlign() const;

        void setVerticalCenterAlign();
        bool isVerticalCenterAlign() const;

    public:
        void setMaxCharCount( uint32_t _maxCharCount );
        uint32_t getMaxCharCount() const;

        void setPixelsnap( bool _pixelsnap );
        bool getPixelsnap() const;

    public:
        const mt::vec2f & getTextSize() const;

    public:
        uint32_t getCharCount() const;

    protected:
        void _render( RenderServiceInterface * _renderService, const RenderState * _state ) override;

    protected:
        void renderOutline_( const RenderState * _state );

    protected:
        bool _activate() override;
        void _deactivate() override;
        bool _compile() override;
        void _release() override;

        void _invalidateWorldMatrix() override;
        void _updateBoundingBox( mt::box2f & _boundingBox ) const override;
        void _invalidateColor() override;

    protected:
        void notifyChangeLocale( const ConstString & _prevLocale, const ConstString & _currentlocale );
        void notifyDebugMode( bool _mode );

    protected:
        ObserverInterfacePtr m_observerChangeLocale;
        ObserverInterfacePtr m_observerDebugMode;

    protected:
        float getHorizontAlignOffset_( const TextLine & _line );

    protected:
        void updateVertices_( const TextFontInterfacePtr & _font );
        void invalidateVertices_() const;

        void updateVerticesWM_( const TextFontInterfacePtr & _font );
        void updateVertexDataWM_( TVectorRenderVertex2D & _outVertex, const TVectorRenderVertex2D & _fromVertex );

        void invalidateVerticesWM_() const;

        inline const TVectorRenderVertex2D & getOutlineVertices( const TextFontInterfacePtr & _font );
        inline const TVectorRenderVertex2D & getTextVertices( const TextFontInterfacePtr & _font );

        void updateVertexData_( const TextFontInterfacePtr & _font, const ColourValue & _color, TVectorRenderVertex2D& _vertexData );

    protected:
        const TVectorTextLineLayout & getTextLayots() const;

    protected:
        void invalidateTextLines() const;
        void invalidateTextEntry() const;
        inline bool isInvalidateTextLines() const;

        void updateTextLines_() const;

    protected:
        inline const TextEntryInterface * getTextEntry() const;
        inline const TextFontInterfacePtr & getFont() const;
        inline void invalidateFont() const;
        void updateFont_() const;
        void updateTextEntry_() const;

    protected:
        mutable TextFontInterfacePtr m_font;
        mutable bool m_invalidateFont;

    protected:
        const ConstString & calcFontName() const;
        float calcLineOffset() const;
        float calcCharOffset() const;
        float calcMaxLength() const;

        const ColourValue & calcColorFont() const;
        const ColourValue & calcColorOutline() const;

        ETextHorizontAlign calcHorizontalAlign() const;
        ETextVerticalAlign calcVerticalAlign() const;

        float calcCharScale() const;

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    protected:
        ConstString m_key;

        mutable const TextEntryInterface * m_textEntry;
        mutable TVectorString m_textFormatArgs;

        mutable U32String m_cacheText;

        ETextHorizontAlign m_horizontAlign;
        ETextVerticalAlign m_verticalAlign;

        float m_charScale;

        float m_maxLength;

        ConstString m_fontName;

        float m_lineOffset;
        float m_charOffset;

        ColourValue m_colorFont;
        ColourValue m_colorOutline;

        uint32_t m_fontParams;

        uint32_t m_maxCharCount;

        mutable uint32_t m_charCount;
        mutable mt::vec2f m_textSize;

        bool m_wrap;
        bool m_outline;
        bool m_pixelsnap;

        bool m_debugMode;

        mutable TVectorTextLineLayout m_layouts;

        struct Chunk
        {
            uint32_t vertex_begin;
            uint32_t vertex_count;

            RenderMaterialInterfacePtr material;
        };

        typedef stdex::vector<Chunk> TVectorChunks;
        TVectorChunks m_chunks;

        TVectorRenderVertex2D m_vertexDataText;
        TVectorRenderVertex2D m_vertexDataOutline;

        TVectorRenderVertex2D m_vertexDataTextWM;
        TVectorRenderVertex2D m_vertexDataOutlineWM;

        mutable bool m_invalidateVertices;
        mutable bool m_invalidateVerticesWM;

        mutable bool m_invalidateTextLines;
        mutable bool m_invalidateTextEntry;
    };
    //////////////////////////////////////////////////////////////////////////
    inline const TVectorRenderVertex2D & TextField::getOutlineVertices( const TextFontInterfacePtr & _font )
    {
        if( m_invalidateVerticesWM == true )
        {
            this->updateVerticesWM_( _font );
        }

        return m_vertexDataOutlineWM;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const TVectorRenderVertex2D & TextField::getTextVertices( const TextFontInterfacePtr & _font )
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
            this->updateFont_();
        }

        return m_font;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const TextEntryInterface * TextField::getTextEntry() const
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
    }
}

#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/FontGlyphInterface.h"
#include "Interface/FontEffectInterface.h"
#include "Interface/RenderTextureInterface.h"
#include "Interface/PrefetcherObserverInterface.h"

#include "Kernel/Identity.h"
#include "Kernel/Observable.h"
#include "Kernel/Unknowable.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/GlyphCode.h"
#include "Kernel/Color.h"
#include "Kernel/VectorString.h"

#include "math/vec4.h"
#include "math/uv4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EFontParams : uint32_t
    {
        EFP_NONE = 0,
        EFP_FONT = 1 << 0,
        EFP_COLOR_FONT = 1 << 1,
        EFP_LINE_OFFSET = 1 << 2,
        EFP_CHAR_OFFSET = 1 << 3,
        EFP_MAX_LENGTH = 1 << 4,
        EFP_HORIZONTAL_ALIGN = 1 << 5,
        EFP_VERTICAL_ALIGN = 1 << 6,
        EFP_CHAR_SCALE = 1 << 7,
        EFP_AUTO_SCALE = 1 << 8,
    };
    //////////////////////////////////////////////////////////////////////////
    struct Glyph
    {
        float advance;

        mt::vec2f offset;
        mt::vec2f size;

        mt::uv4f uv;
        RenderTextureInterfacePtr texture;
    };
    //////////////////////////////////////////////////////////////////////////
    class FontInterface
        : public ServantInterface
        , public Identity
        , public Observable
        , public Unknowable
    {
    public:
        virtual void setGlyph( const FontGlyphInterfacePtr & _glyph ) = 0;
        virtual const FontGlyphInterfacePtr & getGlyph() const = 0;

    public:
        virtual void setEffect( const FontEffectInterfacePtr & _effect ) = 0;
        virtual const FontEffectInterfacePtr & getEffect() const = 0;

    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual bool compileFont() = 0;
        virtual void releaseFont() = 0;

    public:
        virtual bool isCompileFont() const = 0;

    public:
        virtual bool prefetch( const PrefetcherObserverInterfacePtr & _observer ) = 0;
        virtual void unfetch() = 0;

    public:
        virtual uint32_t getLayoutCount() const = 0;

    public:
        virtual void setHeight( uint32_t _height ) = 0;
        virtual uint32_t getHeight() const = 0;

        virtual void setFontColor( const Color & _color ) = 0;
        virtual const Color & getFontColor() const = 0;

        virtual void setLineOffset( float _lineOffset ) = 0;
        virtual float getLineOffset() const = 0;

        virtual void setCharOffset( float _charOffset ) = 0;
        virtual float getCharOffset() const = 0;

    public:
        virtual bool validateText( const ConstString & _key, const Char * _text, size_t _size ) const = 0;
        virtual bool prepareText( const Char * _text, size_t _size, U32String * const _out ) const = 0;
        virtual bool prepareGlyph( const U32String & _code, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual bool hasGlyph( GlyphCode _char ) const = 0;
        virtual bool getGlyph( uint32_t _layout, GlyphCode _char, GlyphCode _next, Glyph * const _glyph ) const = 0;

    public:
        virtual float getFontAscender() const = 0;
        virtual float getFontDescender() const = 0;
        virtual float getFontHeight() const = 0;
        virtual float getFontBearingYA() const = 0;
        virtual float getFontSpacing() const = 0;

    public:
        virtual uint32_t getFontParams() const = 0;

    public:
        virtual bool getFontPremultiply() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FontInterface> FontInterfacePtr;
    //////////////////////////////////////////////////////////////////////////   
}

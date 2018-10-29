#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/RenderTextureInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/GlyphChar.h"
#include "Kernel/ColourValue.h"

#include "Config/String.h"
#include "Config/VectorString.h"

#include "Kernel/Mixin.h"

#include "math/vec4.h"
#include "math/uv4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace IniUtil
    {
        struct IniStore;
    }
    //////////////////////////////////////////////////////////////////////////
    enum EFontParams
    {
        EFP_NONE = 0x00000000,
        EFP_FONT = 0x00000001,
        EFP_COLOR_FONT = 0x00000002,
        EFP_LINE_OFFSET = 0x00000008,
        EFP_CHAR_OFFSET = 0x00000010,
        EFP_MAX_LENGTH = 0x00000020,
        EFP_HORIZONTAL_ALIGN = 0x00000040,
        EFP_VERTICAL_ALIGN = 0x00000080,
        EFP_CHAR_SCALE = 0x00000100,
        EFP_MAX_VALUE = 0xffffffff
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
    class TextFontInterface
        : public ServantInterface
    {
    public:
        virtual void setName( const ConstString & _name ) = 0;
        virtual const ConstString & getName() const = 0;

    public:
        virtual bool initialize( const FileGroupInterfacePtr & _category, const IniUtil::IniStore & _ini ) = 0;

    public:
        virtual bool compileFont() = 0;
        virtual void releaseFont() = 0;

    public:
        virtual uint32_t getLayoutCount() const = 0;

    public:
        virtual void setColourFont( const ColourValue & _colour ) = 0;
        virtual void setLineOffset( float _lineOffset ) = 0;
        virtual void setCharOffset( float _charOffset ) = 0;

    public:
        virtual bool validateText( const ConstString & _key, const String & _text ) const = 0;
        virtual U32String prepareText( const String & _text ) = 0;
        virtual bool prepareGlyph( const U32String & _code ) = 0;

    public:
        virtual bool hasGlyph( GlyphCode _char ) const = 0;
        virtual bool getGlyph( uint32_t _layout, GlyphCode _char, GlyphCode _next, Glyph * _glyph ) const = 0;

    public:
        virtual float getFontAscent() const = 0;
        virtual float getFontDescent() const = 0;
        virtual float getFontHeight() const = 0;
        virtual float getFontBearingYA() const = 0;
        virtual float getFontSpacing() const = 0;

    public:
        virtual uint32_t getFontParams() const = 0;

        virtual const ColourValue & getFontColor() const = 0;

        virtual float getLineOffset() const = 0;
        virtual float getCharOffset() const = 0;

    public:
        virtual bool getFontPremultiply() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextFontInterface> TextFontInterfacePtr;
    //////////////////////////////////////////////////////////////////////////   
}

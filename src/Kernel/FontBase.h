#pragma once

#include "Interface/TextFontInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Compilable.h"
#include "Kernel/Reference.h"

namespace Mengine
{
    class FontBase
        : public TextFontInterface
        , public Compilable
        , public Reference
    {
    public:
        FontBase();
        ~FontBase() override;

    public:
        void setName( const ConstString & _name ) override;
        const ConstString & getName() const override;

    public:
        bool compileFont() override;
        void releaseFont() override;

    public:
        bool _incrementZero() override;
        void _decrementZero() override;

    public:
        void setColourFont( const ColourValue & _colour ) override;
        const ColourValue & getFontColor() const override;

        void setLineOffset( float _lineOffset ) override;
        float getLineOffset() const override;

        void setCharOffset( float _charOffset ) override;
        float getCharOffset() const override;

    public:
        uint32_t getFontParams() const override;

    public:
        bool validateText( const ConstString & _key, const String & _text ) const override;
        U32String prepareText( const String & _text ) override;
        bool prepareGlyph( const U32String & _text ) override;

    protected:
        bool initializeBase_( const IniUtil::IniStore & _ini );

    protected:
        virtual bool _validateGlyphes( const U32String & _codes ) const = 0;
        virtual bool _prepareGlyph( GlyphCode _code ) = 0;

    protected:
        ConstString m_name;

        uint32_t m_params;

        ColourValue m_colourFont;

        float m_lineOffset;
        float m_charOffset;
    };
}

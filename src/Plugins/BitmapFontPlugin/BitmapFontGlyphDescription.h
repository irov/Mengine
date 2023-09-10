#pragma once

#include "Interface/FileGroupInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/GlyphCode.h"
#include "Kernel/Vector.h"

#include "math/vec4.h"
#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct BitmapFontGlyphFace
    {
        GlyphCode code;

        mt::vec4f uv;
        mt::vec2f offset;
        float advance;
        mt::vec2f size;
    };
    //////////////////////////////////////////////////////////////////////////
    class BitmapFontGlyphDescription
        : public Factorable
    {
        DECLARE_FACTORABLE( BitmapFontGlyphDescription );

    public:
        BitmapFontGlyphDescription();
        ~BitmapFontGlyphDescription() override;

    public:
        bool initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath );

    public:
        void setSize( float _size );
        float getSize() const;

        void setHeight( uint32_t _size );
        uint32_t getHeight() const;

        void setAscender( float _ascender );
        float getAscender() const;

        void setDescender( float _descender );
        float getDescender() const;

    public:
        void setTextureWidth( uint32_t _textureWidth );
        void setTextureHeight( uint32_t _textureHeight );

    public:
        bool addGlyphCode( GlyphCode _code, const mt::vec4f & _uv, const mt::vec2f & _offset, float _advance, const mt::vec2f & _size );
        bool existGlyphCode( GlyphCode _code ) const;
        const BitmapFontGlyphFace * getGlyphFace( GlyphCode _code ) const;        

    public:
        void addKerning( GlyphCode _char, GlyphCode _next, float _kerning );
        float getKerning( GlyphCode _char, GlyphCode _next ) const;

    protected:
        float m_size;
        uint32_t m_height;

        float m_ascender;
        float m_descender;

        float m_textureInvWidth;
        float m_textureInvHeight;

        typedef Vector<BitmapFontGlyphFace> VectorFontGlyphFace;
        VectorFontGlyphFace m_chars[257];

        struct KerningDesc
        {
            GlyphCode code;
            GlyphCode next;
            float kerning;
        };

        typedef Vector<KerningDesc> VectorKerning;
        VectorKerning m_kernings[257];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BitmapFontGlyphDescription> BitmapFontGlyphDescriptionPtr;
    //////////////////////////////////////////////////////////////////////////
}
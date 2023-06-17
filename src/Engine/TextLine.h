#pragma once

#include "Interface/FontInterface.h"

#include "TextChar.h"

#include "Kernel/String.h"
#include "Kernel/Vector.h"
#include "Kernel/Color.h"
#include "Kernel/GlyphChar.h"

#include "math/vec4.h"
#include "math/mat4.h"
#include "math/uv4.h"

#include "math/box2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct CharData
    {
        GlyphCode code;

        float advance;

        mt::vec2f offset;
        mt::vec2f size;

        mt::uv4f uv;

        uint32_t fontId;

        RenderTextureInterfacePtr texture;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<CharData> VectorCharData;
    //////////////////////////////////////////////////////////////////////////
    class TextLine
    {
    public:
        TextLine( uint32_t _layout, float _charOffset );
        ~TextLine();

    public:
        bool initialize( uint32_t _fontId, const FontInterfacePtr & _font, const U32String & _text );

    public:
        const VectorCharData & getCharsData() const;

    public:
        float getLength() const;
        uint32_t getCharsDataSize() const;

    public:
        void calcCharPosition( const CharData & _cd, const mt::vec2f & _offset, float _charScale, uint32_t _index, mt::vec3f * const _pos ) const;
        void advanceCharOffset( const CharData & _cd, float _charScale, mt::vec2f * const _offset ) const;

    protected:
        uint32_t m_layout;
        float m_length;

        mutable float m_charOffset;

        mutable VectorCharData m_charsData;
    };
    //////////////////////////////////////////////////////////////////////////
};

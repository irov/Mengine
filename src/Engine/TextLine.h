#pragma once

#include "Interface/FontInterface.h"

#include "Engine/TextChar.h"

#include "Kernel/String.h"
#include "Kernel/Vector.h"
#include "Kernel/Color.h"
#include "Kernel/GlyphCode.h"

#include "math/vec4.h"
#include "math/mat4.h"
#include "math/uv4.h"

#include "math/box2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct TextCharData
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
    typedef Vector<TextCharData> VectorTextCharData;
    //////////////////////////////////////////////////////////////////////////
    class TextLine
    {
    public:
        TextLine( uint32_t _layout, float _charOffset, float _justifyLength );
        ~TextLine();

    public:
        bool initialize( uint32_t _fontId, const FontInterfacePtr & _font, const U32String & _text );

    public:
        const VectorTextCharData & getCharsData() const;

    public:
        float getLength() const;
        uint32_t getCharsDataSize() const;

    public:
        static void calcCharPosition( const TextCharData & _cd, const mt::vec2f & _offset, float _charScale, uint32_t _index, mt::vec3f * const _pos );
        void advanceCharOffset( const TextCharData & _cd, float _charScale, mt::vec2f * const _offset ) const;

    protected:
        uint32_t m_layout;
        float m_length;

        float m_charOffset;
        float m_justifyLength;
        float m_spaceAdvance;

        mutable VectorTextCharData m_charsData;
    };
    //////////////////////////////////////////////////////////////////////////
};

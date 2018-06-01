#pragma once

#include "Interface/TextInterface.h"

#include "TextChar.h"

#include "Config/Typedef.h"
#include "Config/String.h"
#include "Config/Vector.h"

#include "Core/ColourValue.h"
#include "Core/GlyphChar.h"

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
	typedef Vector<CharData> TVectorCharData;
    //////////////////////////////////////////////////////////////////////////
	class TextLine
	{
	public:
        TextLine( uint32_t _layout, float _charOffset );
        ~TextLine();

	public:
        bool initialize( uint32_t _fontId, const TextFontInterfacePtr & _font, const U32String & _text );

	public:
		const TVectorCharData & getCharsData() const;

    public:
		float getLength() const;
		uint32_t getCharsDataSize() const;

	public:
		void calcCharPosition( const CharData & _cd, const mt::vec2f & _offset, float _charScale, uint32_t _index, mt::vec3f & _pos ) const;
		void advanceCharOffset( const CharData & _cd, float _charScale, mt::vec2f & _offset ) const;

	private:
        uint32_t m_layout;
		float m_length;

		mutable float m_charOffset;
		mutable float m_offset;
		
		mutable TVectorCharData m_charsData;
		mutable bool m_invalidateTextLine;
	};
};

#	pragma once

#	include "Interface/TextInterface.h"

#	include "Config/Typedef.h"
#	include "Config/String.h"

#	include "Core/ColourValue.h"
#	include "Core/GlyphChar.h"

#	include "Math/vec4.h"
#	include "Math/mat4.h"
#	include "Math/uv4.h"

#	include "Math/box2.h"

namespace Menge
{
	struct CharData
	{
		GlyphCode code;
		mt::uv4f uv;
		mt::vec2f offset;
		mt::vec2f size;
		float advance;

		RenderTextureInterfacePtr texture;
	};

	typedef stdex::vector<CharData> TVectorCharData;

	class TextLine
	{
	public:
        TextLine( float _charOffset );
        ~TextLine();

	public:
		bool initialize( const TextFontInterfacePtr & _font, const U32String & _text );

	public:
		const TVectorCharData & getCharData() const;

    public:
		float getLength() const;
		uint32_t getCharsDataSize() const;

	public:
		void calcCharPosition( const CharData & _cd, const mt::vec2f & _offset, float _charScale, uint32_t _index, mt::vec3f & _pos ) const;
		void advanceCharOffset( const CharData & _cd, float _charScale, mt::vec2f & _offset ) const;

	private:
		float m_length;

		mutable float m_charOffset;
		mutable float m_offset;
		
		mutable TVectorCharData m_charsData;
		mutable bool m_invalidateTextLine;
	};
};

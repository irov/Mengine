#	pragma once

#	include "Kernel/ResourceReference.h"

#	include "Math/vec4.h"

#   include "Core/GlyphChar.h"

#   include "stdex/binary_vector.h"

namespace Menge
{
	class Glyph
	{
	public:
        Glyph();
		Glyph( const mt::vec4f & _uv, const mt::vec2f & _offset, float _ratio, const mt::vec2f & _size );

	public:
		inline const mt::vec4f & getUV() const;
		inline const mt::vec2f & getOffset() const;
		inline float getRatio() const;
		inline const mt::vec2f & getSize() const;

	public:
		void addKerning( GlyphChar _char, float _kerning );
		float getKerning( GlyphChar _char ) const;

	protected:
		mt::vec4f m_uv;
		mt::vec2f m_offset;
		float m_ratio;
		mt::vec2f m_size;

		typedef stdex::binary_vector<GlyphChar, float, GlyphCharLess> TMapKerning;
		TMapKerning m_kernings;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec4f & Glyph::getUV() const
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec2f & Glyph::getOffset() const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Glyph::getRatio() const
	{
		return m_ratio;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec2f & Glyph::getSize() const
	{
		return m_size;
	}
}
#	pragma once

#	include "Interface/TextInterface.h"

#	include "Factory/Factorable.h"

#	include "stdex/binary_vector.h"

namespace Menge
{
	class TextGlyphChar
		: public Factorable
	{
	public:
		TextGlyphChar();
		
	public:
		void initialize( const mt::vec4f & _uv, const mt::vec2f & _offset, float _ratio, const mt::vec2f & _size );

	public:
		inline const mt::vec4f & getUV() const;
		inline const mt::vec2f & getOffset() const;
		inline float getAdvance() const;
		inline const mt::vec2f & getSize() const;

	public:
		void addKerning( GlyphCode _char, float _kerning );
		float getKerning( GlyphCode _char ) const;

	protected:
		mt::vec4f m_uv;
		mt::vec2f m_offset;
		float m_advance;
		mt::vec2f m_size;

		typedef stdex::binary_vector<GlyphCode, float, GlyphCharLess> TMapKerning;
		TMapKerning m_kernings;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec4f & TextGlyphChar::getUV() const
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec2f & TextGlyphChar::getOffset() const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float TextGlyphChar::getAdvance() const
	{
		return m_advance;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec2f & TextGlyphChar::getSize() const
	{
		return m_size;
	}
}
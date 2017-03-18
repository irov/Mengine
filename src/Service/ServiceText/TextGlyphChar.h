#	pragma once

#	include "Interface/TextInterface.h"

#	include "Factory/Factorable.h"

#	include "stdex/stl_map.h"

#	include "stdex/intrusive_tree.h"

namespace Menge
{
	class TextGlyphChar
		: public Factorable
	{
	public:
		TextGlyphChar();
		
	public:
		void initialize( GlyphCode _code, const mt::vec4f & _uv, const mt::vec2f & _offset, float _ratio, const mt::vec2f & _size );

    public:
        inline const GlyphCode & getCode() const;

	public:
		inline const mt::vec4f & getUV() const;
		inline const mt::vec2f & getOffset() const;
		inline float getAdvance() const;
		inline const mt::vec2f & getSize() const;

	protected:
		GlyphCode m_code;

		mt::vec4f m_uv;
		mt::vec2f m_offset;
		float m_advance;
		mt::vec2f m_size;
	};
    //////////////////////////////////////////////////////////////////////////
    inline const GlyphCode & TextGlyphChar::getCode() const
    {
        return m_code;
    }
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
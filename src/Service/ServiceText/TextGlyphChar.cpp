#	include "TextGlyphChar.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextGlyphChar::TextGlyphChar()
		: m_uv(0.f, 0.f, 0.f, 0.f)
		, m_offset(0.f, 0.f)
		, m_advance(0.f)
		, m_size(0.f, 0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TextGlyphChar::initialize( GlyphCode _code, const mt::vec4f & _uv, const mt::vec2f & _offset, float _advance, const mt::vec2f & _size )
	{
		m_code = _code;
		m_uv = _uv;
		m_offset = _offset;
		m_advance = _advance;
		m_size = _size;
	}
}
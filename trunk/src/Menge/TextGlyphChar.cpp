#	include "TextGlyphChar.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TextGlyphChar::TextGlyphChar()
		: m_uv(0.f, 0.f, 0.f, 0.f)
		, m_offset(0.f, 0.f)
		, m_ratio(0.f)
		, m_size(0.f, 0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TextGlyphChar::initialize( const mt::vec4f & _uv, const mt::vec2f & _offset, float _ratio, const mt::vec2f & _size )
	{
		m_uv = _uv;
		m_offset = _offset;
		m_ratio = _ratio;
		m_size = _size;
	}
	//////////////////////////////////////////////////////////////////////////
	void TextGlyphChar::addKerning( GlyphCode _char, float _kerning )
	{		
		m_kernings.insert( _char, _kerning );
	}
	//////////////////////////////////////////////////////////////////////////
	float TextGlyphChar::getKerning( GlyphCode _char ) const
	{
		float kerning;
		if( m_kernings.has_copy( _char, kerning ) == false )
		{
			return 0.f;
		}

		return kerning;
	}
}
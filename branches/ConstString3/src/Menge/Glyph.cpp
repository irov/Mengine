#	include "ResourceGlyph.h"
#	include "ResourceImplement.h"

#	include <cstdio>

#	include "BinParser.h"

#	include "LogEngine.h"
#	include "Core/String.h"
#	include "Loadable.h"
#	include "LoaderEngine.h"

#	include "ServiceProvider.h"
#	include "Interface/UnicodeInterface.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Glyph::Glyph( const mt::vec4f & _uv, const mt::vec2f & _offset, float _ratio, const mt::vec2f & _size )
		: m_uv(_uv)
		, m_offset(_offset)
		, m_ratio(_ratio)
		, m_size(_size)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & Glyph::getUV() const
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Glyph::getOffset() const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	float Glyph::getRatio() const
	{
		return m_ratio;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Glyph::getSize() const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	void Glyph::addKerning( wchar_t _char, float _kerning )
	{		
		m_kernings.insert( std::make_pair( _char, _kerning ) );
	}
	//////////////////////////////////////////////////////////////////////////
	float Glyph::getKerning( wchar_t _char ) const
	{
		TMapKerning::const_iterator it_found = m_kernings.find( _char );

		if( it_found == m_kernings.end() )
		{
			return 0.f;
		}

		float kerning = it_found->second;

		return kerning;
	}
}
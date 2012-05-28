#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"

#	include <map>

namespace Menge
{
	class Glyph
	{
	public:
		Glyph( const mt::vec4f & _uv, const mt::vec2f & _offset, float _ratio, const mt::vec2f & _size );

	public:
		const mt::vec4f & getUV() const;
		const mt::vec2f & getOffset() const;
		float getRatio() const;
		const mt::vec2f & getSize() const;

	public:
		void addKerning( unsigned int _char, float _kerning );
		float getKerning( unsigned int _char ) const;

	protected:
		mt::vec4f m_uv;
		mt::vec2f m_offset;
		float m_ratio;
		mt::vec2f m_size;

		typedef std::map<unsigned int, float> TMapKerning;
		TMapKerning m_kernings;
	};
}
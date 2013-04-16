#	pragma once

#	include "Kernel/ResourceReference.h"

#	include "Glyph.h"

#	include "Math/vec4.h"

#	include <map>

namespace Menge
{
	class ResourceGlyph
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceGlyph )
		
	public:
		ResourceGlyph();
        ~ResourceGlyph();
		
	public:
		bool hasGlyph( WChar _id, const Glyph ** _glyph ) const;
		const Glyph * getGlyph( WChar _id ) const;
	
	public:
		float getHeight() const;
		
	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		Glyph & addGlyph_( WChar _glyph, const mt::vec4f & _rect, const mt::vec2f & _offset, float _width );

	protected:
		float m_height;

		typedef std::map<WChar, Glyph> TMapGlyph;
		TMapGlyph m_glyphs;
	};
}
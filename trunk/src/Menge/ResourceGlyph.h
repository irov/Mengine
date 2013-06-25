#	pragma once

#	include "Kernel/ResourceReference.h"

#	include "Glyph.h"

#   include "Utils/Core/BinaryVector.h"

#	include "Math/vec4.h"

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
		bool hasGlyph( GlyphChar _id, const Glyph ** _glyph ) const;
		const Glyph * getGlyph( GlyphChar _id ) const;
	
	public:
		float getFontHeight() const;
		
	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		Glyph & addGlyph_( GlyphChar _glyph, const mt::vec4f & _rect, const mt::vec2f & _offset, float _width );

	protected:
		float m_fontHeight;

		typedef BinaryVector<GlyphChar, Glyph, GlyphCharLess> TMapGlyph;
		TMapGlyph m_glyphs;
	};
}
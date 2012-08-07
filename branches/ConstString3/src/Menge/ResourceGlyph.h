#	pragma once

#	include "ResourceReference.h"

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
		
	public:
		bool hasGlyph( wchar_t _id ) const;
		const Glyph * getGlyph( wchar_t _id ) const;
	
	public:
		float getHeight() const;
		
	public:
		void loader( const Metabuf::Metadata * _parser ) override;
		void loaderKerning_( BinParser * _parser, Glyph & _glyph );
		void loaderGlyph_( BinParser * _parser );

	protected:
		Glyph & addGlyph_( wchar_t _glyph, const mt::vec4f & _rect, const mt::vec2f & _offset, float _width );

	protected:
		float m_height;

		typedef std::map<wchar_t, Glyph> TMapGlyph;
		TMapGlyph m_glyphs;
	};
}
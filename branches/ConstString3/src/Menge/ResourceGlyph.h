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
		bool hasGlyph( unsigned int _id ) const;
		const Glyph * getGlyph( unsigned int _id ) const;
	
	public:
		float getHeight() const;
		
	public:
		void loader( BinParser * _parser ) override;
		void loaderKerning_( BinParser * _parser, Glyph & _glyph );
		void loaderGlyph_( BinParser * _parser );

	protected:
		Glyph & addGlyph_( const String& _glyph, const Int4 & _rect, const Int2 & _offset, float _width );

	protected:
		float m_height;

		typedef std::map<unsigned int, Glyph> TMapGlyph;
		TMapGlyph m_glyphs;
	};
}
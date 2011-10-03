#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"

#	include <map>

namespace Menge
{
	class ResourceGlyph
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceGlyph )

	public:
		struct Glyph
		{
			mt::vec4f uv;
			mt::vec2f offset;
			float ratio;
			mt::vec2f size;
		};

	public:
		ResourceGlyph();

	public:
		const Glyph * getGlyph( unsigned int _id ) const;
		float getInitSize() const;

	public:
		void loader( BinParser * _parser ) override;

		void loaderKerning_( BinParser * _parser, Glyph & _glyph );

	protected:
		Glyph & addGlyph_( const String& _glyph, const String& _rect, const String& _offset, float _width );

	protected:
		float m_initSize;

		typedef std::map<unsigned int, Glyph> TMapGlyph;
		TMapGlyph m_glyphs;
	};
}
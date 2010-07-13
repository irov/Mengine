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
			unsigned int id;
			mt::vec4f uv;
			mt::vec2f offset;
			float ratio;
			mt::vec2f size;
		};

	public:
		ResourceGlyph();

	public:
		const Glyph * getGlyph( unsigned int _id ) const;

	public:
		void loader( BinParser * _parser ) override;

	protected:
		void addGlyph_( const String& _glyph, const String& _rect, const String& _offset, int _width );
		void setGlyph_( unsigned int _id, const mt::vec4f& _uv, const mt::vec2f& _offset, float _ratio, const mt::vec2f& _size );

	protected:
		float m_initSize;

		typedef std::map<unsigned int, Glyph> TMapGlyph;
		TMapGlyph m_glyphs;
	};
}
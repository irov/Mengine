#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"
#	include <vector>
#	include <map>

class RenderImageInterface;
class FileDataInterface;

namespace Menge
{
	class ResourceFont
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceFont )

	public:
		ResourceFont( const std::string & _name );

	public:

		struct UVRect
		{
			float u1;
			float v1;
			float u2;
			float v2;

			UVRect( float _left, float _top, float _right, float _bottom )
				: u1( _left ), v1( _top ), u2( _right ), v2( _bottom )
			{}
		};

		UVRect getUV( unsigned int _id ) const;

		float getCharWidth( char _id ) const;
		float getHeight() const;

		const RenderImageInterface * getImage() const;

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		bool _compile();
		void _release();

	private:

		struct Glyph
		{
			Glyph( unsigned int _id, const UVRect& _rect, float _ratio )
				: id(_id)
				, rect(_rect)
				, ratio(_ratio)
			{}
			unsigned int id;
			UVRect rect;
			float ratio;
		};

		typedef std::map<unsigned int, Glyph> TMapGlyph;

		float m_whitespaceWidth;
		float m_height;

		TMapGlyph	m_glyphs;

		std::string m_fontName;
		std::string m_fontDir;

		RenderImageInterface * m_image;

		void setGlyph( unsigned int id, float u1, float v1, float u2, float v2, float textureAspect );
		std::string getFontDir( const std::string & _fontName );

		bool parseFontdef( FileDataInterface * _stream );
		bool parseAttribute( const std::vector<std::string> & _params );
		std::vector<std::string> splitParams( const std::string& str, const std::string& delims = "\t\n ", unsigned int maxSplits = 0);
	};
}
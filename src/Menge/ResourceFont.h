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

		const mt::vec4f & getUV( unsigned int _id ) const;

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
			Glyph( unsigned int _id, const mt::vec4f& _rect, float _ratio )
				: id(_id)
				, rect(_rect)
				, ratio(_ratio)
			{}
			unsigned int id;
			mt::vec4f rect;
			float ratio;
		};

		typedef std::map<unsigned int, Glyph> TMapGlyph;

		float m_whitespaceWidth;
		float m_height;

		TMapGlyph	m_glyphs;

		std::string m_fontName;
		std::string m_fontDir;

		RenderImageInterface * m_image;

		void setGlyph( unsigned int _id, float _u1, float _v1, float _u2, float _v2 );
		std::string getFontDir( const std::string & _fontName );

		bool parseFontdef( FileDataInterface * _stream );
		bool parseAttribute( const std::vector<std::string> & _params );
		void splitParams( std::vector<std::string> & _words, const std::string & _text, const std::string & _separator = "\t\n ");
	};
}
#	pragma once

#	include "Config/Typedef.h"
#	include "Math/vec4.h"
#	include "Math/mat4.h"
#	include "Core/ColourValue.h"
#	include "Math/box2.h"
#	include "Vertex.h"

namespace Menge
{
	class TextField;
	class ResourceFont;
	
	struct CharData
	{
		uint32 code;
		mt::vec2f renderVertex[4];
		mt::vec4f uv;
		mt::vec2f offset;
		mt::vec2f size;
		float ratio;
	};

	typedef std::vector<CharData> TCharsData;

	class TextLine
	{
	public:
		TextLine( TextField * _textField );

	public:
		void initialize( const ResourceFont * _resource, const String& _text );

	public:
		void prepareRenderObject(
			mt::vec2f & offset,
			unsigned int _argb,
			TVectorVertex2D& _renderObject );

		void invalidateRenderLine();
		void updateBoundingBox( mt::box2f& _boundingBox, mt::vec2f& _offset );

		float getLength() const;
		float getWidth() const;

	private:
		TextField * m_textField;
		bool m_invalidateRenderLine;
		void updateRenderLine_( mt::vec2f& _offset );
		float m_offset;
		float  m_length;
		TCharsData charsData;
	};
};

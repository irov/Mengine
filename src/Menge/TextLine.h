#	pragma once

#	include "Config/Typedef.h"
#	include "math/vec4.h"
#	include "Math/mat4.h"
#	include "ColourValue.h"

namespace Menge
{
	class TextField;
	class ResourceFont;
	class RenderImageInterface;

	struct CharData
	{
		String::value_type code;
		mt::vec2f renderVertex[4];
		mt::vec4f uv;
		float ratio;
	};

	typedef std::vector<CharData> TCharsData;

	class TextLine
	{
		public:
			TextLine( TextField & _textField, const ResourceFont * _resource, const String & _text );

		public:

			void renderLine(
				mt::vec2f & offset,
				const ColourValue & _color, 
				const RenderImageInterface * _renderImage);

			void invalidateRenderLine();
			bool isInvalidateRenderVertex() const;

			float getLength() const;

		private:
			TextField & m_textField;
			bool m_invalidateRenderVertex;
			float m_offset;
			float  m_length;
			TCharsData charsData;
	};

};
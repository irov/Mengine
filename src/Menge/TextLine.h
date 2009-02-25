#	pragma once

#	include "Config/Typedef.h"
#	include "math/vec4.h"
#	include "Math/mat4.h"
#	include "ColourValue.h"
#	include "Math/box2.h"

namespace Menge
{
	class TextField;
	class ResourceFont;
	class RenderObject;
	
	struct CharData
	{
		uint32 code;
		mt::vec2f renderVertex[4];
		mt::vec4f uv;
		float ratio;
	};

	typedef std::vector<CharData> TCharsData;

	class TextLine
	{
		public:
			TextLine( TextField & _textField, const ResourceFont * _resource, const String& _text );

		public:

			void prepareRenderObject(
				mt::vec2f & offset,
				ColourValue& _color, 
				RenderObject* _renderObject );

			void invalidateRenderLine();
			void updateBoundingBox( mt::box2f& _boundingBox, mt::vec2f& _offset );

			float getLength() const;

		private:
			TextField & m_textField;
			bool m_invalidateRenderLine;
			void updateRenderLine_( mt::vec2f& _offset );
			float m_offset;
			float  m_length;
			TCharsData charsData;
	};
};

#	pragma once

#	include "Config/Typedef.h"
#	include "Math/vec4.h"
#	include "Math/mat4.h"
#	include "Core/ColourValue.h"
#	include "Math/box2.h"
#	include "Vertex.h"

namespace Menge
{
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
		TextLine( float _height, float _charOffset );

	public:
		void initialize( const ResourceFont * _resource, const String& _text );

	public:
		void prepareRenderObject(
			mt::vec2f & offset
			, const mt::mat3f & _wm
			, unsigned int _argb
			, bool _pixelsnap
			, TVectorVertex2D & _renderObject );

		void invalidateRenderLine();
		void updateBoundingBox( mt::vec2f& _offset, const mt::mat3f & _wm, mt::box2f& _boundingBox );

		float getLength() const;
		float getWidth() const;
		int getCharsDataSize() const;

	private:
		void updateRenderLine_( mt::vec2f& _offset, const mt::mat3f & _wm );

	private:
		float m_charOffset;
		float m_height;
				
		float m_offset;
		float m_length;
		
		TCharsData m_charsData;

		bool m_invalidateRenderLine;
	};
};

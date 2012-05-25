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
		unsigned int code;
		mt::vec3f renderVertex[4];
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
		void initialize( const ResourceFont * _resource, const WString& _text );

	public:
		void prepareRenderObject(
			mt::vec2f & offset
			, const mt::mat4f & _wm
			, unsigned int _argb
			, bool _pixelsnap
			, TVectorVertex2D & _renderObject ) const;

		void invalidateRenderLine() const;
		void updateBoundingBox( mt::vec2f& _offset, const mt::mat4f & _wm, mt::box2f& _boundingBox );

		float getLength() const;
		float getWidth() const;
		int getCharsDataSize() const;

	private:
		void updateRenderLine_( mt::vec2f& _offset, const mt::mat4f & _wm ) const;

	private:
		float m_height;	
		float m_length;

		mutable float m_charOffset;
		mutable float m_offset;
		
		mutable TCharsData m_charsData;
		mutable bool m_invalidateRenderLine;
	};
};

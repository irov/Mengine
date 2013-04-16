#	pragma once

#   include "Interface/RenderSystemInterface.h"

#	include "Config/Typedef.h"

#	include "Core/ColourValue.h"

#	include "Math/vec4.h"
#	include "Math/mat4.h"

#	include "Math/box2.h"

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

	typedef std::vector<CharData> TVectorCharData;

	class TextLine
	{
	public:
		TextLine( ServiceProviderInterface * _serviceProvider, float _height, float _charOffset );

	public:
		void initialize( const ResourceFont * _resource, const WString& _text );

	public:
		void prepareRenderObject(
			mt::vec2f & offset
			, const mt::vec4f & _uv
			, unsigned int _argb
            , bool _pixelsnap
			, TVectorVertex2D & _renderObject ) const;

    public:
		float getLength() const;
		float getWidth() const;
		int getCharsDataSize() const;

	private:
		void updateRenderLine_( mt::vec2f& _offset ) const;

	private:
        ServiceProviderInterface * m_serviceProvider;

		float m_height;	
		float m_length;

		mutable float m_charOffset;
		mutable float m_offset;
		
		mutable TVectorCharData m_charsData;
		mutable bool m_invalidateTextLine;
	};
};

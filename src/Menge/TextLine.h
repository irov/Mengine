#	pragma once

#   include "Interface/RenderSystemInterface.h"
#	include "Interface/TextInterface.h"

#	include "Config/Typedef.h"
#	include "Config/String.h"

#	include "Core/ColourValue.h"
#	include "Core/GlyphChar.h"

#	include "Math/vec4.h"
#	include "Math/mat4.h"

#	include "Math/box2.h"

namespace Menge
{
	struct CharData
	{
		GlyphCode code;
		mt::vec2f vertex[4];
		mt::vec4f uv;
		mt::vec2f offset;
		mt::vec2f size;
		float advance;
	};

	typedef stdex::vector<CharData> TVectorCharData;

	class TextLine
	{
	public:
		TextLine( ServiceProviderInterface * _serviceProvider, float _charOffset );

	public:
		bool initialize( const TextFontInterfacePtr & _font, const String& _text );

	public:
		void prepareRenderObject( mt::vec2f & _offset
			, const mt::uv4f & _uv
			, ColourValue_ARGB _argb
            , bool _pixelsnap
			, TVectorRenderVertex2D & _renderObject ) const;

    public:
		float getLength() const;
		int getCharsDataSize() const;

	private:
		void updateRenderLine_( bool _pixelsnap, mt::vec2f & _offset ) const;

	private:
        ServiceProviderInterface * m_serviceProvider;

		float m_length;

		mutable float m_charOffset;
		mutable float m_offset;
		
		mutable TVectorCharData m_charsData;
		mutable bool m_invalidateTextLine;
	};
};

#	pragma once

#	include <string>

#	include "math/mat3.h"
#	include "math/vec4.h"

#	include "Interface/RenderSystemInterface.h"

#	include "Holder.h"

namespace Menge
{
	class FileEngine;

	class RenderEngine
	{
	public:
		RenderEngine( RenderSystemInterface * _interface );
		
	public:
		void drawLine(const mt::vec2f& p1, const mt::vec2f& p2, float width, unsigned long color);

		RenderImageInterface* loadImage(const std::string &	_imageFile, int _filter);

		void renderImage(
			const mt::mat3f& _transform, 
			const mt::vec2f& _offset,
			const mt::vec4f& _uv,
			const mt::vec2f& _size,
			unsigned int _mixedColor, 
			RenderImageInterface* _rmi);

		void releaseRenderImage( RenderImageInterface* _rmi );

		void update();
		
		//////////////////////////////////////////////////////////////////////////
		RenderFontInterface* loadFont(const std::string &_fontXml);

		void renderText(
			const mt::vec2f & _pos, 
			RenderFontInterface* _font, 
			const std::string & _text);

		void releaseRenderFont( RenderFontInterface* _fnt );

		RenderImageInterface* loadImage( const TextureDesc& _desc );

	protected:
		RenderSystemInterface * m_interface;
	};
}
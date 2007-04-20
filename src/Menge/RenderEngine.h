#	pragma once

#	include "RenderSystemInterface.h"

#	include "SystemDLL.h"
#	include "Holder.h"

#	include <string>

#	include "math/mat3.h"
#	include "math/vec2.h"

namespace Menge
{
	class FileEngine;

	class RenderEngine
		: public SystemDLL<RenderSystemInterface>
	{
	public:
		RenderEngine(const std::string &_dllModule);
		
	public:
		bool createDisplay(int _width, int _height, int _bits, bool _fullScreen);
		
		bool beginSceneDrawing(unsigned long _color);

		bool endSceneDrawing();

		void drawLine(const mt::vec2f& p1, const mt::vec2f& p2, float width, unsigned long color);

		RenderImageInterface* loadImage(const std::string &	_imageFile, bool _haveAlpha = false);

		void renderImage(
			const mt::mat3f& _transform, 
			unsigned int _mixedColor, 
			RenderImageInterface* _rmi);

		void renderImageOffset(
			const mt::mat3f& _transform,
			const mt::vec2f& _offset,
			unsigned int _mixedColor,
			RenderImageInterface * _rmi);

		void renderImageUV(const mt::mat3f& _transform, unsigned int _mixedColor, float u0, float v0, float u1, float v1, float width, float height, RenderImageInterface* _rmi);

		void releaseRenderImage(RenderImageInterface* _rmi);
		
		//////////////////////////////////////////////////////////////////////////
		RenderFontInterface* loadFont(const std::string &_fontXml);

		void renderText(
			mt::vec2f _pos, 
			RenderFontInterface* _font, 
			const std::string& _text);


		void releaseRenderFont(RenderFontInterface* _fnt);

		RenderImageInterface* loadImage(const TextureDesc& _desc);
	};
}
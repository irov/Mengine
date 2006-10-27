#	pragma once

#	include "DllModule.h"
#	include "Keeper.h"

#	include <string>

#	include "math/mat3.h"
#	include "math/vec2.h"

class RenderSystemInterface;
class RenderFontInterface;
class RenderImageInterface;
struct TextureDesc;

namespace Menge
{
	class FileEngine;

	class RenderEngine
		: public DllModuleInterface<RenderSystemInterface>
	{
	public:
		RenderEngine(const std::string &_dllModule);
		
	public:
		bool createDisplay(
			unsigned int _width, 
			unsigned int _height, 
			unsigned int _bits, 
			bool _fullScreen);
		
		bool beginSceneDrawing(unsigned long _color);

		bool endSceneDrawing();

		void drawLine(const mt::vec2f& p1, const mt::vec2f& p2, float width, unsigned long color);

		//////////////////////////////////////////////////////////////////////////
		RenderImageInterface* loadImage(const std::string &	_imageFile, bool _haveAlpha = false);

		void renderImage(
			const mt::mat3f& _transform, 
			unsigned int _mixedColor, 
			RenderImageInterface* _rmi);

		void releaseRenderImage(RenderImageInterface* _rmi);
		
		//////////////////////////////////////////////////////////////////////////
		RenderFontInterface* loadFont(const std::string &_fontXml);

		void renderText(
			mt::vec2f _pos, 
			RenderFontInterface* _font, 
			const std::string& _text);


		void releaseRenderFont(RenderFontInterface* _fnt);

		/*	Добавлено 17.10.2006 в 16:17	*/
		RenderImageInterface* loadImage(const TextureDesc& _desc);
	};
}
#	include "RenderEngine.h"

#	include "RenderSystemInterface.h"

#	include "FileSystemInterface.h"
#	include "FileEngine.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::RenderEngine(const std::string &_dllModule)
		: DllModuleInterface<RenderSystemInterface>(_dllModule)
	{
		Keeper<RenderEngine>::keep(this);
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface* RenderEngine::loadImage(const TextureDesc& _desc)
	{
		RenderImageInterface * image = m_interface->loadImage(_desc);
		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::createDisplay(
		unsigned int _width, 
		unsigned int _height, 
		unsigned int _bits, 
		bool _fullScreen)
	{
		return m_interface->createDisplay(_width,_height,_bits,_fullScreen);
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::beginSceneDrawing(unsigned long _color)
	{
		return m_interface->beginSceneDrawing(_color);
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::endSceneDrawing()
	{
		return m_interface->endSceneDrawing();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::drawLine(const mt::vec2f& p1, const mt::vec2f& p2, float width, unsigned long color)
	{
		m_interface->drawLine(p1,p2,width,color);
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface* RenderEngine::loadImage(const std::string & _imageFile, bool _haveAlpha)
	{
		FileDataInterface* imageData = 
			Keeper<FileEngine>::hostage()->openFile(_imageFile);

		if( imageData == 0 )
		{
			return 0;
		}

		TextureDesc desc;

		desc.buffer = (void*)imageData->getBuffer();
		desc.size = imageData->size();
		desc.haveAlpha = _haveAlpha;

		RenderImageInterface * image = m_interface->loadImage(desc);

		return image;
	}

	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderImage(
		const mt::mat3f& _transform, 
		unsigned int _mixedColor, 
		RenderImageInterface* _rmi)
	{
		m_interface->renderImage(_transform,_mixedColor,_rmi);
	}

	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderImageOffset(
		const mt::mat3f& _transform, 
		const mt::vec2f& _offset,
		unsigned int _mixedColor, 
		RenderImageInterface* _rmi)
	{
		m_interface->renderImageOffset(_transform,_offset,_mixedColor,_rmi);
	}

	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseRenderImage(RenderImageInterface* _rmi)
	{
		m_interface->releaseRenderImage(_rmi);
	}

	//////////////////////////////////////////////////////////////////////////
	RenderFontInterface* RenderEngine::loadFont(const std::string &_fontXml)
	{
		//Fix
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderText(
		mt::vec2f _pos, 
		RenderFontInterface* _font, 
		const std::string& _text)
	{
		m_interface->renderText(_pos,_font,_text);
	}

	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseRenderFont(RenderFontInterface* _fnt)
	{
		m_interface->releaseRenderFont(_fnt);
	}
}
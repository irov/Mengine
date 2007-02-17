#	include "RenderEngine.h"

#	include "FileEngine.h"
#	include "XmlParser.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::RenderEngine(const std::string &_dllModule)
		: DllModuleInterface<RenderSystemInterface>(_dllModule)
	{
		Holder<RenderEngine>::keep(this);
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
			Holder<FileEngine>::hostage()->openFile(_imageFile);

		if(imageData == 0)
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
		FontDesc desc;

		float scaleW = -1;
		float scaleH = -1;

		std::string name;
		std::string ch;
		std::string origin;
		std::string size;

		float aw;
		float x;
		float y;
		float width;
		float height;
		int id;

		FileDataInterface* imageData = 0;

		desc.color = 0xFFFFFFFF;

		XML_PARSE_FILE(_fontXml)
		{
			XML_VALUE_ATTRIBUTE("height",desc.height);

			XML_CHECK_NODE_FOR_EACH("font")
			{
				XML_CHECK_NODE_FOR_EACH("bitmaps")
				{
					XML_CHECK_NODE("bitmap")
					{
						XML_VALUE_ATTRIBUTE("name",name);
						XML_VALUE_ATTRIBUTE("size",size);

						assert(!name.empty());
						assert(!size.empty());

						imageData = 
							Holder<FileEngine>::hostage()->openFile(name);

						if(imageData == 0)
						{
							assert(!"no font image");
							return 0;
						}

						desc.texDesc.buffer = (void*)imageData->getBuffer();
						desc.texDesc.size = imageData->size();
						desc.texDesc.haveAlpha = true;

						sscanf(size.c_str(), "%fx%f", &scaleW, &scaleH); 

						assert((scaleW > 0 && scaleW <= 1024));
						assert((scaleH > 0 && scaleH <= 1024));
					}
				}

				XML_CHECK_NODE_FOR_EACH("glyphs")
				{
					XML_CHECK_NODE("glyph")
					{
						XML_VALUE_ATTRIBUTE("ch",ch);
						XML_VALUE_ATTRIBUTE("origin",origin);
						XML_VALUE_ATTRIBUTE("size",size);
						XML_VALUE_ATTRIBUTE("aw",aw);

					    sscanf(origin.c_str(), "%f,%f", &x,&y); 
						sscanf(size.c_str(), "%fx%f", &width, &height); 

						id = int(ch[0]);
						
						desc.chars[id].uv.x = x / scaleW;
						desc.chars[id].uv.y = y / scaleH;
						desc.chars[id].uv.z = (x + width) / scaleW;
						desc.chars[id].uv.w = (y + height) / scaleH;
						desc.chars[id].width = aw;
					}
				}
			}
		}
		XML_INVALID_PARSE()
		{
			assert(!"Error in font loader!");
			return false;
		}
		
		RenderFontInterface* f = m_interface->loadFont(desc);

		Holder<FileEngine>::hostage()->closeFile(imageData);

		return f;
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
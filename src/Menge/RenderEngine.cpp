#	include "RenderEngine.h"

#	include "FileEngine.h"
#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::RenderEngine( RenderSystemInterface * _interface )
		: m_interface( _interface )
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
	void	RenderEngine::setMaterialColor(unsigned char _ambient[4], 
		unsigned char _diffuse[4],
		unsigned char _specular[4])
	{
		return m_interface->setMaterialColor(_ambient,_diffuse,_specular);
	}
	//////////////////////////////////////////////////////////////////////////
	VertexData * RenderEngine::createVertexData()
	{
		return m_interface->createVertexData();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::drawPrimitive(PrimitiveData * _pd)
	{
		return m_interface->drawPrimitive(_pd);
	}
	//////////////////////////////////////////////////////////////////////////
	IndexData * RenderEngine::createIndexData()
	{
		return m_interface->createIndexData();
	}
	//////////////////////////////////////////////////////////////////////////
	VertexDeclaration * RenderEngine::createVertexDeclaration()
	{
		return m_interface->createVertexDeclaration();
	}
	//////////////////////////////////////////////////////////////////////////
	Texture * RenderEngine::createTextureInMemory(const TextureDesc& _desc)
	{
		return m_interface->createTextureInMemory(_desc);
	}
	//////////////////////////////////////////////////////////////////////////
	void	RenderEngine::releaseTexture(Texture * _tex)
	{
		return m_interface->releaseTexture(_tex);
	}
	//////////////////////////////////////////////////////////////////////////
	void	RenderEngine::setTexture(Texture * _tex)
	{
		return m_interface->setTexture(_tex);
	}
	//////////////////////////////////////////////////////////////////////////
	void	RenderEngine::setProjectionMatrix(const mt::mat4f& _projection)
	{
		return m_interface->setProjectionMatrix(_projection);
	}
	//////////////////////////////////////////////////////////////////////////
	void	RenderEngine::setViewMatrix(const mt::mat4f& _view)
	{
		return m_interface->setViewMatrix(_view);
	}
	//////////////////////////////////////////////////////////////////////////
	void	RenderEngine::setWorldMatrix(const mt::mat4f& _world)
	{
		return m_interface->setWorldMatrix(_world);
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::beginScene(bool backBuffer, bool zBuffer, int color)
	{
		return m_interface->beginScene(backBuffer,zBuffer,color);
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderEngine::endScene()
	{
		return m_interface->endScene();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::drawLine3D(const mt::vec3f& p1, const mt::vec3f& p2, unsigned long color)
	{
		return m_interface->drawLine3D(p1,p2,color);
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::drawLine(const mt::vec2f& p1, const mt::vec2f& p2, unsigned long color)
	{
		return m_interface->drawLine(p1,p2,color);
	}
	void RenderEngine::drawBox( const mt::vec3f & MinEdge, const mt::vec3f & MaxEdge, unsigned long _color)
	{
		return m_interface->drawBox(MinEdge,MaxEdge,_color);
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface* RenderEngine::loadImage(const std::string & _imageFile, int _filter)
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
		desc.filter = _filter;

		RenderImageInterface * image = m_interface->loadImage(desc);

		Holder<FileEngine>::hostage()->closeFile(imageData);

		return image;
	}

	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderImage(
		const mt::mat3f& _transform, 
		const mt::vec2f& _offset,
		const mt::vec4f& _uv,
		const mt::vec2f& _size,
		unsigned int _mixedColor, 
		RenderImageInterface* _rmi)
	{
		m_interface->renderImage(
			_transform, 
			_offset,
			_uv,
			_size,
			_mixedColor,
			_rmi);
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
						desc.texDesc.filter = 1;

						sscanf_s(size.c_str(), "%fx%f", &scaleW, &scaleH); 

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

					    sscanf_s(origin.c_str(), "%f,%f", &x,&y); 
						sscanf_s(size.c_str(), "%fx%f", &width, &height); 

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
		const mt::vec2f & _pos, 
		RenderFontInterface* _font, 
		const std::string& _text)
	{
		m_interface->renderText( _pos, _font, _text.c_str() );
	}

	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseRenderFont(RenderFontInterface* _fnt)
	{
		m_interface->releaseRenderFont(_fnt);
	}
}
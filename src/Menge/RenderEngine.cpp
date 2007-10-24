#	include "RenderEngine.h"

#	include "FileEngine.h"
#	include "XmlParser/XmlParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::RenderEngine( RenderSystemInterface * _interface )
		: m_interface( _interface )
	{
		Holder<RenderEngine>::keep( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::render()
	{
		m_interface->render();
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::beginLayer()
	{
		m_interface->beginLayer();
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::endLayer()
	{
		m_interface->endLayer();
	}
	//////////////////////////////////////////////////////////////////////////
	void	RenderEngine::setProjectionMatrix( const mt::mat4f& _projection )
	{
		return m_interface->setProjectionMatrix( _projection.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void	RenderEngine::setViewMatrix( const mt::mat4f& _view )
	{
		return m_interface->setViewMatrix( _view.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void	RenderEngine::setWorldMatrix( const mt::mat4f& _world )
	{
		return m_interface->setWorldMatrix( _world.m );
	}	
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface* RenderEngine::loadImage( const TextureDesc & _desc )
	{
		RenderImageInterface * image = m_interface->loadImage( _desc );
		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * RenderEngine::loadImage( const std::string & _fileName, std::vector<char> & _buff, size_t _filter )
	{
		FileDataInterface * fileData = Holder<FileEngine>::hostage()->openFile( _fileName );

		if( fileData == 0 )
		{
			printf("Error: Image can't open resource file '%s'\n"
				, _fileName.c_str()
				);

			return 0;
		}

		size_t buff_size = fileData->size();
		_buff.resize( buff_size );
		fileData->read( &_buff[0], fileData->size() );

		TextureDesc	textureDesc;
		textureDesc.buffer = &_buff[0];
		textureDesc.size = buff_size;
		textureDesc.name = _fileName.c_str();
		textureDesc.filter = _filter;

		RenderImageInterface * image = loadImage( textureDesc );

		Holder<FileEngine>::hostage()->closeFile( fileData );

		if( image == 0 )
		{
			printf("Error: Image from file '%s' not loader\n"
				, _fileName.c_str() 
				);

			return 0;
		}	

		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderImage(
		const mt::mat3f & _transform, 
		const mt::vec2f & _offset,
		const mt::vec4f & _uv,
		const mt::vec2f & _size,
		unsigned int _mixedColor, 
		const RenderImageInterface * _image)
	{
		m_interface->renderImage(
			_transform.m, 
			_offset.m,
			_uv.m,
			_size.m,
			_mixedColor,
			_image);
	}

	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseImage( RenderImageInterface * _image )
	{
		m_interface->releaseImage( _image );
	}
	////////////////////////////////////////////////////////////////////////////
	RenderFontInterface* RenderEngine::loadFont( const std::string & _font )
	{
		FontDesc desc;
		desc.name = _font.c_str();
		return m_interface->loadFont( desc );
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderText(
		const RenderFontInterface * _font, 
		const std::string & _text,
		const float * _transform)
	{
		m_interface->renderText( _font, _text.c_str(), _transform );
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::releaseRenderFont( RenderFontInterface* _fnt )
	{
		m_interface->releaseRenderFont( _fnt );
	}
	////////////////////////////////////////////////////////////////////////////
	//void	RenderEngine::setMaterialColor(unsigned char _ambient[4], 
	//	unsigned char _diffuse[4],
	//	unsigned char _specular[4])
	//{
	//	return m_interface->setMaterialColor(_ambient,_diffuse,_specular);
	//}
	//////////////////////////////////////////////////////////////////////////
	//VertexData * RenderEngine::createVertexData()
	//{
	//	return m_interface->createVertexData();
	//}
	//////////////////////////////////////////////////////////////////////////
	//void RenderEngine::drawPrimitive(PrimitiveData * _pd)
	//{
	//	return m_interface->drawPrimitive(_pd);
	//}
	//////////////////////////////////////////////////////////////////////////
	//IndexData * RenderEngine::createIndexData()
	//{
	//	return m_interface->createIndexData();
	//}
	//////////////////////////////////////////////////////////////////////////
	//VertexDeclaration * RenderEngine::createVertexDeclaration()
	//{
	//	return m_interface->createVertexDeclaration();
	//}
	//////////////////////////////////////////////////////////////////////////
	//void	RenderEngine::setTexture(RenderImageInterface * _tex)
	//{
	//	return m_interface->setTexture(_tex);
	//}
	//////////////////////////////////////////////////////////////////////////
	//bool RenderEngine::beginScene(int _color)
	//{
	//	return m_interface->beginScene(_color);
	//}
	//////////////////////////////////////////////////////////////////////////
	//bool RenderEngine::endScene()
	//{
	//	return m_interface->endScene();
	//}
	//////////////////////////////////////////////////////////////////////////
	//void RenderEngine::drawLine3D(const mt::vec3f& p1, const mt::vec3f& p2, unsigned long color)
	//{
	//	return m_interface->drawLine3D(p1,p2,color);
	//}
	//////////////////////////////////////////////////////////////////////////
	//void RenderEngine::drawLine2D(const mt::vec2f& p1, const mt::vec2f& p2, unsigned long color)
	//{
	//	return m_interface->drawLine2D(p1,p2,color);
	//}
	//void RenderEngine::drawBox( const mt::vec3f & MinEdge, const mt::vec3f & MaxEdge, unsigned long _color)
	//{
	//	return m_interface->drawBox(MinEdge,MaxEdge,_color);
	//}
	////////////////////////////////////////////////////////////////////////////
	//RenderFontInterface* RenderEngine::loadFont(const std::string &_fontXml)
	//{
	//	FontDesc desc;

	//	float scaleW = -1;
	//	float scaleH = -1;

	//	std::string name;
	//	std::string ch;
	//	std::string origin;
	//	std::string size;

	//	float aw;
	//	float x;
	//	float y;
	//	float width;
	//	float height;
	//	int id;

	//	FileDataInterface* imageData = 0;

	//	desc.color = 0xFFFFFFFF;

	//	XML_PARSE_FILE(_fontXml)
	//	{
	//		XML_VALUE_ATTRIBUTE("height",desc.height);

	//		XML_CHECK_NODE_FOR_EACH("font")
	//		{
	//			XML_CHECK_NODE_FOR_EACH("bitmaps")
	//			{
	//				XML_CHECK_NODE("bitmap")
	//				{
	//					XML_VALUE_ATTRIBUTE("name",name);
	//					XML_VALUE_ATTRIBUTE("size",size);

	//					assert(!name.empty());
	//					assert(!size.empty());

	//					imageData = 
	//						Holder<FileEngine>::hostage()->openFile(name);

	//					if(imageData == 0)
	//					{
	//						assert(!"no font image");
	//						return 0;
	//					}

	//					desc.texDesc.buffer = (void*)imageData->getBuffer();
	//					desc.texDesc.size = imageData->size();
	//					desc.texDesc.filter = 1;

	//					sscanf_s(size.c_str(), "%fx%f", &scaleW, &scaleH); 

	//					assert((scaleW > 0 && scaleW <= 1024));
	//					assert((scaleH > 0 && scaleH <= 1024));
	//				}
	//			}

	//			XML_CHECK_NODE_FOR_EACH("glyphs")
	//			{
	//				XML_CHECK_NODE("glyph")
	//				{
	//					XML_VALUE_ATTRIBUTE("ch",ch);
	//					XML_VALUE_ATTRIBUTE("origin",origin);
	//					XML_VALUE_ATTRIBUTE("size",size);
	//					XML_VALUE_ATTRIBUTE("aw",aw);

	//				    sscanf_s(origin.c_str(), "%f,%f", &x,&y); 
	//					sscanf_s(size.c_str(), "%fx%f", &width, &height); 

	//					id = int(ch[0]);
	//					
	//					desc.chars[id].uv.x = x / scaleW;
	//					desc.chars[id].uv.y = y / scaleH;
	//					desc.chars[id].uv.z = (x + width) / scaleW;
	//					desc.chars[id].uv.w = (y + height) / scaleH;
	//					desc.chars[id].width = aw;
	//				}
	//			}
	//		}
	//	}
	//	XML_INVALID_PARSE()
	//	{
	//		assert(!"Error in font loader!");
	//		return false;
	//	}
	//	
	//	RenderFontInterface* f = m_interface->loadFont(desc);

	//	Holder<FileEngine>::hostage()->closeFile(imageData);

	//	return f;
	//}
}
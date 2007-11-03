#	include "RenderEngine.h"

#	include "FileEngine.h"

#	include "XmlParser/XmlParser.h"

#	include "ErrorMessage.h"

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
			printf( "Error: Image can't open resource file '%s'\n", _fileName.c_str() );

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
			printf( "Error: Image from file '%s' not loader\n", _fileName.c_str() );

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
}
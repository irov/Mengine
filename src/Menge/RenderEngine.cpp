#	include "RenderEngine.h"

#	include "FileEngine.h"

#	include "XmlParser/XmlParser.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderEngine::RenderEngine( RenderSystemInterface * _interface )
		: m_interface( _interface )
		, m_renderViewport( mt::vec2f(0.f,0.f), mt::vec2f(1024.f,768.f) )
		, m_renderCamera(0)
	{
		Holder<RenderEngine>::keep( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::render()
	{
		m_interface->render();
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderViewport( const Viewport & _viewport )
	{
		m_renderViewport = _viewport;
	}
	////////////////////////////////////////////////////////////////////////////
	const Viewport & RenderEngine::getRenderViewport() const
	{
		return m_renderViewport;
	}
	////////////////////////////////////////////////////////////////////////////
	void RenderEngine::setRenderCamera( Camera3D * _camera )
	{
		m_renderCamera = _camera;
	}
	////////////////////////////////////////////////////////////////////////////
	Camera3D * RenderEngine::getRenderCamera()
	{
		return m_renderCamera;
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
			MENGE_LOG( "Error: Image can't open resource file '%s'\n", _fileName.c_str() );

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
			MENGE_LOG( "Error: Image from file '%s' not loader\n", _fileName.c_str() );

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
		mt::mat3f transform = _transform;
		transform.v2.v2 -= m_renderViewport.begin;
		m_interface->renderImage(
			transform.m,
			_offset.m,
			_uv.m,
			_size.m,
			_mixedColor,
			_image);
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::renderImage(		
			const mt::mat3f & _transform, 
			const mt::vec2f & _a,
			const mt::vec2f & _b,
			const mt::vec2f & _c,
			const mt::vec2f & _d,
			const mt::vec4f & _uv,
			unsigned int _color, 
			const RenderImageInterface* _image)
	{
		mt::mat3f transform = _transform;
		transform.v2.v2 -= m_renderViewport.begin;
		m_interface->renderImage(
			transform.m,
			_a.m,
			_b.m,
			_c.m,
			_d.m,
			_uv.m,
			_color,
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
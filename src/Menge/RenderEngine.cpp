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
	void RenderEngine::render( RenderImageInterface* _outImage, const char* _imageName, const int* rect )
	{
		m_interface->render( _outImage, _imageName, rect );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderEngine::setContentResolution( const mt::vec2f _resolution )
	{
		m_interface->setContentResolution( _resolution.m );
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
	RenderImageInterface * RenderEngine::loadImage( const std::string & _filename, std::vector<char> & _buff, size_t _filter )
	{
		FileDataInterface * file = Holder<FileEngine>::hostage()->openFile( _filename );

		if( file == 0 )
		{
			MENGE_LOG( "Error: Image can't open resource file '%s'\n", _filename.c_str() );

			return 0;
		}

		size_t buff_size = file->size();
		_buff.resize( buff_size );
		file->read( &_buff[0], buff_size );

		TextureDesc	textureDesc;
		textureDesc.buffer = &_buff[0];
		textureDesc.size = buff_size;
		textureDesc.name = _filename.c_str();
		textureDesc.filter = _filter;

		RenderImageInterface * image = loadImage( textureDesc );

		Holder<FileEngine>::hostage()->closeFile( file );

		if( image == 0 )
		{
			MENGE_LOG( "Error: Image from file '%s' not loader\n", _filename.c_str() );

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
		unsigned int _color, 
		const RenderImageInterface * _image)
	{
		mt::mat3f transform = _transform;
		transform.v2.v2 -= m_renderViewport.begin;
		m_interface->renderImage(
			transform.m,
			_offset.m,
			_uv.m,
			_size.m,
			_color,
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
}

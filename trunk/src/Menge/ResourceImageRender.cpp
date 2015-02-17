#	include "ResourceImageRender.h"

#	include "Kernel/ResourceImplement.h"

#	include "Interface/RenderSystemInterface.h"

#   include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageRender );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageRender::ResourceImageRender()
		: m_width(0)
		, m_height(0)
		, m_channels(0)
		, m_depth(0)
		, m_format(PF_UNKNOWN)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageRender::setWidth( uint32_t _width )
	{
		m_width = _width;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t ResourceImageRender::getWidth() const
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageRender::setHeight( uint32_t _height )
	{
		m_height = _height;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t ResourceImageRender::getHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageRender::setChannels( uint32_t _channels )
	{
		m_channels = _channels;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t ResourceImageRender::getChannels() const
	{
		return m_channels;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageRender::setDepth( uint32_t _depth )
	{
		m_depth = _depth;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t ResourceImageRender::getDepth() const
	{
		return m_depth;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageRender::setPixelFormat( PixelFormat _format )
	{
		m_format = _format;
	}
	//////////////////////////////////////////////////////////////////////////
	PixelFormat ResourceImageRender::getPixelFormat() const
	{
		return m_format;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageRender::_compile()
	{	
		RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE(m_serviceProvider)
			->createRenderTargetTexture( m_width, m_height, m_channels, m_depth, m_format );

		if( texture == nullptr )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageRender::bake( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, Node * _node, bool _clear )
	{
		const RenderTextureInterfacePtr & texture = this->getTexture();

		if( texture == nullptr )
		{
			return false;
		}

		const RenderImageInterfacePtr & image = texture->getImage();

		if( image == nullptr )
		{
			return false;
		}

		if( RENDER_SYSTEM(m_serviceProvider)
			->lockRenderTarget( image ) == false )
		{
			return false;
		}

		RENDER_SERVICE(m_serviceProvider)
			->beginScene();

		if( _clear == true )
		{
			RENDER_SERVICE(m_serviceProvider)
				->clear( 0x00000000 );
		}

		_node->render( _viewport, _camera, 0U );

		RENDER_SERVICE(m_serviceProvider)
			->endScene();

		if( RENDER_SYSTEM(m_serviceProvider)
			->unlockRenderTarget() == false )
		{
			return false;
		}

		return true;
	}
}

#include "DX9RenderTarget.h"

#include "DX9RenderEnum.h"
#include "DX9ErrorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderTarget::DX9RenderTarget()
		: m_width( 0 )
		, m_height( 0 )
		, m_device( nullptr )
        , m_hwWidth( 0 )
        , m_hwHeight( 0 )
		, m_renderTexture( nullptr )
		, m_surface( nullptr )
		, m_oldSurface( nullptr )
	{
	}
	//////////////////////////////////////////////////////////////////////////
    DX9RenderTarget::~DX9RenderTarget()
    {
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderTarget::initialize( LPDIRECT3DDEVICE9 _device, uint32_t _width, uint32_t _height, PixelFormat _format )
	{
		m_device = _device;

		m_width = _width;
		m_height = _height;
        m_format = _format;

        D3DFORMAT d3dformat = s_toD3DFormat( _format );

		LPDIRECT3DTEXTURE9 renderTexture;
		IF_DXCALL( m_device, CreateTexture, (m_width, m_height, 1, D3DUSAGE_RENDERTARGET, d3dformat, D3DPOOL_DEFAULT, &renderTexture, NULL) )
		{
			return false;
		}

		D3DSURFACE_DESC texDesc;
        IF_DXCALL( renderTexture, GetLevelDesc, (0, &texDesc) )
        {
            return false;
        }

        m_hwWidth = texDesc.Width;
        m_hwHeight = texDesc.Height;

        if( this->_initialize() == false )
        {
            return false;
        }

        m_renderTexture = renderTexture;

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderTarget::_initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	void DX9RenderTarget::finalize()
	{
		m_renderTexture->Release();
		m_renderTexture = nullptr;

        this->_finalize();
	}
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderTarget::_finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderTarget::getWidth() const
    {
        return m_width;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderTarget::getHeight() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    PixelFormat DX9RenderTarget::getFormat() const
    {
        return m_format;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderTarget::getHWWidth() const
    {
        return m_hwWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderTarget::getHWHeight() const
    {
        return m_hwHeight;
    }
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderTarget::begin()
	{
		LPDIRECT3DSURFACE9 surface;
		DXCALL( m_renderTexture, GetSurfaceLevel, ( 0, &surface ) );

		if( surface == nullptr )
		{
			return false;
		}

		LPDIRECT3DSURFACE9 old_surface;
		DXCALL( m_device, GetRenderTarget, ( 0, &old_surface ) );

		DXCALL( m_device, SetRenderTarget, ( 0, surface ) );

		m_oldSurface = old_surface;
		m_surface = surface;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderTarget::end()
	{
		DXCALL( m_device, SetRenderTarget, ( 0, m_oldSurface ) );

		if( m_oldSurface != nullptr )
		{
			m_oldSurface->Release();
			m_oldSurface = nullptr;
		}

		m_surface->Release();
		m_surface = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderTarget::getData( unsigned char * _buffer, size_t _pitch )
	{
        (void)_buffer;
        (void)_pitch;

        return false;
	}
    //////////////////////////////////////////////////////////////////////////
    LPDIRECT3DTEXTURE9 DX9RenderTarget::getDX9RenderTexture() const
    {
        return m_renderTexture;
    }    
}

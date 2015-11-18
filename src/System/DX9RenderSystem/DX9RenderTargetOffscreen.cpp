#	include "DX9RenderTargetOffscreen.h"

#	include "DX9RenderEnum.h"
#	include "DX9ErrorHelper.h"

namespace Menge
{
	DX9RenderTargetOffscreen::DX9RenderTargetOffscreen()
		: m_serviceProvider( nullptr )
		, m_width( 0 )
		, m_height( 0 )
		, m_device( nullptr )
		, m_renderTexture( nullptr )
		, m_surfacePlain( nullptr )
		, m_beginSurface( nullptr )
		, m_oldSurface( nullptr )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DX9RenderTargetOffscreen::~DX9RenderTargetOffscreen()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderTargetOffscreen::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderTargetOffscreen::initialize( LPDIRECT3DDEVICE9 _device, uint32_t _width, uint32_t _height, PixelFormat _format )
	{
		m_device = _device;

		m_width = _width;
		m_height = _height;

		m_format = s_toD3DFormat( _format );

		LPDIRECT3DTEXTURE9 renderTexture;
		IF_DXCALL( m_serviceProvider, m_device, CreateTexture, (m_width, m_height, 1, D3DUSAGE_RENDERTARGET, m_format, D3DPOOL_DEFAULT, &renderTexture, NULL) )
		{
			return false;
		}

		D3DSURFACE_DESC texDesc;
		IF_DXCALL( m_serviceProvider, renderTexture, GetLevelDesc, (0, &texDesc) )
		{
			return false;
		}

		LPDIRECT3DSURFACE9 surfacePlain;
		IF_DXCALL( m_serviceProvider, m_device, CreateOffscreenPlainSurface, (m_width, m_height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surfacePlain, NULL) )
		{
			return false;
		}

		m_renderTexture = renderTexture;
		m_surfacePlain = surfacePlain;
		m_desc = texDesc;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderTargetOffscreen::finalize()
	{
		m_renderTexture->Release();
		m_renderTexture = nullptr;

		m_surfacePlain->Release();
		m_surfacePlain = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderTargetOffscreen::begin()
	{
		LPDIRECT3DSURFACE9 surface;
		DXCALL( m_serviceProvider, m_renderTexture, GetSurfaceLevel, ( 0, &surface ) );

		if( surface == nullptr )
		{
			return false;
		}

		LPDIRECT3DSURFACE9 old_surface;
		DXCALL( m_serviceProvider, m_device, GetRenderTarget, ( 0, &old_surface ) );

		DXCALL( m_serviceProvider, m_device, SetRenderTarget, ( 0, surface ) );
		
		m_oldSurface = old_surface;
		m_beginSurface = surface;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderTargetOffscreen::end()
	{
		DXCALL( m_serviceProvider, m_device, SetRenderTarget, ( 0, m_oldSurface ) );

		if( m_oldSurface != nullptr )
		{
			m_oldSurface->Release();
			m_oldSurface = nullptr;
		}

		m_beginSurface->Release();
		m_beginSurface = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderTargetOffscreen::getData( unsigned char * _buffer, size_t _pitch )
	{
		if( m_surfacePlain == nullptr )
		{
			return false;
		}

		if( m_beginSurface == nullptr )
		{
			return false;
		}

		DXCALL( m_serviceProvider, m_device, GetRenderTargetData, (m_beginSurface, m_surfacePlain) );

		D3DLOCKED_RECT LockedRect;
		DXCALL( m_serviceProvider, m_surfacePlain, LockRect, ( &LockedRect, NULL, 0 ) );

		if( LockedRect.pBits == NULL || LockedRect.Pitch == 0 )
		{
			return false;
		}

		unsigned char * dstData = _buffer;
		unsigned char * srcData = (unsigned char *)LockedRect.pBits;

		for( DWORD i = 0; i != m_height; ++i )
		{
			stdex::memorycopy( dstData, 0, srcData, m_width * 4 );

			dstData += _pitch;
			srcData += LockedRect.Pitch;
		}

		DXCALL( m_serviceProvider, m_surfacePlain, UnlockRect, () );

		return true;
	}
}

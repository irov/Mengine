#	include "Render.h"

#	include <d3dx9tex.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
#	define DXCALL
	//////////////////////////////////////////////////////////////////////////
	static const DWORD FVF_PARTICLE = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX8;
	//////////////////////////////////////////////////////////////////////////
	static const UINT MAX_ELEMENT_COUNT = 262144;
	//////////////////////////////////////////////////////////////////////////
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT result = ::DefWindowProc( hWnd, uMsg, wParam, lParam );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	static const WCHAR * ClassName = L"AE_Astralax_Class";
	//////////////////////////////////////////////////////////////////////////
	Render::Render()
		: m_hwnd(NULL)
		, m_hd3d9(NULL)
		, m_pD3D(NULL)
		, m_device(NULL)
		, m_vertexBuffer(NULL)
		, m_indexBuffer(NULL)
		, m_renderTexture(NULL)
		, m_surfacePlain(NULL)
		, m_surface(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Render::~Render()
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	bool Render::initialize()
	{
		WNDCLASS wc;

		wc.style = NULL;
		wc.lpfnWndProc = &WndProc;
		wc.cbClsExtra = NULL;
		wc.cbWndExtra = NULL;
		wc.hInstance = NULL;
		wc.hIcon = NULL;
		wc.hCursor = NULL;
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;		
		wc.lpszClassName = ClassName;

		RegisterClass(&wc);

		m_hwnd = CreateWindowEx( WS_EX_TOOLWINDOW, ClassName, NULL, WS_POPUP, 0, 0, 0, 0, NULL, NULL, NULL, NULL );

		if( m_hwnd == NULL )
		{
			return false;
		}

		m_hd3d9 = ::LoadLibraryA( "d3d9.dll" );

		if( m_hd3d9 == NULL )
		{
			return false;
		}

		typedef IDirect3D9* (WINAPI *PDIRECT3DCREATE9)(UINT);
		PDIRECT3DCREATE9 pDirect3DCreate9 = (PDIRECT3DCREATE9)::GetProcAddress( m_hd3d9, "Direct3DCreate9" );

		if( pDirect3DCreate9 == NULL )
		{
			return false;
		}

		m_pD3D = pDirect3DCreate9( D3D_SDK_VERSION ); // D3D_SDK_VERSION

		D3DDISPLAYMODE Display;
		if( m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &Display) != D3D_OK )
		{
			return false;
		}

		ZeroMemory( &m_d3d9pp, sizeof(m_d3d9pp) );
		m_d3d9pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_d3d9pp.BackBufferFormat = Display.Format;

		m_d3d9pp.BackBufferCount = 1;
		m_d3d9pp.BackBufferWidth = 2048;
		m_d3d9pp.BackBufferHeight = 2048;

		m_d3d9pp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

		m_d3d9pp.Windowed = TRUE;

		//d3d9pp.FullScreen_RefreshRateInHz = Display.RefreshRate;

		//d3d9pp.MultiSampleType = D3DMULTISAMPLE_NONE;
		//d3d9pp.MultiSampleQuality = 0;
		//d3d9pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

		m_d3d9pp.hDeviceWindow = m_hwnd;

		if( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&m_d3d9pp, &m_device ) != D3D_OK )
		{
			Sleep(300);

			if( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd,
				D3DCREATE_HARDWARE_VERTEXPROCESSING,
				&m_d3d9pp, &m_device ) != D3D_OK )
			{

				Sleep(300);

				if( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING,
					&m_d3d9pp, &m_device ) != D3D_OK )
				{
					return false;
				}
			}
		}		
		
		UINT buffer_vertex_sizeof = MAX_ELEMENT_COUNT * 4 * sizeof( Vertex );
		DXCALL m_device->CreateVertexBuffer( buffer_vertex_sizeof, D3DUSAGE_WRITEONLY, FVF_PARTICLE, D3DPOOL_MANAGED, &m_vertexBuffer, NULL );

		if( m_vertexBuffer == nullptr )
		{
			return false;
		}

		UINT buffer_index_size = MAX_ELEMENT_COUNT * 6 * sizeof( UINT );
		DXCALL m_device->CreateIndexBuffer( buffer_index_size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_indexBuffer, NULL );

		if( m_indexBuffer == nullptr )
		{
			return false;
		}
		
		if( this->initResource_() == false )
		{
			return false;
		}

		this->initStates_();
	
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Render::finalize()
	{
		this->releaseResource_();

		if( m_vertexBuffer != nullptr )
		{
			m_vertexBuffer->Release();
			m_vertexBuffer = nullptr;
		}

		if( m_indexBuffer != nullptr )
		{
			m_indexBuffer->Release();
			m_indexBuffer = nullptr;
		}

		if( m_device != nullptr ) 
		{ 
			m_device->Release(); 
			m_device = nullptr; 
		}

		if( m_pD3D != nullptr ) 
		{ 
			m_pD3D->Release();
			m_pD3D = nullptr; 
		}

		if( m_hd3d9 != NULL )
		{
			FreeLibrary( m_hd3d9 );
			m_hd3d9 = NULL;
		}

		DestroyWindow( m_hwnd );
		m_hwnd = NULL;

		UnregisterClass( ClassName, NULL );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Render::initResource_()
	{
		DXCALL m_device->CreateTexture( 2048, 2048, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_renderTexture, NULL );

		if( m_renderTexture == nullptr )
		{
			return false;
		}

		DXCALL m_device->CreateOffscreenPlainSurface( 2048, 2048, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &m_surfacePlain, NULL);

		if( m_surfacePlain == nullptr )
		{
			return false;
		}

		DXCALL m_renderTexture->GetSurfaceLevel( 0, &m_surface );

		if( m_surface == nullptr )
		{
			return false;
		}

		DXCALL m_device->SetRenderTarget( 0, m_surface );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Render::initStates_()
	{
		DXCALL m_device->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
		DXCALL m_device->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

		DXCALL m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		DXCALL m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		DXCALL m_device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

		DXCALL m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		DXCALL m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		DXCALL m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		DXCALL m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		DXCALL m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		DXCALL m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		DXCALL m_device->SetRenderState(D3DRS_LIGHTING, FALSE);
		DXCALL m_device->SetRenderState(D3DRS_ZENABLE, FALSE);

		DXCALL m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		
		DXCALL m_device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
		DXCALL m_device->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
		DXCALL m_device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ZERO);
		DXCALL m_device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
		DXCALL m_device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE); 
		DXCALL m_device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
	}
	//////////////////////////////////////////////////////////////////////////
	void Render::releaseResource_()
	{
		if( m_renderTexture != nullptr )
		{
			m_renderTexture->Release();
			m_renderTexture = nullptr;
		}

		if( m_surfacePlain != nullptr )
		{
			m_surfacePlain->Release();
			m_surfacePlain = nullptr;
		}

		if( m_surface != nullptr )
		{
			m_surface->Release();
			m_surface = nullptr;
		}

		DXCALL m_device->SetStreamSource( 0, nullptr, 0, 0 );
		DXCALL m_device->SetIndices( nullptr );

		DXCALL m_device->SetTexture( 0, nullptr );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Render::resetDevice_()
	{		
		this->releaseResource_();

		HRESULT hr = m_device->Reset( &m_d3d9pp );

		if (hr == D3DERR_DEVICELOST)
		{
			return false;
		}
		else if( FAILED( hr ) == true )
		{
			return false;
		}

		if( this->initResource_() == false )
		{
			return false;
		}

		this->initStates_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Render::test()
	{
		HRESULT hr = m_device->TestCooperativeLevel();

		if( hr == D3DERR_DEVICENOTRESET )
		{
			if( this->resetDevice_() == false )
			{
				::Sleep( 200 );

				return false;
			}
		}
		else if( FAILED( hr ) )
		{
			//_logger.message( "Render::beginScene invalid TestCooperativeLevel %d"
			//	, hr
			//	);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Render::beginScene()
	{	
		DXCALL m_device->BeginScene();
		
		DXCALL m_device->Clear( 0, NULL, D3DCLEAR_TARGET, 0x0, 0.0f, 0 );

		DXCALL m_device->SetFVF( FVF_PARTICLE );

		DXCALL m_device->SetStreamSource( 0, m_vertexBuffer, 0, sizeof( Vertex ) );
		DXCALL m_device->SetIndices( m_indexBuffer );

		DXCALL m_device->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Render::endScene()
	{
		DXCALL m_device->EndScene();

		DXCALL m_device->Present( NULL, NULL, NULL, NULL );
	}
	//////////////////////////////////////////////////////////////////////////
	void Render::setViewport( DWORD _vw, DWORD _vh )
	{
		D3DVIEWPORT9 VP;
		VP.X = 0;
		VP.Y = 0;
		VP.Width = _vw;
		VP.Height = _vh;
		VP.MinZ = 0.f;
		VP.MaxZ = 1.f;

		DXCALL m_device->SetViewport( &VP );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Render::lockVertexBuffer( UINT _count, Vertex ** _vertices )
	{
		UINT vertex_size = _count * sizeof( Vertex );

		Vertex * vertices;
		DXCALL m_vertexBuffer->Lock( 0, vertex_size, (void**)&vertices, D3DLOCK_DISCARD );

		if( vertices == NULL )
		{
			return false;
		}

		*_vertices = vertices;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Render::unlockVertexBuffer()
	{ 
		DXCALL m_vertexBuffer->Unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Render::lockIndicesBuffer( UINT _count, Indices ** _indices )
	{
		UINT index_size = _count * sizeof( Indices );

		Indices * indices = nullptr;
		DXCALL m_indexBuffer->Lock( 0, index_size, (void**)&indices, D3DLOCK_DISCARD );

		if( indices == NULL )
		{
			return false;
		}

		*_indices = indices;

		return true;				
	}
	//////////////////////////////////////////////////////////////////////////
	void Render::unlockIndicesBuffer()
	{ 
		DXCALL m_indexBuffer->Unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	void Render::drawIndexedPrimitive( INT _baseVertexIndex, UINT _numVertices, UINT _primCount )
	{
		DXCALL m_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, _numVertices, _baseVertexIndex, _primCount );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Render::getRenderData( unsigned char * _buffer, size_t _pitch, DWORD _width, DWORD _height )
	{	
		if( m_surface == NULL )
		{
			//_logger.message("Render::getRenderData m_surface == NULL"
			//	);

			return false;
		}

		m_device->GetRenderTargetData( m_surface, m_surfacePlain );

		if( m_surfacePlain == NULL )
		{
			//_logger.message("Render::getRenderData GetRenderTargetData m_surfacePlain == NULL"
			//	);

			return false;
		}
		
		D3DLOCKED_RECT LockedRect;
		DXCALL m_surfacePlain->LockRect( &LockedRect, NULL, 0 );

		if( LockedRect.pBits == NULL || LockedRect.Pitch == 0 )
		{
			//_logger.message("Render::getRenderData LockRect LockedRect.pBits == NULL || LockedRect.Pitch == 0"
			//	);

			return false;
		}

		unsigned char * dstData = _buffer;
		unsigned char * srcData = (unsigned char *)LockedRect.pBits;

		for( DWORD i = 0; i != _height; ++i )
		{
			unsigned char * dstDataRGBA = dstData;
			unsigned char * srcDataRGBA = srcData;
			for( DWORD j = 0; j != _width; ++j )
			{
				dstDataRGBA[0] = srcDataRGBA[3];
				dstDataRGBA[1] = srcDataRGBA[2];
				dstDataRGBA[2] = srcDataRGBA[1];
				dstDataRGBA[3] = srcDataRGBA[0];
				
				dstDataRGBA += 4;
				srcDataRGBA += 4;
			}

			dstData += _pitch;
			srcData += LockedRect.Pitch;
		}

		DXCALL m_surfacePlain->UnlockRect();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Render::getRegValue( const std::wstring & _path, const std::wstring & _key, std::wstring & _value ) const
	{
		HKEY hKey;
		LONG lRes = RegOpenKeyEx( HKEY_CURRENT_USER, _path.c_str(), 0, KEY_READ, &hKey );  // Check Python x32
		
		if( lRes == ERROR_FILE_NOT_FOUND )
		{
			lRes = RegOpenKeyEx( HKEY_CURRENT_USER, _path.c_str(), 0, KEY_READ | KEY_WOW64_64KEY, &hKey );  // Check Python x64
		}

		if( lRes != ERROR_SUCCESS )
		{
			//_logger.message("getRegValue %ls RegOpenKeyEx get Error %d"
			//	, _path.c_str()
			//	, lRes
			//	);

			return false;
		}

		WCHAR value[8196];

		DWORD dwBufferSize = sizeof( value );
		LONG nError = ::RegQueryValueEx( hKey, _key.c_str(), 0, NULL, (LPBYTE)value, &dwBufferSize );

		RegCloseKey( hKey );
		
		if( nError != ERROR_SUCCESS )
		{
			//_logger.message("getRegValue %ls RegQueryValueEx get Error %d"
			//	, _path.c_str()
			//	, nError
			//	);

			return false;
		}

		_value.assign( value );

		return true;
	}
}
#	include "d3d9RenderSystem.h"
#	include "d3d9VertexData.h"
#	include "d3d9IndexData.h"
#	include "d3d9Texture.h"
#	include "d3d9VertexDeclaration.h"
#	include "d3d9RenderFont.h"

#	include <windows.h>
#	include <assert.h>

#	pragma comment (lib, "d3d9.lib")
#	pragma comment (lib, "d3dx9.lib")

bool initInterfaceSystem(RenderSystemInterface**	_ptrRenderSystem)
{
	try
	{
		*_ptrRenderSystem = new Direct3d9RenderSystem();
	}
	catch (...)
	{
		return false;
	}

	return true;
}

void releaseInterfaceSystem(RenderSystemInterface* _ptrRenderSystem)
{
	delete static_cast<Direct3d9RenderSystem*>(_ptrRenderSystem);
}

void	Direct3d9RenderSystem::setProjectionMatrix(const mt::mat4f& _projection)
{
	HRESULT hr;
	hr = pID3DDevice->SetTransform(D3DTS_PROJECTION,LPD3DXMATRIX(_projection.m));
	if(hr != S_OK)
	{
		assert(!"can't set projection matrix");
	}
}

void	Direct3d9RenderSystem::setViewMatrix(const mt::mat4f& _view)
{

	D3DXMATRIX out;
	D3DXVECTOR3 eye(1,0,1);
	D3DXVECTOR3 at(0,0,0);
	D3DXVECTOR3 up(0,0,1);

	D3DXMatrixLookAtLH(&out, &eye, &at, &up);


	HRESULT hr;
	hr = pID3DDevice->SetTransform(D3DTS_VIEW,LPD3DXMATRIX(_view.m));
	if(hr != S_OK)
	{
		assert(!"can't set view matrix");
	}
}

void	Direct3d9RenderSystem::setWorldMatrix(const mt::mat4f& _world)
{
	HRESULT hr;
	hr = pID3DDevice->SetTransform(D3DTS_WORLD,LPD3DXMATRIX(_world.m));
	if(hr != S_OK)
	{
		assert(!"can't set world matrix");
	}
}

Direct3d9RenderSystem::Direct3d9RenderSystem()
	: pID3DDevice(0)
{}

Direct3d9RenderSystem::~Direct3d9RenderSystem()
{
	if(m_batchFontVB)
	{
		m_batchFontVB->Release(); 
		m_batchFontVB = NULL;
	}

	if(m_batchFontIB)
	{
		m_batchFontIB->Release(); 
		m_batchFontIB = NULL;
	}
}

Texture * Direct3d9RenderSystem::createTextureInMemory(const TextureDesc& _desc)
{
	LPDIRECT3DTEXTURE9	m_texPointer;

	bool haveAlpha = _desc.filter;

	if (FAILED(D3DXCreateTextureFromFileInMemoryEx(pID3DDevice, _desc.buffer, (UINT)_desc.size,
                                    D3DX_DEFAULT, D3DX_DEFAULT,
                                    1,                  
                                    0,                  
									haveAlpha ? D3DFMT_A8R8G8B8 : D3DFMT_R8G8B8,    
                                    D3DPOOL_MANAGED,    
                                    D3DX_FILTER_NONE,   
                                    D3DX_DEFAULT,       
                                    0,                  
                                    NULL, NULL,
                                    &m_texPointer)))
	{
		assert(!"can't get TEXTURE desc!");
    }

	return new d3d9Texture(m_texPointer);
}

void	Direct3d9RenderSystem::setTexture(Texture * _tex)
{
	d3d9Texture* _d3d9tex = static_cast<d3d9Texture*>(_tex);

	if(_d3d9tex == NULL)
	{
		pID3DDevice->SetTexture(0,NULL);
	}
	else
	{
		pID3DDevice->SetTexture(0,_d3d9tex->_get());
	}
}

void	Direct3d9RenderSystem::releaseTexture(Texture * _tex)
{
	delete static_cast<d3d9Texture*>(_tex);
}

bool	Direct3d9RenderSystem::createDisplay(
	HWND _hWnd, int _width, int _height, int _bits, bool _fullScreen,
	bool _vsync, bool _stencilBuffer, bool _antiAlias, bool _pureSoftware)
{
	HRESULT hr;

	DeviceLost = false;

	IDirect3D9* pID3D = Direct3DCreate9(D3D_SDK_VERSION);
	
	D3DADAPTER_IDENTIFIER9 dai;
	if (!FAILED(pID3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &dai)))
	{
		int Product = HIWORD(dai.DriverVersion.HighPart);
		int Version = LOWORD(dai.DriverVersion.HighPart);
		int SubVersion = HIWORD(dai.DriverVersion.LowPart);
		int Build = LOWORD(dai.DriverVersion.LowPart);

		printf("%s %s %d.%d.%d.%d", dai.Description, dai.Driver, Product, Version,
			SubVersion, Build);
	}

	D3DDISPLAYMODE d3ddm;
	hr = pID3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if (FAILED(hr))
	{
		printf("Error: Could not get Adapter Display mode.");
		return false;
	}

	D3DPRESENT_PARAMETERS present;

	ZeroMemory(&present, sizeof(present));

	present.SwapEffect					= _fullScreen ? D3DSWAPEFFECT_FLIP : D3DSWAPEFFECT_COPY;
	present.Windowed					= _fullScreen ? FALSE : TRUE;
	present.BackBufferFormat			= d3ddm.Format;
	present.EnableAutoDepthStencil		= TRUE;
	present.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;

	if (_fullScreen)
	{
		present.BackBufferWidth = _width;
		present.BackBufferHeight = _height;
		present.BackBufferFormat = D3DFMT_R5G6B5;
		present.BackBufferCount = 0;
		present.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

		if (_vsync)
			present.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		
		// request 32bit mode if user specified 32 bit, added by Thomas Stüfe
		if (_bits == 32 && !_stencilBuffer) 
			present.BackBufferFormat = D3DFMT_A8R8G8B8;
	}

	D3DDEVTYPE devtype = D3DDEVTYPE_HAL;
	
	// enable anti alias if possible and whished
	if (_antiAlias)
	{
		DWORD qualityLevels = 0; 

		if (!FAILED(pID3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, 
		           devtype, present.BackBufferFormat, !_fullScreen, 
		           D3DMULTISAMPLE_2_SAMPLES, &qualityLevels)))
        {        
			// enable multi sampling
            present.SwapEffect         = D3DSWAPEFFECT_DISCARD; 
            present.MultiSampleType    = D3DMULTISAMPLE_2_SAMPLES; 
			present.MultiSampleQuality = qualityLevels-1;
        } 
		else
		if (!FAILED(pID3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, 
					devtype, present.BackBufferFormat, !_fullScreen, 
					D3DMULTISAMPLE_NONMASKABLE, &qualityLevels))) 
		{        
			// enable non maskable multi sampling
			present.SwapEffect         = D3DSWAPEFFECT_DISCARD; 
			present.MultiSampleType    = D3DMULTISAMPLE_NONMASKABLE; 
			present.MultiSampleQuality = qualityLevels-1;
		} 		
		else 
        { 
            printf("Anti aliasing disabled because hardware/driver lacks necessary caps.");    
			_antiAlias = false;
        }
	}

	if (_stencilBuffer)
	{
		present.AutoDepthStencilFormat = D3DFMT_D24S8;
		if(FAILED(pID3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, devtype,
			present.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL, 
			D3DRTYPE_SURFACE, D3DFMT_D24S8)))
		{
			printf("Device does not support stencilbuffer, disabling stencil buffer.");
			_stencilBuffer = false;
		}
		else
		if(FAILED(pID3D->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, devtype,
			present.BackBufferFormat, present.BackBufferFormat, D3DFMT_D24S8)))
		{
			printf("Depth-stencil format is not compatible with display format, disabling stencil buffer.");
			_stencilBuffer = false;
		} 		
	}

	if (!_stencilBuffer)
	{
		present.AutoDepthStencilFormat = D3DFMT_D16;
	}

	if (_pureSoftware)
	{
		hr = pID3D->CreateDevice(	D3DADAPTER_DEFAULT, D3DDEVTYPE_REF,	_hWnd,
									D3DCREATE_SOFTWARE_VERTEXPROCESSING, &present, &pID3DDevice);

		if (FAILED(hr))
			printf("Was not able to create Direct3D9 software device.");
	}
	else
	{
		hr = pID3D->CreateDevice(	D3DADAPTER_DEFAULT, devtype,	_hWnd,
									D3DCREATE_HARDWARE_VERTEXPROCESSING, &present, &pID3DDevice);

		if(FAILED(hr))
		{
			hr = pID3D->CreateDevice(	D3DADAPTER_DEFAULT, devtype,	_hWnd,
										D3DCREATE_MIXED_VERTEXPROCESSING , &present, &pID3DDevice);

			if(FAILED(hr))
			{
				hr = pID3D->CreateDevice(	D3DADAPTER_DEFAULT, devtype, _hWnd,
											D3DCREATE_SOFTWARE_VERTEXPROCESSING, &present, &pID3DDevice);

				if (FAILED(hr))
					printf("Was not able to create Direct3D9 device.");
			}
		}
	}


	if (!pID3DDevice)
	{
		printf("Was not able to create DIRECT3D9 device.");
		return false;
	}

	D3DCAPS9 Caps;

	// get caps
	pID3DDevice->GetDeviceCaps(&Caps);

	// disalbe stencilbuffer if necessary
	if (_stencilBuffer && 
		(!(Caps.StencilCaps & D3DSTENCILCAPS_DECRSAT) ||
		!(Caps.StencilCaps & D3DSTENCILCAPS_INCRSAT) ||
		!(Caps.StencilCaps & D3DSTENCILCAPS_KEEP)))
	{
		printf("Device not able to use stencil buffer, disabling stencil buffer.");
		_stencilBuffer = false;
	}

	// set default vertex shader
	//setVertexShader(EVT_STANDARD);

	// enable antialiasing
	if (_antiAlias)
		pID3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);

	// set fog mode
	//setFog(FogColor, LinearFog, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

	// set exposed data
	//ExposedData.D3D9.D3D9 = pID3D;
	//ExposedData.D3D9.D3DDev9 = pID3DDevice;
	//ExposedData.D3D9.HWnd =  reinterpret_cast<s32>(hwnd);

	//ResetRenderStates = true;

	// create materials
//	createMaterialRenderers();
//
	// set the renderstates
//	setRenderStates3DMode();

	// set maximal anisotropy
	pID3DDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, min(16, Caps.MaxAnisotropy));
	pID3DDevice->SetSamplerState(1, D3DSAMP_MAXANISOTROPY, min(16, Caps.MaxAnisotropy));

	/*DXUTSetCallbackDeviceCreated( OnCreateDevice, this );
	DXUTSetCallbackDeviceReset( OnResetDevice, this );
	DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackKeyboard( NULL );
	DXUTSetCallbackFrameRender( OnFrameRender, this );
	DXUTSetCallbackFrameMove( NULL );
	DXUTSetCursorSettings( true, true );
	DXUTInit( true, true, true ); 

	DXUTCreateWindow( L"Render System test" );
	DXUTCreateDevice( D3DADAPTER_DEFAULT, !_fullScreen, _width, _height, IsDeviceAcceptable, ModifyDeviceSettings );

	*/
	_initBatching();
	_initRenderSystemParams();

	return true;
}

bool Direct3d9RenderSystem::beginScene(bool backBuffer, bool zBuffer, int color)
{
	HRESULT hr;

	if (!pID3DDevice)
		return false;

	if (DeviceLost)
	{
		if(FAILED(hr = pID3DDevice->TestCooperativeLevel()))
        {
            if (hr == D3DERR_DEVICELOST)
                return false;

            if (hr == D3DERR_DEVICENOTRESET)
			{
			//	reset();
				_initRenderSystemParams();
			}
            return false;
        }
	}

	DWORD flags = 0;

	if (backBuffer)
		flags |= D3DCLEAR_TARGET;

	if (zBuffer)
		flags |= D3DCLEAR_ZBUFFER;

	//if (StencilBuffer)
	//	flags |= D3DCLEAR_STENCIL;

	hr = pID3DDevice->Clear( 0, NULL, flags, color, 1.0, 0);
	if (FAILED(hr))
		printf("DIRECT3D9 clear failed.");

	hr = pID3DDevice->BeginScene();
	if (FAILED(hr))
	{
		printf("DIRECT3D9 begin scene failed.");
		return false;
	}

	return true;
}

bool Direct3d9RenderSystem::endScene()
{
	if (DeviceLost)
		return false;

	HRESULT hr = pID3DDevice->EndScene();
	if (FAILED(hr))
	{
		printf("DIRECT3D9 end scene failed.");
		return false;
	}

	hr = pID3DDevice->Present(NULL, NULL, NULL, NULL);

	if (hr == D3DERR_DEVICELOST)
	{
		DeviceLost = true;
		printf("DIRECT3D9 device lost.");
	}
	else
	if (FAILED(hr) && hr != D3DERR_INVALIDCALL)
	{
		printf("DIRECT3D9 present failed.");
		return false;
	}

	return true;
}

RenderFontInterface * Direct3d9RenderSystem::loadFont(const FontDesc &_fontDesc)
{
	return new D3D9Font(this, pID3DDevice, _fontDesc);
}

void	Direct3d9RenderSystem::releaseRenderFont(RenderFontInterface* _fnt)
{
	delete static_cast<D3D9Font*>(_fnt);
}

void	Direct3d9RenderSystem::releaseRenderImage(RenderImageInterface* _rmi)
{
	delete static_cast<D3D9RenderImage*>(_rmi);
}
void	Direct3d9RenderSystem::renderText(mt::vec2f _pos, 
										  RenderFontInterface* _font,
										  const std::string& _text)

{
	D3D9Font * d3d9font = static_cast<D3D9Font*>(_font);

	RenderImageInterface* _rmi = d3d9font->getRenderImage();

	_prepareBatch(_rmi,-1);

	mt::vec2f& anPos = _pos;
	size_t textSize = _text.size();

	for(std::string::const_iterator i = _text.begin(), e = _text.end(); i != e; ++i)
	{		
		FontCharDesc	Char = d3d9font->_getChar(*i);

		if(*i == ' ')
		{
			anPos.x+=Char.width;
			continue;
		}

		D3D9Vertex	m_fontVerts[4];

		m_fontVerts[0].tcoor = mt::vec2f(Char.uv.x, Char.uv.y);
		m_fontVerts[1].tcoor = mt::vec2f(Char.uv.z, Char.uv.y);
		m_fontVerts[2].tcoor = mt::vec2f(Char.uv.z, Char.uv.w);
		m_fontVerts[3].tcoor = mt::vec2f(Char.uv.x, Char.uv.w);

		float	offX = anPos.x + Char.width;
		float	offY = anPos.y + _font->getHeight();

		m_fontVerts[0].position = mt::vec3f(anPos.x, anPos.y, 0);
		m_fontVerts[1].position = mt::vec3f(offX, anPos.y, 0);
		m_fontVerts[2].position = mt::vec3f(offX, offY, 0);
		m_fontVerts[3].position = mt::vec3f(anPos.x, offY, 0);

		m_fontVerts[0].color = 
		m_fontVerts[1].color = 
		m_fontVerts[2].color = 
		m_fontVerts[3].color = 0xFFFFFFFF;

		m_fontVerts[0].rhw = 
		m_fontVerts[1].rhw = 
		m_fontVerts[2].rhw = 
		m_fontVerts[3].rhw = 1.0f;

		anPos.x += Char.width;

		m_vertices.insert(m_vertices.end(), m_fontVerts, m_fontVerts + 4);
	}

	m_batches.back().end = m_vertices.size();
}

void	Direct3d9RenderSystem::drawPrimitive(PrimitiveData * _pd)
{
	_renderBatches();

	HRESULT hr;

	Direct3d9VertexData * vd = static_cast<Direct3d9VertexData*>(_pd->getVertexData());

	Direct3d9IndexData * id = static_cast<Direct3d9IndexData*>(_pd->getIndexData());
			
	pID3DDevice->SetVertexDeclaration( vd->_getVertexDecl() );
	pID3DDevice->SetStreamSource(0, vd->_get(), 0, vd->getVertexSize());
	pID3DDevice->SetIndices(id->_get());

	int primCount = 0;

	D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST;

	if(_pd->getRenderOp() == TRIANGLESTRIP)
	{
		primitiveType = D3DPT_TRIANGLESTRIP;
	}

	if(id != NULL)
	{
		primCount = id->getIndexCount();
		int vertexCount = vd->getVertexCount();
		size_t vertexStart = vd->getVertexStart();
		size_t indexStart = id->getIndexStart();

		pID3DDevice->DrawIndexedPrimitive( primitiveType, 
			vertexStart, 0, vertexCount, indexStart, primCount );
	}
	else
	{
		primCount = vd->getVertexCount()/3;

		pID3DDevice->DrawPrimitive( primitiveType,
			vd->getVertexCount(), primCount );
	} 
}

void	Direct3d9RenderSystem::_setDevice(IDirect3DDevice9 * _pd3dDevice)
{
	pID3DDevice = _pd3dDevice;
}

IDirect3DDevice9 * Direct3d9RenderSystem::_getDevice()
{
	return pID3DDevice;
}

VertexData * Direct3d9RenderSystem::createVertexData()
{
	VertexData * m_vertexData = new Direct3d9VertexData(pID3DDevice);
	return m_vertexData;
}

IndexData * Direct3d9RenderSystem::createIndexData()
{
	Direct3d9IndexData * m_indexData = new Direct3d9IndexData(pID3DDevice);
	return m_indexData;
}

VertexDeclaration * Direct3d9RenderSystem::createVertexDeclaration()
{
	d3d9VertexDeclaration * m_vertDecl = new d3d9VertexDeclaration(pID3DDevice);
	return m_vertDecl;
}

RenderImageInterface* Direct3d9RenderSystem::loadImage(const TextureDesc& _desc)
{
    return new D3D9RenderImage(pID3DDevice, _desc);
}

void	Direct3d9RenderSystem::_prepareBatch(RenderImageInterface* _rmi, int _blend)
{
	if( m_batches.empty() )
	{
		_newBatch(0,_rmi,_blend);
	}

	Batch & current_b = m_batches.back();

	if( current_b.image != _rmi || current_b.blend != _blend)
	{
		_newBatch(current_b.end,_rmi,_blend);
	}
}

void	Direct3d9RenderSystem::_newBatch(size_t _begin, RenderImageInterface* _rmi, int _blend)
{
	Batch b;
	b.begin = _begin;
	b.image = _rmi;
	b.blend = _blend;
	b.end = 0;
	m_batches.push_back( b );
}

void Direct3d9RenderSystem::_renderBatches()
{
	if( m_vertices.empty() )
	{
		return;
	}

	pID3DDevice->SetVertexShader(NULL);
	pID3DDevice->SetFVF (D3DFVF_TLVERTEX);
	pID3DDevice->SetIndices(m_batchFontIB);
	
	static D3D9Vertex*	batchVertices = NULL;

	pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pID3DDevice->SetStreamSource(0, m_batchFontVB, 0, m_size1Vert);

	m_batchFontVB->Lock(0, (UINT)m_vertices.size() * m_size1Vert, (VOID**)&batchVertices, 0);
	memcpy(batchVertices, &m_vertices[0], (UINT)m_vertices.size() * m_size1Vert);
	m_batchFontVB->Unlock();

	UINT startIndex = 0;

	for each( const Batch & b in m_batches )
	{
		D3D9RenderImage* imaged3d9ptype = static_cast<D3D9RenderImage*>(b.image);
		pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, imaged3d9ptype->_isAlpha());
		pID3DDevice->SetTexture(0, imaged3d9ptype->_getTexPointer());

		UINT num_vertices = (UINT)b.end - (UINT)b.begin;
		UINT num_primitives = num_vertices/2;
		pID3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, num_vertices, startIndex, num_primitives);
		startIndex += num_primitives*3;
	}

	m_vertices.clear();
	m_batches.clear();
}

void	Direct3d9RenderSystem::renderImage(
				const mt::mat3f& _transform, 
				const mt::vec2f& _offset,
				const mt::vec4f& _uv,
				const mt::vec2f& _size,
				unsigned int _mixedColor, 
				RenderImageInterface* _rmi)
{
	
	_prepareBatch(_rmi,-1);

	D3D9RenderImage* imaged3d9ptype = static_cast<D3D9RenderImage*>(_rmi);

	D3D9Vertex* srcVertices = imaged3d9ptype->_getD3D9V4();

	srcVertices[0].position = mt::vec3f(0.0f, 0.0f, 1.0f);
	srcVertices[1].position = mt::vec3f(_size.x, 0.0f, 1.0f);
	srcVertices[2].position = mt::vec3f(_size.x, _size.y, 1.0f);
	srcVertices[3].position = mt::vec3f(0.0f, _size.y, 1.0f);

	for(size_t i = 0; i < 4; ++i)
	{
		mt::vec3f transformed_pos;
		mt::mul_v3_m3(transformed_pos, mt::vec3f(srcVertices[i].position.v2+_offset,1), _transform );
		srcVertices[i].position = transformed_pos;
		srcVertices[i].color = _mixedColor;
	}

	srcVertices[0].tcoor = mt::vec2f(_uv[0], _uv[1]);
	srcVertices[1].tcoor = mt::vec2f(_uv[2], _uv[1]);
	srcVertices[2].tcoor = mt::vec2f(_uv[2], _uv[3]);
	srcVertices[3].tcoor = mt::vec2f(_uv[0], _uv[3]);

	m_vertices.insert(m_vertices.end(), srcVertices, srcVertices + 4);

	m_batches.back().end = m_vertices.size();
}

void	Direct3d9RenderSystem::_initRenderSystemParams()
{
	pID3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pID3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	pID3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pID3DDevice->SetTextureStageState(0,D3DTSS_COLOROP,  D3DTOP_MODULATE);
	pID3DDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	pID3DDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);

	pID3DDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,  D3DTOP_MODULATE);
	pID3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	pID3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
}

bool	Direct3d9RenderSystem::_initBatching()
{
	m_size4Verts = sizeof(D3D9Vertex) * 4;
	m_size1Vert = sizeof(D3D9Vertex);

	if (D3D_OK != pID3DDevice->CreateVertexBuffer(
		BATCH_BUFFER_SIZE * m_size1Vert, 
		D3DUSAGE_WRITEONLY,
		D3DFVF_TLVERTEX,
		D3DPOOL_MANAGED,
		&m_batchFontVB,
		NULL
		))
	{
		return	false;
	}

	if (D3D_OK != pID3DDevice->CreateIndexBuffer(
		BATCH_BUFFER_SIZE * 3, 
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_batchFontIB,
		NULL
		))
	{
		return	false;
	}

	int		index = 0;
	short*	indices = NULL;

	if (FAILED(m_batchFontIB->Lock(0, BATCH_BUFFER_SIZE*6/4*sizeof(WORD), (VOID**)&indices, 0)))
	{
		return	false;
	}

	int n = 0;

	for (int i = 0; i < BATCH_BUFFER_SIZE/4; i++)
	{
		indices[index++] = n + 0;
		indices[index++] = n + 1;
		indices[index++] = n + 2;
		indices[index++] = n + 2;
		indices[index++] = n + 3;
		indices[index++] = n + 0;
		n+=4;
	}

	m_batchFontIB->Unlock();

	return	true;
}
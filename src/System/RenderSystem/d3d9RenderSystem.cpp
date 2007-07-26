#	include "d3d9RenderSystem.h"
#	include "d3d9VertexData.h"
#	include "d3d9IndexData.h"
#	include "d3d9VertexDeclaration.h"
#	include "d3d9RenderFont.h"
#	include "d3d9RenderImage.h"

#	include <windows.h>
#	include <assert.h>
//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem(RenderSystemInterface** _ptrRenderSystem)
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
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem(RenderSystemInterface* _ptrRenderSystem)
{
	delete static_cast<Direct3d9RenderSystem*>(_ptrRenderSystem);
}
//////////////////////////////////////////////////////////////////////////
Direct3d9RenderSystem::Direct3d9RenderSystem()
	: pID3DDevice(0)
{}
//////////////////////////////////////////////////////////////////////////
Direct3d9RenderSystem::~Direct3d9RenderSystem()
{
	BatchRender.destroy();
}
//////////////////////////////////////////////////////////////////////////
void	Direct3d9RenderSystem::setProjectionMatrix(const mt::mat4f& _projection)
{
	HRESULT hr = pID3DDevice->SetTransform(D3DTS_PROJECTION,LPD3DXMATRIX(_projection.m));
	if(hr != S_OK)
	{
		assert(!"can't set projection matrix");
	}
}
//////////////////////////////////////////////////////////////////////////
void	Direct3d9RenderSystem::setViewMatrix(const mt::mat4f& _view)
{
	HRESULT hr = pID3DDevice->SetTransform(D3DTS_VIEW,LPD3DXMATRIX(_view.m));
	if(hr != S_OK)
	{
		assert(!"can't set view matrix");
	}
}
//////////////////////////////////////////////////////////////////////////
void	Direct3d9RenderSystem::setWorldMatrix(const mt::mat4f& _world)
{
	HRESULT hr = pID3DDevice->SetTransform(D3DTS_WORLD,LPD3DXMATRIX(_world.m));
	if(hr != S_OK)
	{
		assert(!"can't set world matrix");
	}
}
//////////////////////////////////////////////////////////////////////////
void	Direct3d9RenderSystem::setTexture(RenderImageInterface * _tex)
{
	D3D9RenderImage * _d3d9tex = static_cast<D3D9RenderImage*>(_tex);

	HRESULT hr;

	if(_d3d9tex == NULL)
	{
		hr = pID3DDevice->SetTexture(0,NULL);
	}
	else
	{
		hr = pID3DDevice->SetTexture(0,_d3d9tex->_get());
	}

	if(hr != S_OK)
	{
		assert(!"can't set texture");
	}
}
//////////////////////////////////////////////////////////////////////////
bool	Direct3d9RenderSystem::createDisplay(
	HWND _hWnd, int _width, int _height, int _bits, bool _fullScreen,
	bool _vsync, bool _stencilBuffer, bool _antiAlias, bool _pureSoftware)
{
	HRESULT hr;

	DeviceLost = false;

	IDirect3D9 * pID3D = Direct3DCreate9(D3D_SDK_VERSION);
	
	D3DADAPTER_IDENTIFIER9 dai;

	if (!FAILED(pID3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &dai)))
	{
		int Product = HIWORD(dai.DriverVersion.HighPart);
		int Version = LOWORD(dai.DriverVersion.HighPart);
		int SubVersion = HIWORD(dai.DriverVersion.LowPart);
		int Build = LOWORD(dai.DriverVersion.LowPart);

		printf("%s %s %d.%d.%d.%d\n", dai.Description, dai.Driver, Product, Version,
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
		{
			present.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		}
		
		// request 32bit mode if user specified 32 bit, added by Thomas Stüfe
		if (_bits == 32 && !_stencilBuffer) 
		{
			present.BackBufferFormat = D3DFMT_A8R8G8B8;
		}
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
	{
		pID3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	}

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

	BatchRender.init(pID3DDevice);

	_initRenderSystemParams();

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool Direct3d9RenderSystem::beginScene(int _color)
{
	HRESULT hr;

	if (!pID3DDevice)
	{
		return false;
	}

	if (DeviceLost)
	{
		if(FAILED(hr = pID3DDevice->TestCooperativeLevel()))
        {
            if (hr == D3DERR_DEVICELOST)
                return false;

            if (hr == D3DERR_DEVICENOTRESET)
			{
				_initRenderSystemParams();
			}
            return false;
        }
	}

	DWORD flags = 0;

	bool backBuffer = true;
	bool zBuffer = true;

	if (backBuffer)
	{
		flags |= D3DCLEAR_TARGET;
	}

	if (zBuffer)
	{
		flags |= D3DCLEAR_ZBUFFER;
	}

	//if (StencilBuffer)
	//	flags |= D3DCLEAR_STENCIL;

	hr = pID3DDevice->Clear( 0, NULL, flags, _color, 1.0, 0);

	if (FAILED(hr))
	{
		printf("DIRECT3D9 clear failed.");
	}

	hr = pID3DDevice->BeginScene();

	if (FAILED(hr))
	{
		printf("DIRECT3D9 begin scene failed.");
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool Direct3d9RenderSystem::endScene()
{
	BatchRender.render();
	if (DeviceLost)
	{
		return false;
	}

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
	{
		if (FAILED(hr) && hr != D3DERR_INVALIDCALL)
		{
			printf("DIRECT3D9 present failed.");
			return false;
		}
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void	Direct3d9RenderSystem::setMaterialColor(unsigned char _ambient[4], 
		unsigned char _diffuse[4],
		unsigned char _specular[4])
{
	D3DMATERIAL9 mat;

    mat.Ambient.r = _ambient[0]/255.0f;
	mat.Ambient.g = _ambient[1]/255.0f;
    mat.Ambient.b = _ambient[2]/255.0f;
	mat.Ambient.a = _ambient[3]/255.0f;

    mat.Diffuse.r = _diffuse[0]/255.0f;
	mat.Diffuse.g = _diffuse[1]/255.0f;
    mat.Diffuse.b = _diffuse[2]/255.0f;
	mat.Diffuse.a = _diffuse[3]/255.0f;

    mat.Specular.r = _specular[0]/255.0f;
	mat.Specular.g = _specular[1]/255.0f;
    mat.Specular.b = _specular[2]/255.0f;
	mat.Specular.a = _specular[3]/255.0f;

	mat.Power = 0.0f;
    mat.Emissive.r = 0.0f;
	mat.Emissive.g = 0.0f;
	mat.Emissive.b = 0.0f;
	mat.Emissive.a = 0.0f;

	HRESULT hr = pID3DDevice->SetMaterial(&mat);
	if(hr != S_OK)
	{
		assert(!"Invalid material!");
	}
}
//////////////////////////////////////////////////////////////////////////
RenderFontInterface * Direct3d9RenderSystem::loadFont(const FontDesc &_fontDesc)
{
	RenderImageInterface * image = loadImage(_fontDesc.texDesc);
	return new D3D9Font(image, _fontDesc);
}
//////////////////////////////////////////////////////////////////////////
void	Direct3d9RenderSystem::releaseRenderFont(RenderFontInterface* _fnt)
{
	delete static_cast<D3D9Font*>(_fnt);
}
//////////////////////////////////////////////////////////////////////////
void	Direct3d9RenderSystem::releaseRenderImage(RenderImageInterface* _rmi)
{
	delete static_cast<D3D9RenderImage*>(_rmi);
}
//////////////////////////////////////////////////////////////////////////
void	Direct3d9RenderSystem::renderText(const mt::vec2f& _pos, 
										  RenderFontInterface * _font,
										  const std::string & _text)

{
	D3D9Font * d3d9font = static_cast<D3D9Font*>(_font);

	RenderImageInterface * renderImage = d3d9font->_get();

	BatchRender.begin(renderImage,-1);

	mt::vec2f letterPosition = _pos;

	for(std::string::const_iterator i = _text.begin(), e = _text.end(); i != e; ++i)
	{		
		FontCharDesc & Char = d3d9font->_getChar(*i);

		if(*i == ' ')	
		{
			letterPosition.x += Char.width;
			continue;
		}

		float	letterWidth = letterPosition.x + Char.width;
		float	letterHeight = letterPosition.y + _font->getHeight();

		D3D9Vertex m_fontVerts[4] = {};

		m_fontVerts[0].tcoor = mt::vec2f(Char.uv.x, Char.uv.y);
		m_fontVerts[1].tcoor = mt::vec2f(Char.uv.z, Char.uv.y);
		m_fontVerts[2].tcoor = mt::vec2f(Char.uv.z, Char.uv.w);
		m_fontVerts[3].tcoor = mt::vec2f(Char.uv.x, Char.uv.w);

		m_fontVerts[0].position = mt::vec3f(letterPosition.x, letterPosition.y, 0);
		m_fontVerts[1].position = mt::vec3f(letterWidth, letterPosition.y, 0);
		m_fontVerts[2].position = mt::vec3f(letterWidth, letterHeight, 0);
		m_fontVerts[3].position = mt::vec3f(letterPosition.x, letterHeight, 0);

		m_fontVerts[0].color = 
		m_fontVerts[1].color = 
		m_fontVerts[2].color = 
		m_fontVerts[3].color = 0xFFFFFFFF;

		m_fontVerts[0].rhw = 
		m_fontVerts[1].rhw = 
		m_fontVerts[2].rhw = 
		m_fontVerts[3].rhw = 1.0f;

		letterPosition.x += Char.width;

		BatchRender.insertVertices(m_fontVerts, m_fontVerts+4);
	}

	BatchRender.end();
}
//////////////////////////////////////////////////////////////////////////
void	Direct3d9RenderSystem::drawPrimitive(PrimitiveData * _pd)
{
	// If we've some 2d graphs before 3d rendering, so draw it.
	BatchRender.render();

	D3D9VertexData * vd = static_cast<D3D9VertexData*>(_pd->getVertexData());

	D3D9IndexData * id = static_cast<D3D9IndexData*>(_pd->getIndexData());

	HRESULT hr;

	hr = pID3DDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
	if(hr != S_OK)
	{
		assert(!"Can't set z buffer to true");
	}

	hr = pID3DDevice->SetVertexDeclaration( vd->_getVertexDecl() );
	if(hr != S_OK)
	{
		assert(!"Can't SetVertexDeclaration");
	}

	hr = pID3DDevice->SetStreamSource(0, vd->_get(), 0, vd->getVertexSize());
	if(hr != S_OK)
	{
		assert(!"Can't SetStreamSource");
	}

	hr = pID3DDevice->SetIndices(id->_get());
	if(hr != S_OK)
	{
		assert(!"Can't SetIndices");
	}

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

		hr = pID3DDevice->DrawIndexedPrimitive( primitiveType, 
			vertexStart, 0, vertexCount, indexStart, primCount );

		if(hr != S_OK)
		{
			assert(!"Can't DrawIndexedPrimitive");
		}
	}
	else
	{
		primCount = vd->getVertexCount()/3;

		hr = pID3DDevice->DrawPrimitive( primitiveType,
			vd->getVertexCount(), primCount );

		if(hr != S_OK)
		{
			assert(!"Can't DrawPrimitive");
		}
	} 

	hr = pID3DDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
	if(hr != S_OK)
	{
		assert(!"Can't set z buffer to false");
	}
}
//////////////////////////////////////////////////////////////////////////
VertexData * Direct3d9RenderSystem::createVertexData()
{
	VertexData * vertexData = new D3D9VertexData(pID3DDevice);
	return vertexData;
}
//////////////////////////////////////////////////////////////////////////
IndexData * Direct3d9RenderSystem::createIndexData()
{
	D3D9IndexData * indexData = new D3D9IndexData(pID3DDevice);
	return indexData;
}
//////////////////////////////////////////////////////////////////////////
VertexDeclaration * Direct3d9RenderSystem::createVertexDeclaration()
{
	D3D9VertexDeclaration * vertDecl = new D3D9VertexDeclaration(pID3DDevice);
	return vertDecl;
}
//////////////////////////////////////////////////////////////////////////
RenderImageInterface* Direct3d9RenderSystem::loadImage(const TextureDesc& _desc)
{
	D3DXIMAGE_INFO info;

	if( D3DXGetImageInfoFromFileInMemory( _desc.buffer, (UINT)_desc.size, &info ) != D3D_OK )
	{
		assert(!"can't get TEXTURE image info!");
	}

	bool haveAlpha = _desc.filter;

	LPDIRECT3DTEXTURE9	texPointer = NULL;

	if (FAILED(D3DXCreateTextureFromFileInMemoryEx(pID3DDevice, _desc.buffer, (UINT)_desc.size,
                                    info.Width, info.Height,
                                    1,                  
                                    0,                  
									haveAlpha ? D3DFMT_A8R8G8B8 : D3DFMT_R8G8B8,    
                                    D3DPOOL_MANAGED,    
                                    D3DX_FILTER_NONE,   
                                    D3DX_DEFAULT,       
                                    0,                  
                                    NULL, NULL,
                                    &texPointer)))
    {
        assert(!"can't get TEXTURE desc!");
    }

    return new D3D9RenderImage(texPointer, _desc);
}
//////////////////////////////////////////////////////////////////////////
void	Direct3d9RenderSystem::renderImage(
				const mt::mat3f& _transform, 
				const mt::vec2f& _offset,
				const mt::vec4f& _uv,
				const mt::vec2f& _size,
				unsigned int _mixedColor, 
				RenderImageInterface* _rmi)
{
	BatchRender.begin(_rmi,-1);

	D3D9RenderImage * imaged3d9ptype = static_cast<D3D9RenderImage*>(_rmi);

	static D3D9Vertex srcVertices[4] = {};

	srcVertices[0].position = mt::vec3f(0.0f, 0.0f, 1.0f);
	srcVertices[1].position = mt::vec3f(_size.x, 0.0f, 1.0f);
	srcVertices[2].position = mt::vec3f(_size.x, _size.y, 1.0f);
	srcVertices[3].position = mt::vec3f(0.0f, _size.y, 1.0f);

	srcVertices[0].rhw = 1.0f;
	srcVertices[1].rhw = 1.0f;
	srcVertices[2].rhw = 1.0f;
	srcVertices[3].rhw = 1.0f;

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

	BatchRender.insertVertices(srcVertices, srcVertices+4);
	BatchRender.end();
}
//////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////
void Direct3d9RenderSystem::drawLine2D(const mt::vec2f& p1, const mt::vec2f& p2, unsigned long _color)
{
	BatchRender.render();
	//_renderBatches();

	pID3DDevice->SetVertexShader(NULL);
	pID3DDevice->SetIndices(NULL);
	pID3DDevice->SetStreamSource(0,0,0,0);

	pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	struct Vertex
	{
		float x, y, z, w;
		unsigned long color; 
	};

	Vertex p[2] = {{p1.x,p1.y,1.0f,1.0f,_color}, {p2.x,p2.y,1.0f,1.0f,_color}};
	
	if FAILED(pID3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE))
	{
		assert(0);
	}
	
	if FAILED(pID3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, (void *)p, sizeof(Vertex)))
	{
		assert(0);
	}

	pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
}
//////////////////////////////////////////////////////////////////////////
void Direct3d9RenderSystem::drawLine3D(const mt::vec3f& p1,const mt::vec3f& p2, unsigned long _color)
{
	BatchRender.render();
	//_renderBatches();

	pID3DDevice->SetVertexShader(NULL);
	pID3DDevice->SetIndices(NULL);
	pID3DDevice->SetStreamSource(0,0,0,0);

	pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pID3DDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);

	float vert[6] = {p1.x, p1.y, p1.z, p2.x, p2.y, p2.z};

	HelperVertex p[2] = {{p1.x,p1.y,p1.z,_color}, {p2.x,p2.y,p2.z,_color}};

	if FAILED(pID3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE))
	{
		assert(0);
	}

	if FAILED(pID3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, (void *)p, sizeof(HelperVertex)))
	{
		assert(0);
	}

	pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pID3DDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
}
//////////////////////////////////////////////////////////////////////////
void Direct3d9RenderSystem::drawBox( const mt::vec3f & _minEdge, const mt::vec3f & _maxEdge, unsigned long _color)
{
	BatchRender.render();

	pID3DDevice->SetVertexShader(NULL);
	pID3DDevice->SetIndices(NULL);
	pID3DDevice->SetStreamSource(0,0,0,0);
	pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pID3DDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);

	float vert[6] = {_minEdge.x, _minEdge.y, _minEdge.z, _maxEdge.x, _maxEdge.y, _maxEdge.z};

	HelperVertex p[24];

	for (int i = 0; i < 2; i++)
	{
		p[0+i*12].x = vert[0];
		p[0+i*12].y = vert[1];
		p[0+i*12].z = vert[2+i*3];

		p[1+i*12].x = vert[0];
		p[1+i*12].y = vert[4];
		p[1+i*12].z = vert[2+i*3];

		p[2+i*12].x = vert[0];
		p[2+i*12].y = vert[4];
		p[2+i*12].z = vert[2+i*3];

		p[3+i*12].x = vert[3];
		p[3+i*12].y = vert[4];
		p[3+i*12].z = vert[2+i*3];

		p[4+i*12].x = vert[3];
		p[4+i*12].y = vert[4];
		p[4+i*12].z = vert[2+i*3];

		p[5+i*12].x = vert[3];
		p[5+i*12].y = vert[1];
		p[5+i*12].z = vert[2+i*3];

		p[6+i*12].x = vert[3];
		p[6+i*12].y = vert[1];
		p[6+i*12].z = vert[2+i*3];

		p[7+i*12].x = vert[0];
		p[7+i*12].y = vert[1];
		p[7+i*12].z = vert[2+i*3];

		for (int j = 0; j < 2; j++)
		{
			p[8+j*2+i*12].x = vert[3*i];
			p[8+j*2+i*12].y = vert[3*j + 1];
			p[8+j*2+i*12].z = vert[2];

			p[9+j*2+i*12].x = vert[3*i];
			p[9+j*2+i*12].y = vert[3*j + 1];
			p[9+j*2+i*12].z = vert[5];
		}

	}

	for (int i = 0; i < 24; i++) 
	{
		p[i].color = _color;
	}
	
	if FAILED(pID3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE))
	{
		assert(0);
	}

	if FAILED(pID3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 12,(void *)p, sizeof(HelperVertex)))
	{
		assert(0);
	}

	pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pID3DDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
}
//////////////////////////////////////////////////////////////////////////
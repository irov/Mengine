#	include "RenderSystem.h"
#	include "RenderImage.h"

#	include <assert.h>

#	pragma comment (lib, "d3d9.lib")
#	pragma comment (lib, "d3dx9.lib")

#	include "../../Menge/Application.h"

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

Direct3d9RenderSystem::Direct3d9RenderSystem()
{
	m_direct3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (!m_direct3d9)
	{
		assert(!"error Direct3DCreate9\n");
	}
}

Direct3d9RenderSystem::~Direct3d9RenderSystem()
{
	if(m_pLine)
	{
		m_pLine->Release();
	} 

	if (m_direct3d9 != NULL)
	{
		m_direct3d9->Release();
	}

	if (m_deviceD3D9 != NULL)
	{
		m_deviceD3D9->Release();
	}
}


RenderImageInterface* Direct3d9RenderSystem::loadImage(const TextureDesc& _desc)
{
    return new D3D9RenderImage(m_deviceD3D9, _desc);
}

RenderFontInterface * Direct3d9RenderSystem::loadFont(const FontDesc &_fontDesc)
{
	return new D3D9Font(m_deviceD3D9, _fontDesc);
}

void	Direct3d9RenderSystem::releaseRenderFont(RenderFontInterface* _fnt)
{
	delete static_cast<D3D9Font*>(_fnt);
}

void	Direct3d9RenderSystem::releaseRenderImage(RenderImageInterface* _rmi)
{
	delete static_cast<D3D9RenderImage*>(_rmi);
}

void	Direct3d9RenderSystem::renderImage(			
					const mt::mat3f& _transform, 
					const mt::vec2f& _offset,
					const mt::vec4f& _uv,
					const mt::vec2f& _size,
					unsigned int _mixedColor, 
					RenderImageInterface* _rmi)
{
	D3D9RenderImage*	imaged3d9ptype = static_cast<D3D9RenderImage*>(_rmi);

	D3D9Vertex* srcVertices = imaged3d9ptype->_getD3D9V4();

	D3D9Vertex*	dstVertices = NULL;

	m_vbDynamic->Lock(0, m_size4Verts, (VOID**)&dstVertices, 0);

	srcVertices[0].position = mt::vec3f(0.0f, 0.0f, 1.0f);
	srcVertices[1].position = mt::vec3f(_size.x, 0.0f, 1.0f);
	srcVertices[2].position = mt::vec3f(_size.x, _size.y, 1.0f);
	srcVertices[3].position = mt::vec3f(0.0f, _size.y, 1.0f);

	for(size_t i = 0; i < 4; ++i)
	{
		mt::mul_v3_m3(dstVertices[i].position, mt::vec3f(srcVertices[i].position.v2+_offset,1), _transform );
		dstVertices[i].color = _mixedColor;
	}

	dstVertices[0].tcoor = mt::vec2f(_uv[0], _uv[1]);
	dstVertices[1].tcoor = mt::vec2f(_uv[2], _uv[1]);
	dstVertices[2].tcoor = mt::vec2f(_uv[2], _uv[3]);
	dstVertices[3].tcoor = mt::vec2f(_uv[0], _uv[3]);

	m_vbDynamic->Unlock();

	m_deviceD3D9->SetRenderState(D3DRS_ALPHABLENDENABLE, imaged3d9ptype->_isAlpha());
	m_deviceD3D9->SetTexture(0, imaged3d9ptype->_getTexPointer());
	m_deviceD3D9->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
}



void	Direct3d9RenderSystem::renderText(mt::vec2f _pos, 
										  RenderFontInterface* _font,
										  const std::string& _text)
{	
	D3D9Font*   fontd3d9ptype = static_cast<D3D9Font*>(_font);
 
    FontBatch   fontBatch = {_pos,_text,fontd3d9ptype};
 
    LPDIRECT3DTEXTURE9	texture = fontd3d9ptype->_getTexPointer();
 
	m_flushBuffers.insert(std::make_pair(texture,fontBatch));
}

void	Direct3d9RenderSystem::_flushFonts()
{
	if(m_flushBuffers.empty())
	{
		return;
	}

	static	D3D9Vertex*		batchVertices;
	
	m_currentTex = m_flushBuffers.begin()->first;
	
	m_deviceD3D9->SetTexture(0, m_currentTex);

	m_deviceD3D9->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	
	m_deviceD3D9->SetStreamSource(0, m_batchVB, 0, m_size1Vert);

	m_numBatchVertices = 0;

	unsigned long	count = 0;

	m_batchVB->Lock(0, BATCH_BUFFER_SIZE * m_size1Vert, (VOID**)&batchVertices, 0);
	
	for(TMultimapFontBatch::iterator it = m_flushBuffers.begin(); it != m_flushBuffers.end(); ++it)
	{
		mt::vec2f& anPos = it->second.pos;
	
		size_t textSize = it->second.text.size();

		m_fontVerts.resize(textSize * 4);

		if (m_numBatchVertices >= BATCH_BUFFER_SIZE || m_currentTex != it->first)
		{
			m_batchVB->Unlock();

			m_deviceD3D9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_numBatchVertices, 0, m_numBatchVertices / 2); 
		
			m_currentTex = it->first;
			m_deviceD3D9->SetTexture(0, m_currentTex);
	
			m_numBatchVertices = 0;
			m_batchVB->Lock(0, BATCH_BUFFER_SIZE * m_size1Vert, (VOID**)&batchVertices, 0);
		}

		count = 0;
	
		for(std::string::const_iterator i = it->second.text.begin(), e = it->second.text.end(); i != e; ++i)
		{		
			FontCharDesc	Char = it->second.font->_getChar(*i);

			if(*i == ' ')
			{
				anPos.x+=Char.width;
				continue;
			}

			m_fontVerts[count+0].tcoor = mt::vec2f(Char.uv.x, Char.uv.y);
			m_fontVerts[count+1].tcoor = mt::vec2f(Char.uv.z, Char.uv.y);
			m_fontVerts[count+2].tcoor = mt::vec2f(Char.uv.z, Char.uv.w);
			m_fontVerts[count+3].tcoor = mt::vec2f(Char.uv.x, Char.uv.w);

		
			float	offX = anPos.x + Char.width;
			float	offY = anPos.y + it->second.font->getHeight();

			m_fontVerts[count+0].position = mt::vec3f(anPos.x, anPos.y, 0);
			m_fontVerts[count+1].position = mt::vec3f(offX, anPos.y, 0);
			m_fontVerts[count+2].position = mt::vec3f(offX, offY, 0);
			m_fontVerts[count+3].position = mt::vec3f(anPos.x, offY, 0);

			m_fontVerts[count+0].color = 
			m_fontVerts[count+1].color = 
			m_fontVerts[count+2].color = 
			m_fontVerts[count+3].color = 0xFFFFFFFF;

			m_fontVerts[count+0].rhw = 1.0f;
			m_fontVerts[count+1].rhw = 1.0f;
			m_fontVerts[count+2].rhw = 1.0f;
			m_fontVerts[count+3].rhw = 1.0f;

			anPos.x+=Char.width;

			count+=4;
		}
		
		memcpy(&batchVertices[m_numBatchVertices], &m_fontVerts[0], m_size1Vert * count);
		m_numBatchVertices+=count;
	}

	m_batchVB->Unlock();
	
	if(m_numBatchVertices)
	{
		m_deviceD3D9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_numBatchVertices, 0, m_numBatchVertices/2); 
	}

	m_deviceD3D9->SetStreamSource(0, m_vbDynamic, 0, m_size1Vert);
	
    m_flushBuffers.clear();
}

bool	Direct3d9RenderSystem::beginSceneDrawing(unsigned long _color)
{
	if (!m_deviceD3D9)
	{
		return false;
	}

	HRESULT	hr;

	if (m_deviceLost)
	{
		if(FAILED(hr = m_deviceD3D9->TestCooperativeLevel()))
        {
            if (hr == D3DERR_DEVICELOST)
			{
                return false;
			}

            if (hr == D3DERR_DEVICENOTRESET)
			{
				if (FAILED(m_deviceD3D9->Reset(&m_present)))
				{
					return false;
				}

				_initRenderSystem();

				m_deviceD3D9->SetVertexShader(NULL);
				
				m_deviceD3D9->SetFVF (D3DFVF_TLVERTEX);
				m_deviceD3D9->SetStreamSource(0, m_vbDynamic, 0, m_size1Vert);
				m_deviceD3D9->SetIndices(m_batchIB);

				m_deviceLost = false;
			}
        }
	}

	if (FAILED(m_deviceD3D9->Clear(0, NULL, D3DCLEAR_TARGET, _color, 1.0, 0)))
	{
		return false;
	}

	if (FAILED(m_deviceD3D9->BeginScene()))
	{
		return	false;
	}

	return	true;
}

bool	Direct3d9RenderSystem::endSceneDrawing()
{
	_flushFonts();

	if (m_deviceLost)
	{
		return false;
	}
	
	if (FAILED(m_deviceD3D9->EndScene()))
	{
		return	false;
	}

	HRESULT hr = m_deviceD3D9->Present(NULL,NULL,NULL,NULL);
	if (hr == D3DERR_DEVICELOST)
	{
		m_deviceLost = true;
	}
	else
	if (FAILED(hr) && hr != D3DERR_INVALIDCALL)
	{
		return false;
	}

	return	true;
}

/* это мне не нравиться, надо куда-нить убрать*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;
	case WM_DESTROY:
        PostQuitMessage(0);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
/* */

bool Direct3d9RenderSystem::createDisplay(int _width, int _height, int _bits, bool _fullScreen)
{
	m_width = _width;
	m_height = _height;
	m_bits = _bits;
	m_fullScreen = _fullScreen;

	bool vsync = true;

	HINSTANCE hInstance = GetModuleHandle(0);

	D3DDISPLAYMODE d3ddm;
	HRESULT hr = m_direct3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

	if (FAILED(hr))
	{
		assert(!"[!] Could not get Adapter Display mode.");
	}

	const char* className = "main window";

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= className;
	wcex.hIconSm		= 0;
	wcex.hIcon			= NULL; 
	RegisterClassEx(&wcex);

	RECT clientSize;

	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = m_width;
	clientSize.bottom = m_height;
	
	DWORD style = WS_POPUP;

	if (!_fullScreen)
	{
		style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	}

	AdjustWindowRect(&clientSize, style, FALSE);

	unsigned int realWidth = clientSize.right - clientSize.left;
	unsigned int realHeight = clientSize.bottom - clientSize.top;

	unsigned int windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
	unsigned int windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

	if (_fullScreen)
	{
		windowLeft = 0;
		windowTop = 0;
	}

	HWND HWnd = CreateWindow(className,"", style, windowLeft, windowTop, realWidth, realHeight,NULL, NULL, hInstance, NULL);

	ShowWindow(HWnd, SW_SHOW);
	UpdateWindow(HWnd);

	MoveWindow(HWnd, windowLeft, windowTop, realWidth, realHeight, TRUE);

	ZeroMemory(&m_present, sizeof(m_present));

	m_present.SwapEffect = _fullScreen ? D3DSWAPEFFECT_FLIP : D3DSWAPEFFECT_COPY;
	m_present.Windowed = _fullScreen ? FALSE : TRUE;
	m_present.BackBufferFormat = d3ddm.Format;
	//mPresent.EnableAutoDepthStencil		= TRUE;
	m_present.EnableAutoDepthStencil = FALSE;
	m_present.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (_fullScreen)
	{
		m_present.BackBufferWidth = m_width;
		m_present.BackBufferHeight = m_height;
		m_present.BackBufferFormat = D3DFMT_R5G6B5;
		m_present.BackBufferCount = 0;
		m_present.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

		if (vsync)
		{
			m_present.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		}
		
		if (_bits == 32) 
		{
			m_present.BackBufferFormat = D3DFMT_A8R8G8B8;
		}
	}

	D3DDEVTYPE devtype = D3DDEVTYPE_HAL;

	m_present.AutoDepthStencilFormat = D3DFMT_D16;
	
	//#define _NVPERFHUD_

	#ifndef _NVPERFHUD_
		if (FAILED(m_direct3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, HWnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&m_present, &m_deviceD3D9 )))
		{
			assert(!"[!] error CreateDevice\n");
		}
	#else
	  D3DADAPTER_IDENTIFIER9 Identifier;
	  bool use_nvpefhud = false;
	  m_direct3d9->GetAdapterIdentifier(m_direct3d9->GetAdapterCount() - 1, 0 , &Identifier);
	  if (strcmp(Identifier.Description, "NVIDIA NVPerfHUD") == 0)
		  use_nvpefhud = true;

	  if (use_nvpefhud)
	  {
		LOG.msg("[i] Используем NVPerfhud\n");
		if (FAILED(m_direct3d9->CreateDevice(m_direct3d9->GetAdapterCount() - 1,
			D3DDEVTYPE_REF, HWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&m_present, &m_deviceD3D9 )))
		{
		  LOG.msg("[!] Ошибка CreateDevice\n");
	//	  return false;
		}
	  }
	  else
	  {
		if (FAILED(m_direct3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, HWnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&m_present, &m_deviceD3D9 )))
		{
		  LOG.msg("[!] Ошибка CreateDevice\n");
	//	  return false;
		}
	  }
	#endif

	m_deviceLost = false;
	_initRenderSystem();
	_initBatching();
	return true;
}

bool	Direct3d9RenderSystem::_initRenderSystem()
{
	m_deviceD3D9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_deviceD3D9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	m_deviceD3D9->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_deviceD3D9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_deviceD3D9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_deviceD3D9->SetTextureStageState(0,D3DTSS_COLOROP,  D3DTOP_MODULATE);
	m_deviceD3D9->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	m_deviceD3D9->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);

	m_deviceD3D9->SetTextureStageState(0,D3DTSS_ALPHAOP,  D3DTOP_MODULATE);
	m_deviceD3D9->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	m_deviceD3D9->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);

	m_deviceD3D9->SetTexture(0,NULL);
	m_currentTex = NULL;

	// #######################
	if(FAILED(D3DXCreateLine(m_deviceD3D9, &m_pLine)) )
	{
		assert(!"can't create line");
	}

	return	true;
}

bool	Direct3d9RenderSystem::_initBatching()
{
	m_size4Verts = sizeof(D3D9Vertex) * 4;
	m_size1Vert = sizeof(D3D9Vertex);

	m_deviceD3D9->CreateVertexBuffer(
		m_size4Verts, 
		D3DUSAGE_WRITEONLY,
		D3DFVF_TLVERTEX,
		D3DPOOL_MANAGED,
		&m_vbDynamic, 
		NULL
	);

	m_fontVerts.reserve(1000);

	if (D3D_OK != m_deviceD3D9->CreateVertexBuffer(
		BATCH_BUFFER_SIZE * m_size1Vert, 
		D3DUSAGE_WRITEONLY,
		D3DFVF_TLVERTEX,
		D3DPOOL_MANAGED,
		&m_batchVB,
		NULL
		))
	{
		return	false;
	}

	if (D3D_OK != m_deviceD3D9->CreateIndexBuffer(
		BATCH_BUFFER_SIZE * 3, 
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_batchIB,
		NULL
		))
	{
		return	false;
	}

	m_numBatchVertices = 0;

	int		index = 0;
	short*	indices = NULL;

	if (FAILED(m_batchIB->Lock(0, BATCH_BUFFER_SIZE * 3, (VOID**)&indices, 0)))
	{
		return	false;
	}

	for (int i = 0; i < BATCH_BUFFER_SIZE; i +=4)
	{
		indices[index++] = i + 0;
		indices[index++] = i + 2;
		indices[index++] = i + 3;
		indices[index++] = i + 0;
		indices[index++] = i + 1;
		indices[index++] = i + 2;
	}

	m_batchIB->Unlock();

	m_deviceD3D9->SetVertexShader(NULL);
	m_deviceD3D9->SetFVF (D3DFVF_TLVERTEX);
	m_deviceD3D9->SetStreamSource(0, m_vbDynamic, 0, m_size1Vert);
	m_deviceD3D9->SetIndices(m_batchIB);
	return	true;
}

void Direct3d9RenderSystem::drawLine(const mt::vec2f& p1, const mt::vec2f& p2, float width, unsigned long color)
{
	m_pLine->SetAntialias(true);
	m_pLine->SetWidth(width);

	D3DXVECTOR2 pVector[2] = {*(D3DXVECTOR2*)&p1, *(D3DXVECTOR2*)&p2};

	if(FAILED(m_pLine->Draw(pVector, 2, color)) )
	{
		assert(!"can't draw line");
	}
}
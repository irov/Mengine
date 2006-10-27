#include "RenderSystem.h"

#include <assert.h>

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
	mDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (!mDirect3D9)
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

	if (mDirect3D9 != NULL)
	{
		mDirect3D9->Release();
	}

	if (mDeviceD3D9 != NULL)
	{
		mDeviceD3D9->Release();
	}
}


RenderImageInterface* Direct3d9RenderSystem::loadImage(const TextureDesc& _desc)
{
    return new D3D9RenderImage(mDeviceD3D9, _desc);
}

RenderFontInterface * Direct3d9RenderSystem::loadFont(const FontDesc &_fontDesc)
{
	return new D3D9Font(mDeviceD3D9, _fontDesc);
}

void	Direct3d9RenderSystem::releaseRenderFont(RenderFontInterface* _fnt)
{
	delete static_cast<D3D9Font*>(_fnt);
}

void	Direct3d9RenderSystem::releaseRenderImage(RenderImageInterface* _rmi)
{
	delete static_cast<D3D9RenderImage*>(_rmi);
}

void Direct3d9RenderSystem::renderImage(const mt::mat3f& _transform,
										unsigned int _mixedColor, 
										RenderImageInterface * _rmi)
{
	D3D9RenderImage*	imaged3d9ptype = static_cast<D3D9RenderImage*>(_rmi);

	const D3D9Vertex* srcVertices = imaged3d9ptype->_getD3D9V4();
	 
	D3D9Vertex*	dstVertices = NULL;

	mVBDynamic->Lock(0, mSizeOf4Verts, (VOID**)&dstVertices, 0);

	for(size_t i = 0; i < 4; ++i)
	{
		mt::mul_v3_m3(dstVertices[i].position, srcVertices[i].position, _transform );
		dstVertices[i].rhw = 1.0f;
		dstVertices[i].tcoor = srcVertices[i].tcoor;
		dstVertices[i].mColor = _mixedColor;
	}

	mVBDynamic->Unlock();

	mDeviceD3D9->SetRenderState(D3DRS_ALPHABLENDENABLE, imaged3d9ptype->_isAlpha());
	mDeviceD3D9->SetTexture(0, imaged3d9ptype->_getTexPointer());
	mDeviceD3D9->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

	drawLine(dstVertices[0].position.v2,dstVertices[1].position.v2,1,0xffffffff);
	drawLine(dstVertices[1].position.v2,dstVertices[2].position.v2,1,0xffffffff);
	drawLine(dstVertices[2].position.v2,dstVertices[3].position.v2,1,0xffffffff);
	drawLine(dstVertices[3].position.v2,dstVertices[0].position.v2,1,0xffffffff);
}

void	Direct3d9RenderSystem::renderText(mt::vec2f _pos, 
										  RenderFontInterface* _font,
										  const std::string& _text)
{	
	D3D9Font*   fontd3d9ptype = static_cast<D3D9Font*>(_font);
 
    FontBatch   fontBatch = {_pos,_text,fontd3d9ptype};
 
    LPDIRECT3DTEXTURE9	texture = fontd3d9ptype->_getTexPointer();
 
	mBuffersToFlush.insert(std::make_pair(texture,fontBatch));
}

void	Direct3d9RenderSystem::_flushFonts()
{
	if(mBuffersToFlush.empty())
	{
		return;
	}

	static	D3D9Vertex*		batchVertices;
	
	mCurrentTex = mBuffersToFlush.begin()->first;
	
	mDeviceD3D9->SetTexture(0, mCurrentTex);

	mDeviceD3D9->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	
	mDeviceD3D9->SetStreamSource(0, mBatchVB, 0, mSizeOfVert);

	mNumBatchVertices = 0;

	unsigned long	count = 0;

	mBatchVB->Lock(0, BATCH_BUFFER_SIZE * mSizeOfVert, (VOID**)&batchVertices, 0);
	
	for(TMultimapFontBatch::iterator it = mBuffersToFlush.begin(); it != mBuffersToFlush.end(); ++it)
	{
		mt::vec2f& anPos = it->second.pos;
	
		size_t textSize = it->second.text.size();

		mFontVertices.resize(textSize * 4);

		if (mNumBatchVertices >= BATCH_BUFFER_SIZE || mCurrentTex != it->first)
		{
			mBatchVB->Unlock();

			mDeviceD3D9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mNumBatchVertices, 0, mNumBatchVertices / 2); 
		
			mCurrentTex = it->first;
			mDeviceD3D9->SetTexture(0, mCurrentTex);
	
			mNumBatchVertices = 0;
			mBatchVB->Lock(0, BATCH_BUFFER_SIZE * mSizeOfVert, (VOID**)&batchVertices, 0);
		}

		count = 0;
	
		for(std::string::const_iterator i = it->second.text.begin(), e = it->second.text.end(); i != e; ++i)
		{		
			if(*i == ' ')
			{
				anPos.x+=it->second.font->getWidth();
				continue;
			}

			mt::vec4f	texcoor = it->second.font->_getChar(*i);

			mFontVertices[count+0].tcoor = mt::vec2f(texcoor.x, texcoor.y);
			mFontVertices[count+1].tcoor = mt::vec2f(texcoor.z, texcoor.y);
			mFontVertices[count+2].tcoor = mt::vec2f(texcoor.z, texcoor.w);
			mFontVertices[count+3].tcoor = mt::vec2f(texcoor.x, texcoor.w);

			float	offX = anPos.x+it->second.font->getWidth();
			float	offY = -it->second.font->getHeight();

			mFontVertices[count+0].position = mt::vec3f(anPos.x, 0.0f, 1.0f);
			mFontVertices[count+1].position = mt::vec3f(offX, 0.0f, 1.0f);
			mFontVertices[count+2].position = mt::vec3f(offX, offY, 1.0f);
			mFontVertices[count+3].position = mt::vec3f(anPos.x, offY, 1.0f);

			mFontVertices[count+0].mColor = 
			mFontVertices[count+1].mColor = 
			mFontVertices[count+2].mColor = 
			mFontVertices[count+3].mColor = 0xFFFFFFFF;

			anPos.x+=it->second.font->getWidth();

			count+=4;
		}
		
		memcpy(&batchVertices[mNumBatchVertices], &mFontVertices[0], mSizeOfVert * count);
		mNumBatchVertices+=count;
	}

	mBatchVB->Unlock();
	
	if(mNumBatchVertices)
	{
		mDeviceD3D9->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mNumBatchVertices, 0, mNumBatchVertices/2); 
	}

	mDeviceD3D9->SetStreamSource(0, mVBDynamic, 0, mSizeOfVert);
	
    mBuffersToFlush.clear();
}

bool	Direct3d9RenderSystem::beginSceneDrawing(unsigned long _color)
{
	if (!mDeviceD3D9)
	{
		return false;
	}

	HRESULT	hr;

	if (mDeviceLost)
	{
		if(FAILED(hr = mDeviceD3D9->TestCooperativeLevel()))
        {
            if (hr == D3DERR_DEVICELOST)
			{
                return false;
			}

            if (hr == D3DERR_DEVICENOTRESET)
			{
				if (FAILED(mDeviceD3D9->Reset(&mPresent)))
				{
					return false;
				}

				_initRenderSystem();

				mDeviceD3D9->SetVertexShader(NULL);
				
				mDeviceD3D9->SetFVF (D3DFVF_TLVERTEX);
				mDeviceD3D9->SetStreamSource(0, mVBDynamic, 0, mSizeOfVert);
				mDeviceD3D9->SetIndices(mBatchIB);

				mDeviceLost = false;
			}
        }
	}

	if (FAILED(mDeviceD3D9->Clear(0, NULL, D3DCLEAR_TARGET, _color, 1.0, 0)))
	{
		return false;
	}

	if (FAILED(mDeviceD3D9->BeginScene()))
	{
		return	false;
	}

	return	true;
}

bool	Direct3d9RenderSystem::endSceneDrawing()
{
	_flushFonts();

	if (mDeviceLost)
	{
		return false;
	}
	
	if (FAILED(mDeviceD3D9->EndScene()))
	{
		return	false;
	}

	HRESULT hr = mDeviceD3D9->Present(NULL,NULL,NULL,NULL);
	if (hr == D3DERR_DEVICELOST)
	{
		mDeviceLost = true;
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

bool Direct3d9RenderSystem::createDisplay(unsigned int _width, unsigned int _height, unsigned int _bits, bool _fullScreen)
{
	mWidth = _width;
	mHeight = _height;
	mBits = _bits;
	mFullScreen = _fullScreen;

	bool vsync = true;

	HINSTANCE hInstance = GetModuleHandle(0);

	D3DDISPLAYMODE d3ddm;
	HRESULT hr = mDirect3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

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
	clientSize.right = _width;
	clientSize.bottom = _height;
	
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

	ZeroMemory(&mPresent, sizeof(mPresent));

	mPresent.SwapEffect					= _fullScreen ? D3DSWAPEFFECT_FLIP : D3DSWAPEFFECT_COPY;
	mPresent.Windowed					= _fullScreen ? FALSE : TRUE;
	mPresent.BackBufferFormat			= d3ddm.Format;
	//mPresent.EnableAutoDepthStencil		= TRUE;
	mPresent.EnableAutoDepthStencil		= FALSE;
	mPresent.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;

	if (_fullScreen)
	{
		mPresent.BackBufferWidth = _width;
		mPresent.BackBufferHeight = _height;
		mPresent.BackBufferFormat = D3DFMT_R5G6B5;
		mPresent.BackBufferCount = 0;
		mPresent.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

		if (vsync)
		{
			mPresent.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		}
		
		if (_bits == 32) 
		{
			mPresent.BackBufferFormat = D3DFMT_A8R8G8B8;
		}
	}

	D3DDEVTYPE devtype = D3DDEVTYPE_HAL;

	mPresent.AutoDepthStencilFormat = D3DFMT_D16;
	
	//#define _NVPERFHUD_

	#ifndef _NVPERFHUD_
		if (FAILED(mDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, HWnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&mPresent, &mDeviceD3D9 )))
		{
			assert(!"[!] error CreateDevice\n");
		}
	#else
	  D3DADAPTER_IDENTIFIER9 Identifier;
	  bool use_nvpefhud = false;
	  mDirect3D9->GetAdapterIdentifier(mDirect3D9->GetAdapterCount() - 1, 0 , &Identifier);
	  if (strcmp(Identifier.Description, "NVIDIA NVPerfHUD") == 0)
		  use_nvpefhud = true;

	  if (use_nvpefhud)
	  {
		LOG.msg("[i] Используем NVPerfhud\n");
		if (FAILED(mDirect3D9->CreateDevice(mDirect3D9->GetAdapterCount() - 1,
			D3DDEVTYPE_REF, HWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&mPresent, &mDeviceD3D9 )))
		{
		  LOG.msg("[!] Ошибка CreateDevice\n");
	//	  return false;
		}
	  }
	  else
	  {
		if (FAILED(mDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, HWnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&mPresent, &mDeviceD3D9 )))
		{
		  LOG.msg("[!] Ошибка CreateDevice\n");
	//	  return false;
		}
	  }
	#endif

	mDeviceLost = false;
	_initRenderSystem();
	_initBatching();
	return true;
}

bool	Direct3d9RenderSystem::_initRenderSystem()
{
	mDeviceD3D9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	mDeviceD3D9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	mDeviceD3D9->SetRenderState(D3DRS_LIGHTING, FALSE);
	mDeviceD3D9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	mDeviceD3D9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	mDeviceD3D9->SetTextureStageState(0,D3DTSS_COLOROP,  D3DTOP_MODULATE);
	mDeviceD3D9->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	mDeviceD3D9->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);

	mDeviceD3D9->SetTextureStageState(0,D3DTSS_ALPHAOP,  D3DTOP_MODULATE);
	mDeviceD3D9->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	mDeviceD3D9->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);

	mDeviceD3D9->SetTexture(0,NULL);
	mCurrentTex = NULL;

	// #######################
	if(FAILED(D3DXCreateLine(mDeviceD3D9, &m_pLine)) )
	{
		assert(!"can't create line");
	}

	return	true;
}

bool	Direct3d9RenderSystem::_initBatching()
{
	mSizeOf4Verts = sizeof(D3D9Vertex) * 4;
	mSizeOfVert = sizeof(D3D9Vertex);

	mDeviceD3D9->CreateVertexBuffer(
		mSizeOf4Verts, 
		D3DUSAGE_WRITEONLY,
		D3DFVF_TLVERTEX,
		D3DPOOL_MANAGED,
		&mVBDynamic, 
		NULL
	);

	mFontVertices.reserve(1000);

	if (D3D_OK != mDeviceD3D9->CreateVertexBuffer(
		BATCH_BUFFER_SIZE * mSizeOfVert, 
		D3DUSAGE_WRITEONLY,
		D3DFVF_TLVERTEX,
		D3DPOOL_MANAGED,
		&mBatchVB,
		NULL
		))
	{
		return	false;
	}

	if (D3D_OK != mDeviceD3D9->CreateIndexBuffer(
		BATCH_BUFFER_SIZE * 3, 
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&mBatchIB,
		NULL
		))
	{
		return	false;
	}

	mNumBatchVertices = 0;

	int		index = 0;
	short*	indices = NULL;

	if (FAILED(mBatchIB->Lock(0, BATCH_BUFFER_SIZE * 3, (VOID**)&indices, 0)))
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

	mBatchIB->Unlock();

	mDeviceD3D9->SetVertexShader(NULL);
	mDeviceD3D9->SetFVF (D3DFVF_TLVERTEX);
	mDeviceD3D9->SetStreamSource(0, mVBDynamic, 0, mSizeOfVert);
	mDeviceD3D9->SetIndices(mBatchIB);
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
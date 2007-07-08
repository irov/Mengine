#	include "d3d9RenderSystem.h"
#	include "d3d9VertexData.h"
#	include "d3d9IndexData.h"
#	include "d3d9Texture.h"
#	include "d3d9VertexDeclaration.h"

#	include <windows.h>
#	include <assert.h>
#	include <XInput.h> 
#	include "DXUT.h"
#	include "DXUTmisc.h"
#	include "DXUTenum.h"
#	include "DXUTmesh.h"
#	include "DXUTRes.h"

//IDirect3DTexture9*		md2_texture = 0;

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

bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    IDirect3D9* pD3D = DXUTGetD3DObject(); 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;
    return true;
}

bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
    if( (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
         pCaps->VertexShaderVersion < D3DVS_VERSION(2,0) )
    {
        pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( pDeviceSettings->DeviceType == D3DDEVTYPE_REF )
            DXUTDisplaySwitchingToREFWarning();
    }

    return true;
}

HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

	Direct3d9RenderSystem* renderSystem = static_cast<Direct3d9RenderSystem*>(pUserContext);
	renderSystem->_setDevice(pd3dDevice);

//	D3DXCreateTextureFromFile(pd3dDevice, L"HLSLwithoutEffects.jpg", &md2_texture);

    return S_OK;
}

HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    /*pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
   
	D3DXMATRIX m_mProj;
	
    D3DXMatrixPerspectiveFovLH(&m_mProj, D3DX_PI/4, fAspectRatio, 0.1f, 4000.0f  );
	
	mt::mat4f	m;

	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
		{
			m[i][j] = m_mProj.m[i][j];
		}
	

	setProjectionMatrix(m);

	D3DXMATRIX mat_t;

	D3DXMatrixIdentity(&mat_t);
	D3DXMatrixTranslation(&mat_t, 0.0f, 0.0f, 5.0f);
	pd3dDevice->SetTransform(D3DTS_VIEW, &(D3DMATRIX)mat_t);

	D3DXMATRIX mat_rot, mat_user;

	D3DXMatrixRotationYawPitchRoll(&mat_rot, D3DX_PI,1.5f*D3DX_PI, 1);

	static float iii = 70.0f;
	D3DXMatrixRotationX(&mat_user, iii);
	D3DXMatrixMultiply(&mat_rot, &mat_rot, &mat_user);

	pd3dDevice->SetTransform(D3DTS_WORLD, &(D3DMATRIX)mat_rot);
*/
    return S_OK;
}

void	Direct3d9RenderSystem::setProjectionMatrix(const mt::mat4f& _projection)
{
	HRESULT hr;
	hr = m_pd3dDevice->SetTransform(D3DTS_PROJECTION,&D3DXMATRIX(_projection.m));
}

void	Direct3d9RenderSystem::setViewMatrix(const mt::mat4f& _view)
{
	HRESULT hr;
	hr = m_pd3dDevice->SetTransform(D3DTS_VIEW,&D3DXMATRIX(_view.m));
}

void	Direct3d9RenderSystem::setWorldMatrix(const mt::mat4f& _world)
{
	HRESULT hr;
	hr = m_pd3dDevice->SetTransform(D3DTS_WORLD,&D3DXMATRIX(_world.m));
}

void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;
 
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0);

    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
	//    pd3dDevice->SetTexture(0, md2_texture);

		Direct3d9RenderSystem* rs = static_cast<Direct3d9RenderSystem*>(pUserContext);
		rs->m_renderFunction();
		// if smth is 
		rs->_renderBatches();
        pd3dDevice->EndScene();
    }
}

LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
    return 0;
}

void CALLBACK OnLostDevice( void* pUserContext )
{}

void CALLBACK OnDestroyDevice( void* pUserContext )
{}

Direct3d9RenderSystem::Direct3d9RenderSystem()
	: m_pd3dDevice(0)
	, m_renderFunction(0)
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

	if (FAILED(D3DXCreateTextureFromFileInMemoryEx(m_pd3dDevice, _desc.buffer, (UINT)_desc.size,
                                    D3DX_DEFAULT, D3DX_DEFAULT,
                                    1,                  
                                    0,                  
									0 ? D3DFMT_A8R8G8B8 : D3DFMT_R8G8B8,    
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
	m_pd3dDevice->SetTexture(0,_d3d9tex->_get());
}

void	Direct3d9RenderSystem::releaseTexture(Texture * _tex)
{
	delete static_cast<d3d9Texture*>(_tex);
}

bool	Direct3d9RenderSystem::createDisplay(int _width, int _height, int _bits, bool _fullScreen)
{
	DXUTSetCallbackDeviceCreated( OnCreateDevice, this );
	DXUTSetCallbackDeviceReset( OnResetDevice );
	DXUTSetCallbackDeviceLost( OnLostDevice );
	DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
	DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackKeyboard( NULL );
	DXUTSetCallbackFrameRender( OnFrameRender, this );
	DXUTSetCallbackFrameMove( NULL );
	DXUTSetCursorSettings( true, true );
	DXUTInit( true, true, true ); 

	DXUTCreateWindow( L"Render System test" );
	DXUTCreateDevice( D3DADAPTER_DEFAULT, !_fullScreen, _width, _height, IsDeviceAcceptable, ModifyDeviceSettings );

	_initBatching();
	return true;
}

void	Direct3d9RenderSystem::update()
{
	DXUTRender3DEnvironment();
}

void	Direct3d9RenderSystem::setRenderCallback(renderFunction _rf)
{
	m_renderFunction = _rf;
}

RenderFontInterface * Direct3d9RenderSystem::loadFont(const FontDesc &_fontDesc)
{
	return new D3D9Font(this, m_pd3dDevice, _fontDesc);
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

void	Direct3d9RenderSystem::drawPrimitive(SubMesh * _pd)
{
	_renderBatches();

	HRESULT hr;

	Direct3d9VertexData * vd = static_cast<Direct3d9VertexData*>(_pd->getVertexData());

	Direct3d9IndexData * id = static_cast<Direct3d9IndexData*>(_pd->getIndexData());
			
	m_pd3dDevice->SetVertexDeclaration( vd->_getVertexDecl() );
	m_pd3dDevice->SetStreamSource(0, vd->_get(), 0, vd->getVertexSize());
	m_pd3dDevice->SetIndices(id->_get());

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

		m_pd3dDevice->DrawIndexedPrimitive( primitiveType, 
			vertexStart, 0, vertexCount, indexStart, primCount );
	}
	else
	{
		primCount = vd->getVertexCount()/3;

		m_pd3dDevice->DrawPrimitive( primitiveType,
			vd->getVertexCount(), primCount );
	} 
}

void	Direct3d9RenderSystem::_setDevice(IDirect3DDevice9 * _pd3dDevice)
{
	m_pd3dDevice = _pd3dDevice;
}

IDirect3DDevice9 * Direct3d9RenderSystem::_getDevice()
{
	return m_pd3dDevice;
}

VertexData * Direct3d9RenderSystem::createVertexData()
{
	VertexData * m_vertexData = new Direct3d9VertexData(m_pd3dDevice);
	return m_vertexData;
}

IndexData * Direct3d9RenderSystem::createIndexData()
{
	Direct3d9IndexData * m_indexData = new Direct3d9IndexData(m_pd3dDevice);
	return m_indexData;
}

VertexDeclaration * Direct3d9RenderSystem::createVertexDeclaration()
{
	d3d9VertexDeclaration * m_vertDecl = new d3d9VertexDeclaration(m_pd3dDevice);
	return m_vertDecl;
}

RenderImageInterface* Direct3d9RenderSystem::loadImage(const TextureDesc& _desc)
{
    return new D3D9RenderImage(m_pd3dDevice, _desc);
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

	m_pd3dDevice->SetVertexShader(NULL);
	m_pd3dDevice->SetFVF (D3DFVF_TLVERTEX);
	m_pd3dDevice->SetIndices(m_batchFontIB);
	
	static D3D9Vertex*	batchVertices = NULL;

	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pd3dDevice->SetStreamSource(0, m_batchFontVB, 0, m_size1Vert);

	m_batchFontVB->Lock(0, (UINT)m_vertices.size() * m_size1Vert, (VOID**)&batchVertices, 0);
	memcpy(batchVertices, &m_vertices[0], (UINT)m_vertices.size() * m_size1Vert);
	m_batchFontVB->Unlock();

	UINT startIndex = 0;

	for each( const Batch & b in m_batches )
	{
		D3D9RenderImage* imaged3d9ptype = static_cast<D3D9RenderImage*>(b.image);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, imaged3d9ptype->_isAlpha());
		m_pd3dDevice->SetTexture(0, imaged3d9ptype->_getTexPointer());

		UINT num_vertices = (UINT)b.end - (UINT)b.begin;
		UINT num_primitives = num_vertices/2;
		m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, num_vertices, startIndex, num_primitives);
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

bool	Direct3d9RenderSystem::_initBatching()
{
	m_size4Verts = sizeof(D3D9Vertex) * 4;
	m_size1Vert = sizeof(D3D9Vertex);

	if (D3D_OK != m_pd3dDevice->CreateVertexBuffer(
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

	if (D3D_OK != m_pd3dDevice->CreateIndexBuffer(
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
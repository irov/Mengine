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

	//D3DXCreateTextureFromFile(pd3dDevice, L"HLSLwithoutEffects.jpg", &md2_texture);

    return S_OK;
}

HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
   
	D3DXMATRIX m_mProj;
	
    D3DXMatrixPerspectiveFovLH(&m_mProj, D3DX_PI/4, fAspectRatio, 0.1f, 4000.0f  );
	pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_mProj);

	D3DXMATRIX mat_t;

	D3DXMatrixIdentity(&mat_t);
	D3DXMatrixTranslation(&mat_t, 0.0f, 0.0f, 165.0f);
	pd3dDevice->SetTransform(D3DTS_VIEW, &(D3DMATRIX)mat_t);

	D3DXMATRIX mat_rot, mat_user;

	D3DXMatrixRotationYawPitchRoll(&mat_rot, D3DX_PI,1.5f*D3DX_PI, 1);

	static float iii = 70.0f;
	D3DXMatrixRotationX(&mat_user, iii);
	D3DXMatrixMultiply(&mat_rot, &mat_rot, &mat_user);

	pd3dDevice->SetTransform(D3DTS_WORLD, &(D3DMATRIX)mat_rot);

    return S_OK;
}

void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;
 
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0);

    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
	    //pd3dDevice->SetTexture(0, md2_texture);
		renderFunction RenderFunction = static_cast<renderFunction>(pUserContext);
		RenderFunction();
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
{}

Texture * Direct3d9RenderSystem::createTexture()
{
	return new d3d9Texture();
}

bool	Direct3d9RenderSystem::createDisplay(int _width, int _height, int _bits, bool _fullScreen)
{
	DXUTSetCallbackDeviceCreated( OnCreateDevice, this );
	DXUTSetCallbackDeviceReset( OnResetDevice );
	DXUTSetCallbackDeviceLost( OnLostDevice );
	DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
	DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackKeyboard( NULL );
	DXUTSetCallbackFrameRender( OnFrameRender, m_renderFunction );
	DXUTSetCallbackFrameMove( NULL );
	DXUTSetCursorSettings( true, true );
	DXUTInit( true, true, true ); 

	DXUTCreateWindow( L"HLSLwithoutEffects" );
	DXUTCreateDevice( D3DADAPTER_DEFAULT, _fullScreen, _width, _height, IsDeviceAcceptable, ModifyDeviceSettings );

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

void	Direct3d9RenderSystem::drawPrimitive(const PrimitiveData& _pd)
{
	HRESULT hr;

	Direct3d9VertexData * vd = static_cast<Direct3d9VertexData*>(_pd.getVertexData());

	Direct3d9IndexData * id = static_cast<Direct3d9IndexData*>(_pd.getIndexData());
			
	m_pd3dDevice->SetVertexDeclaration( vd->_getVertexDecl() );
	m_pd3dDevice->SetStreamSource(0, vd->_get(), 0, vd->getVertexSize());
	m_pd3dDevice->SetIndices(id->_get());

	int primCount = 0;

	D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST;

	if(_pd.getRenderOp() == TRIANGLESTRIP)
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
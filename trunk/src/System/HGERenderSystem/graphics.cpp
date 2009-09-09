/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: graphics
*/

#	include "Config/Typedef.h"

#include "hge_impl.h"
#include <d3d8.h>
#include <d3dx8.h>
#include <d3d8types.h>
#	include <cassert>

D3DFORMAT s_toD3DFormat( int _format )
{
	switch( _format )
	{
	case 1:
		return D3DFMT_L8;
	case 3:
		return D3DFMT_A8;
	case 4:
		return D3DFMT_A4L4;
	case 5:
		return D3DFMT_A8L8; // Assume little endian here
	case 31:
		return D3DFMT_R3G3B2;
	case 9:
		return D3DFMT_A1R5G5B5;
	case 6:
		return D3DFMT_R5G6B5;
	case 8:
		return D3DFMT_A4R4G4B4;
	case 10:
		return D3DFMT_R8G8B8;
	case 12:
		return D3DFMT_A8R8G8B8;
	case 26:
		return D3DFMT_X8R8G8B8;
	case 34:
		return D3DFMT_G16R16;
	case 17:
		return D3DFMT_DXT1;
	case 18:
		return D3DFMT_DXT2;
	case 19:
		return D3DFMT_DXT3;
	case 20:
		return D3DFMT_DXT4;
	case 21:
		return D3DFMT_DXT5;
	case 0:
	default:
		return D3DFMT_UNKNOWN;
	}
}

void CALL HGE_Impl::Gfx_Clear( DWORD color )
{

	//pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	if( pCurTarget )
	{
		if( pCurTarget->pDepth )
		{
			pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0 );
		}
		else
		{
			pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0 );
		}
	}
	else
	{
		if( bZBuffer )
		{
			pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0 );
		}
		else
		{
			pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0 );
		}
	}
}

void CALL HGE_Impl::Gfx_SetClipping( int x, int y, int w, int h )
{
	D3DVIEWPORT8 vp;
	int scr_width, scr_height;

	/*m_viewportX = x;
	m_viewportY = y;
	m_viewportWidth = w;
	m_viewportHeight = h;*/

	if(!pCurTarget) 
	{
		scr_width=pHGE->System_GetStateInt(HGE_SCREENWIDTH);
		scr_height=pHGE->System_GetStateInt(HGE_SCREENHEIGHT);
	}
	else 
	{
		scr_width=Texture_GetWidth((HTEXTURE)pCurTarget->pTex);
		scr_height=Texture_GetHeight((HTEXTURE)pCurTarget->pTex);
	}

	if(!w) 
	{
		vp.X=0;
		vp.Y=0;
		vp.Width=scr_width;
		vp.Height=scr_height;
	}
	else
	{
		if(x<0) { w+=x; x=0; }
		if(y<0) { h+=y; y=0; }

		if(x+w > scr_width) w=scr_width-x;
		if(y+h > scr_height) h=scr_height-y;

		vp.X=x;
		vp.Y=y;
		vp.Width=w;
		vp.Height=h;
	}

	vp.MinZ=0.0f;
	vp.MaxZ=1.0f;

	_render_batch();
	HRESULT hr = pD3DDevice->SetViewport(&vp);
	if( FAILED( hr ) )
	{
		System_Log( "Error: D3D8 failed to SetViewport" );
	}

	D3DXMATRIXA16 tmp;
	D3DXMatrixScaling(&matProj, 1.0f, -1.0f, 1.0f);
	D3DXMatrixTranslation(&tmp, -0.5f, +0.5f, 0.0f);
	D3DXMatrixMultiply(&matProj, &matProj, &tmp);
	D3DXMatrixOrthoOffCenterLH(&tmp, (float)vp.X, (float)(vp.X+vp.Width), -((float)(vp.Y+vp.Height)), -((float)vp.Y), vp.MinZ, vp.MaxZ);
	D3DXMatrixMultiply(&matProj, &matProj, &tmp);
	pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

void CALL HGE_Impl::Gfx_SetTransform(float x, float y, float dx, float dy, float rot, float hscale, float vscale)
{
	D3DXMATRIXA16 tmp;

	if( vscale == 0.0f ) 
	{
		D3DXMatrixIdentity(&matView);
	}
	else
	{
		D3DXMatrixTranslation(&matView, -x, -y, 0.0f);
		D3DXMatrixScaling(&tmp, hscale, vscale, 1.0f);
		D3DXMatrixMultiply(&matView, &matView, &tmp);
		D3DXMatrixRotationZ(&tmp, -rot);
		D3DXMatrixMultiply(&matView, &matView, &tmp);
		D3DXMatrixTranslation(&tmp, x+dx, y+dy, 0.0f);
		D3DXMatrixMultiply(&matView, &matView, &tmp);
	}

	_render_batch();
	pD3DDevice->SetTransform( D3DTS_VIEW, &matView );
}

bool CALL HGE_Impl::Gfx_BeginScene( HTARGET targ )
{
	NumDips = 0;
	LPDIRECT3DSURFACE8 pSurf=0, pDepth=0;
	D3DDISPLAYMODE Mode;
	CRenderTargetList *target=(CRenderTargetList *)targ;

	HRESULT hr = pD3DDevice->TestCooperativeLevel();
	if (hr == D3DERR_DEVICELOST)
	{
		return false;
	}
	else if (hr == D3DERR_DEVICENOTRESET)
	{
		if( bWindowed )
		{
			if(FAILED(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Mode)) || Mode.Format==D3DFMT_UNKNOWN) 
			{
				_PostError( "Can't determine desktop video mode" );
				return false;
			}

			d3dppW.BackBufferFormat = Mode.Format;
			if(_format_id(Mode.Format) < 4) 
			{
				nScreenBPP=16;
			}
			else 
			{
				nScreenBPP=32;
			}
		}

	    if(!_GfxRestore()) return false; 
	}
    
	if(VertArray)
	{
		_PostError( "Gfx_BeginScene: Scene is already being rendered" );
		return false;
	}
	
	if(target != pCurTarget)
	{
		if(target)
		{
			target->pTex->GetSurfaceLevel(0, &pSurf);
			pDepth=target->pDepth;
		}
		else
		{
			pSurf=pScreenSurf;
			pDepth=pScreenDepth;
		}
		if(FAILED(pD3DDevice->SetRenderTarget(pSurf, pDepth)))
		{
			if(target) pSurf->Release();
			_PostError( "Gfx_BeginScene: Can't set render target" );
			return false;
		}
		if(target)
		{
			pSurf->Release();
			if(target->pDepth) pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE ); 
			else pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE ); 
			_SetProjectionMatrix(target->width, target->height);
		}
		else
		{
			if(bZBuffer) pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE ); 
			else pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
			_SetProjectionMatrix(nScreenWidth, nScreenHeight);
		}

		pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);
		D3DXMatrixIdentity(&matView);
		pD3DDevice->SetTransform(D3DTS_VIEW, &matView);

		pCurTarget=target;
	}

	pD3DDevice->BeginScene();
	pVB->Lock( 0, 0, (BYTE**)&VertArray, 0 );

	return true;
}

void CALL HGE_Impl::Gfx_EndScene()
{
	_render_batch(true);
	pD3DDevice->EndScene();
}

void CALL HGE_Impl::Gfx_SwapBuffers()
{
	// sync GPU with CPU
	syncCPU_();

	if( !pCurTarget ) 
	{
		HRESULT hr = pD3DDevice->Present( NULL, NULL, NULL, NULL );
		if( FAILED( hr ) )
		{
			System_Log( "Error: D3D8 failed to swap buffers" );
		}
		m_frames++;
	}
}

void CALL HGE_Impl::Gfx_RenderLine(float x1, float y1, float x2, float y2, DWORD color, float z)
{
	if(VertArray)
	{
		if(CurPrimType!=HGEPRIM_LINES || nPrim>=VERTEX_BUFFER_SIZE/HGEPRIM_LINES || CurTexture || CurBlendMode!=BLEND_DEFAULT)
		{
			_render_batch();

			CurPrimType=HGEPRIM_LINES;
			if(CurBlendMode != BLEND_DEFAULT) _SetBlendMode(BLEND_DEFAULT);
			if(CurTexture) { pD3DDevice->SetTexture(0, 0); CurTexture=0; }
		}

		int i=nPrim*HGEPRIM_LINES;
		VertArray[i].x = x1; VertArray[i+1].x = x2;
		VertArray[i].y = y1; VertArray[i+1].y = y2;
		VertArray[i].z     = VertArray[i+1].z = z;
		VertArray[i].col   = VertArray[i+1].col = color;
		VertArray[i].tx    = VertArray[i+1].tx =
		VertArray[i].ty    = VertArray[i+1].ty = 0.0f;

		nPrim++;
	}
}

void CALL HGE_Impl::Gfx_RenderTriple(const hgeTriple *triple)
{
	if(VertArray)
	{
		if(CurPrimType!=HGEPRIM_TRIPLES || nPrim>=VERTEX_BUFFER_SIZE/HGEPRIM_TRIPLES || CurTexture!=triple->tex || CurBlendMode!=triple->blend)
		{
			_render_batch();

			CurPrimType=HGEPRIM_TRIPLES;
			if(CurBlendMode != triple->blend) _SetBlendMode(triple->blend);
			if(triple->tex != CurTexture) {
				pD3DDevice->SetTexture( 0, (LPDIRECT3DTEXTURE8)triple->tex );
				CurTexture = triple->tex;
			}
		}

		memcpy(&VertArray[nPrim*HGEPRIM_TRIPLES], triple->v, sizeof(hgeVertex)*HGEPRIM_TRIPLES);
		nPrim++;
	}
}

void CALL HGE_Impl::Gfx_RenderQuad(const hgeQuad *quad)
{
	if(VertArray)
	{
		if( CurPrimType!=HGEPRIM_QUADS 
			|| nPrim>=VERTEX_BUFFER_SIZE/HGEPRIM_QUADS 
			|| CurTexture!=quad->tex 
			|| m_currSrcBlend != quad->srcBlend 
			|| m_currDstBlend != quad->dstBlend )
		{
			_render_batch();

			CurPrimType=HGEPRIM_QUADS;
			//if(CurBlendMode != quad->blend) _SetBlendMode(quad->blend);
			if( m_currSrcBlend != quad->srcBlend || m_currDstBlend != quad->dstBlend )
			{
				Gfx_SetBlendState( quad->srcBlend, quad->dstBlend );
			}
			if(quad->tex != CurTexture)
			{
				pD3DDevice->SetTexture( 0, (LPDIRECT3DTEXTURE8)quad->tex );
				CurTexture = quad->tex;
			}
		}

		memcpy(&VertArray[nPrim*HGEPRIM_QUADS], quad->v, sizeof(hgeVertex)*HGEPRIM_QUADS);
		nPrim++;
	}
}

hgeVertex* CALL HGE_Impl::Gfx_StartBatch(int prim_type, HTEXTURE tex, int blend, int *max_prim)
{
	if(VertArray)
	{
		_render_batch();

		CurPrimType=prim_type;
		if(CurBlendMode != blend) _SetBlendMode(blend);
		if(tex != CurTexture)
		{
			pD3DDevice->SetTexture( 0, (LPDIRECT3DTEXTURE8)tex );
			CurTexture = tex;
		}

		*max_prim=VERTEX_BUFFER_SIZE / prim_type;
		return VertArray;
	}
	else return 0;
}

void CALL HGE_Impl::Gfx_FinishBatch(int nprim)
{
	nPrim=nprim;
}

HTARGET CALL HGE_Impl::Target_Create(int width, int height, bool zbuffer)
{
	CRenderTargetList *pTarget;
	D3DSURFACE_DESC TDesc;

	pTarget = new CRenderTargetList;
	pTarget->pTex=0;
	pTarget->pDepth=0;

	D3DFORMAT fmt = d3dpp->BackBufferFormat;
	D3DXCheckTextureRequirements( pD3DDevice, NULL, NULL, 0, D3DUSAGE_RENDERTARGET, &fmt, D3DPOOL_DEFAULT );
	HRESULT hr = D3DXCreateTexture(pD3DDevice, width, height, 1, D3DUSAGE_RENDERTARGET,
		fmt, D3DPOOL_DEFAULT, &pTarget->pTex);
	if( FAILED( hr ) )
	{
		_PostError( "Can't create render target texture" );
		System_Log( " HR = %d", hr );
		delete pTarget;
		return 0;
	}

	pTarget->pTex->GetLevelDesc(0, &TDesc);
	pTarget->width=TDesc.Width;
	pTarget->height=TDesc.Height;

	if(zbuffer)
	{
		hr = pD3DDevice->CreateDepthStencilSurface(pTarget->width, pTarget->height,
			D3DFMT_D16, D3DMULTISAMPLE_NONE, &pTarget->pDepth);
		if( FAILED( hr ) )
		{   
			pTarget->pTex->Release();
			_PostError( "Can't create render target depth buffer" );
			delete pTarget;
			return 0;
		}
	}

	pTarget->next=pTargets;
	pTargets=pTarget;

	return (HTARGET)pTarget;
}

void CALL HGE_Impl::Target_Free(HTARGET target)
{
	CRenderTargetList *pTarget=pTargets, *pPrevTarget=NULL;

	while(pTarget)
	{
		if((CRenderTargetList *)target == pTarget)
		{
			if(pPrevTarget)
				pPrevTarget->next = pTarget->next;
			else
				pTargets = pTarget->next;

			if(pTarget->pTex) 
			{
				// dirty hack =(
				while( pTarget->pTex->Release() );
			}
			if(pTarget->pDepth) 
			{
				// dirty hack =(
				while( pTarget->pDepth->Release() );
			}

			delete pTarget;
			return;
		}

		pPrevTarget = pTarget;
		pTarget = pTarget->next;
	}
}

HTEXTURE CALL HGE_Impl::Target_GetTexture(HTARGET target)
{
	CRenderTargetList *targ=(CRenderTargetList *)target;
	if(target) return (HTEXTURE)targ->pTex;
	else return 0;
}

HTEXTURE CALL HGE_Impl::Texture_Create(int width, int height,int _format)
{
	LPDIRECT3DTEXTURE8 pTex;
	D3DFORMAT fmt = s_toD3DFormat( _format );
	if( FAILED( D3DXCreateTexture( pD3DDevice, width, height,
										1,					// Mip levels
										0,					// Usage
										fmt,				// Format
										D3DPOOL_MANAGED,	// Memory pool
										&pTex ) ) )
	{	
		_PostError( "Can't create texture" );
		return NULL;
	}

	LPDIRECT3DSURFACE8 surf;
	pTex->GetSurfaceLevel( 0, &surf );
	D3DSURFACE_DESC TDesc;
	surf->GetDesc(&TDesc);


	CTextureList *texItem;
	texItem=new CTextureList;
	texItem->tex=(HTEXTURE)pTex;
	texItem->width=width;
	texItem->height=height;
	texItem->next=textures;
	textures=texItem;

	return (HTEXTURE)pTex;
}

HTEXTURE CALL HGE_Impl::Texture_Load(const char *filename, DWORD size, bool bMipmap)
{
	void *data = 0;
	DWORD _size;
	D3DFORMAT fmt1, fmt2;
	LPDIRECT3DTEXTURE8 pTex;
	D3DXIMAGE_INFO info;
	CTextureList *texItem;

	if(size) { data=(void *)filename; _size=size; }
	else
	{
		assert( 0 && "Resource Loading not allowed" );
		//data=pHGE->Resource_Load(filename, &_size);
		if(!data) return NULL;
	}

	if(*(DWORD*)data == 0x20534444) // Compressed DDS format magic number
	{
		fmt1=D3DFMT_UNKNOWN;
		fmt2=D3DFMT_A8R8G8B8;
	}
	else
	{
		fmt1=D3DFMT_A8R8G8B8;
		fmt2=D3DFMT_UNKNOWN;
	}

//	if( FAILED( D3DXCreateTextureFromFileInMemory( pD3DDevice, data, _size, &pTex ) ) ) pTex=NULL;
	if( FAILED( D3DXCreateTextureFromFileInMemoryEx( pD3DDevice, data, _size,
										D3DX_DEFAULT, D3DX_DEFAULT,
										bMipmap ? 0:1,		// Mip levels
										0,					// Usage
										fmt1,				// Format
										D3DPOOL_MANAGED,	// Memory pool
										D3DX_FILTER_NONE,	// Filter
										D3DX_DEFAULT,		// Mip filter
										0,					// Color key
										&info, NULL,
										&pTex ) ) )

	if( FAILED( D3DXCreateTextureFromFileInMemoryEx( pD3DDevice, data, _size,
										D3DX_DEFAULT, D3DX_DEFAULT,
										bMipmap ? 0:1,		// Mip levels
										0,					// Usage
										fmt2,				// Format
										D3DPOOL_MANAGED,	// Memory pool
										D3DX_FILTER_NONE,	// Filter
										D3DX_DEFAULT,		// Mip filter
										0,					// Color key
										&info, NULL,
										&pTex ) ) )

	{	
		_PostError( "Can't create texture" );
		//if(!size) Resource_Free(data);
		return NULL;
	}

	//if(!size) Resource_Free(data);
	
	texItem=new CTextureList;
	texItem->tex=(HTEXTURE)pTex;
	texItem->width=info.Width;
	texItem->height=info.Height;
	texItem->next=textures;
	textures=texItem;

	return (HTEXTURE)pTex;
}

void CALL HGE_Impl::Texture_LoadRawData( HTEXTURE _hTex, const char* data, int _pitch, int _width, int _height, int _format )
{
	IDirect3DTexture8* tex = reinterpret_cast<IDirect3DTexture8*>( _hTex );
	LPDIRECT3DSURFACE8 surf;
	tex->GetSurfaceLevel( 0, &surf );
	D3DFORMAT fmt = s_toD3DFormat( _format );
	RECT rect;
	rect.top = 0; rect.left = 0;
	rect.right = _width; rect.bottom = _height;
	HRESULT hr = D3DXLoadSurfaceFromMemory( surf, NULL, &rect, data, fmt, _pitch, NULL, &rect, D3DX_DEFAULT, 0 );
	if( FAILED( hr ) )
	{
		_PostError( "Error: Failed to load texture data" );
		System_Log( "HR = %d", hr );
		return;
	}
	D3DSURFACE_DESC TDesc;
	if(FAILED(surf->GetDesc(&TDesc))) return;
	if( _width != TDesc.Width || _height != TDesc.Height )
	{
		D3DLOCKED_RECT lrect;
		hr = surf->LockRect( &lrect, NULL, 0 );
		if( TDesc.Width > _width )
		{
			unsigned char* tData = (unsigned char*)(lrect.pBits);
			for( int i = 0; i < _height; i++ )
			{
				std::copy( &(tData[_width*4-4]), &(tData[_width*4]), &(tData[_width*4]) );
				tData += lrect.Pitch;
			}
		}
		if( TDesc.Height > _height )
		{
			unsigned char* tData = (unsigned char*)(lrect.pBits);
			std::copy( &(tData[(_height-1)*lrect.Pitch]), &(tData[_height*lrect.Pitch]), &(tData[_height*lrect.Pitch]) );
		}
		surf->UnlockRect();
	}
}

void CALL HGE_Impl::Texture_Free(HTEXTURE tex)
{
	LPDIRECT3DTEXTURE8 pTex=(LPDIRECT3DTEXTURE8)tex;
	CTextureList *texItem=textures, *texPrev=0;

	while(texItem)
	{
		if(texItem->tex==tex)
		{
			if(texPrev) texPrev->next=texItem->next;
			else textures=texItem->next;
			delete texItem;
			break;
		}
		texPrev=texItem;
		texItem=texItem->next;
	}
	if(pTex != NULL) 
	{
		// dirty hack =(
		while( pTex->Release() );
	}
}

int CALL HGE_Impl::Texture_GetWidth(HTEXTURE tex, bool bOriginal)
{
	D3DSURFACE_DESC TDesc;
	LPDIRECT3DTEXTURE8 pTex=(LPDIRECT3DTEXTURE8)tex;
	CTextureList *texItem=textures;

	if(bOriginal)
	{
		while(texItem)
		{
			if(texItem->tex==tex) return texItem->width;
			texItem=texItem->next;
		}
		return 0;
	}
	else
	{
		if(FAILED(pTex->GetLevelDesc(0, &TDesc))) return 0;
		else return TDesc.Width;
	}
}


int CALL HGE_Impl::Texture_GetHeight(HTEXTURE tex, bool bOriginal)
{
	D3DSURFACE_DESC TDesc;
	LPDIRECT3DTEXTURE8 pTex=(LPDIRECT3DTEXTURE8)tex;
	CTextureList *texItem=textures;

	if(bOriginal)
	{
		while(texItem)
		{
			if(texItem->tex==tex) return texItem->height;
			texItem=texItem->next;
		}
		return 0;
	}
	else
	{
		if(FAILED(pTex->GetLevelDesc(0, &TDesc))) return 0;
		else return TDesc.Height;
	}
}


DWORD * CALL HGE_Impl::Texture_Lock(HTEXTURE tex, int* _pitch, bool bReadOnly, int left, int top, int width, int height)
{
	LPDIRECT3DTEXTURE8 pTex=(LPDIRECT3DTEXTURE8)tex;
	D3DSURFACE_DESC TDesc;
	D3DLOCKED_RECT TRect;
	RECT region, *prec;
	int flags;

	pTex->GetLevelDesc(0, &TDesc);
	if(TDesc.Format!=D3DFMT_A8R8G8B8 && TDesc.Format!=D3DFMT_X8R8G8B8) return 0;

	if(width && height)
	{
		region.left=left;
		region.top=top;
		region.right=left+width;
		region.bottom=top+height;
		prec=&region;
	}
	else prec=0;

	if(bReadOnly) flags=D3DLOCK_READONLY;
	else flags=0;

	if(FAILED(pTex->LockRect(0, &TRect, prec, flags)))
	{
		_PostError( "Can't lock texture" );
		return 0;
	}

	*_pitch = TRect.Pitch;
	return (DWORD *)TRect.pBits;
}


void CALL HGE_Impl::Texture_Unlock(HTEXTURE tex)
{
	LPDIRECT3DTEXTURE8 pTex=(LPDIRECT3DTEXTURE8)tex;
	pTex->UnlockRect(0);
}

//////// Implementation ////////

int	CALL HGE_Impl::GetNumDips()
{
	return NumDips;
}

void HGE_Impl::_render_batch( bool bEndScene )
{
	HRESULT hr;
	if(VertArray)
	{
		pVB->Unlock();
		
		if(nPrim)
		{
			switch(CurPrimType)
			{
				case HGEPRIM_QUADS:
					hr = pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, nPrim<<2, 0, nPrim<<1);
					NumDips++;
					if( FAILED( hr ) )
					{
						System_Log( "ERROR!!!" );
					}
					break;

				case HGEPRIM_TRIPLES:
					//������� ��� ������ �����
					//DWORD state;
					//pD3DDevice->GetRenderState(D3DRS_WRAP0,&state);
					//pD3DDevice->SetRenderState(D3DRS_WRAP0,D3DWRAP_U | D3DWRAP_V);
					pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
					pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
					//
					pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, nPrim);

					pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
					pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

					//
					//pD3DDevice->SetRenderState(D3DRS_WRAP0,state);
					break;

				case HGEPRIM_LINES:
					pD3DDevice->DrawPrimitive(D3DPT_LINELIST, 0, nPrim);
					break;
			}

			nPrim=0;
		}

		if( bEndScene )
		{
			VertArray = 0;
		}
		else 
		{
			pVB->Lock( 0, 0, (BYTE**)&VertArray, 0 );
		}
	}
}

void HGE_Impl::_SetBlendMode(int blend)
{
	if((blend & BLEND_ALPHABLEND) != (CurBlendMode & BLEND_ALPHABLEND))
	{
		if(blend & BLEND_ALPHABLEND) pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		else pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}

    if((blend & BLEND_ZWRITE) != (CurBlendMode & BLEND_ZWRITE))
	{
		if(blend & BLEND_ZWRITE) pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		else pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}			
	
	if((blend & BLEND_COLORADD) != (CurBlendMode & BLEND_COLORADD))
	{
		if(blend & BLEND_COLORADD) pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
		else pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	}

	CurBlendMode = blend;
}

void HGE_Impl::_SetProjectionMatrix(int width, int height)
{
	D3DXMATRIXA16 tmp;
	D3DXMatrixScaling(&matProj, 1.0f, -1.0f, 1.0f);
	D3DXMatrixTranslation(&tmp, -0.5f, height+0.5f, 0.0f);
	D3DXMatrixMultiply(&matProj, &matProj, &tmp);
	D3DXMatrixOrthoOffCenterLH(&tmp, 0, (float)width, 0, (float)height, 0.0f, 1.0f);
	D3DXMatrixMultiply(&matProj, &matProj, &tmp);
}

bool HGE_Impl::_GfxInit()
{
	D3DADAPTER_IDENTIFIER8 AdID;
	D3DDISPLAYMODE Mode;
	D3DFORMAT Format=D3DFMT_UNKNOWN;
	UINT nModes, i;
	m_gfxListener = 0;
	m_frames = 0;

	m_syncTargets[0] = 0;
	m_syncTargets[1] = 0;
	m_syncTemp = 0;
	m_syncTempTex = 0;
	pIB3D = 0;
	pVB3D = 0;
	CurTexture = 0;
// Init D3D
							
	pD3D=Direct3DCreate8( D3D_SDK_VERSION ); // D3D_SDK_VERSION
	if( pD3D == NULL )
	{
		_PostError( "Can't create D3D interface" );
		return false;
	}

// Get adapter info

	pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, D3DENUM_NO_WHQL_LEVEL, &AdID);
	System_Log( "D3D Driver: %s", AdID.Driver );
	System_Log( "Description: %s", AdID.Description );
	System_Log( "Version: %d.%d.%d.%d",
			HIWORD(AdID.DriverVersion.HighPart),
			LOWORD(AdID.DriverVersion.HighPart),
			HIWORD(AdID.DriverVersion.LowPart),
			LOWORD(AdID.DriverVersion.LowPart));

// Set up Windowed presentation parameters
	
	if(FAILED(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Mode)) || Mode.Format==D3DFMT_UNKNOWN) 
	{
		_PostError( "Can't determine desktop video mode" );
		if(bWindowed) return false;
	}
	
	ZeroMemory(&d3dppW, sizeof(d3dppW));
	d3dppW.BackBufferFormat = Mode.Format;
	d3dppW.MultiSampleType  = D3DMULTISAMPLE_NONE;
	d3dppW.Windowed         = TRUE;
	d3dppW.Flags			= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3dppW.BackBufferCount  = 1;


// Set up Full Screen presentation parameters

	nModes=pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT);

	for(i=0; i<nModes; i++)
	{
		pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, i, &Mode);
		m_displayModes.push_back( Mode );
		m_resList.push_back( Mode.Width );
		m_resList.push_back( Mode.Height );
	}

	for(i=0; i<nModes; i++)
	{
		Mode = m_displayModes[i];
		if(Mode.Width != (UINT)nScreenWidth || Mode.Height != (UINT)nScreenHeight) continue;
		if(nScreenBPP==16 && (_format_id(Mode.Format) > _format_id(D3DFMT_A1R5G5B5))) continue;
		if(_format_id(Mode.Format) > _format_id(Format)) Format=Mode.Format;
	}

	if(Format == D3DFMT_UNKNOWN)
	{
		_PostError( "Can't find appropriate full screen video mode" );
		if(!bWindowed) return false;
	}

	ZeroMemory(&d3dppFS, sizeof(d3dppFS));
	d3dppFS.BackBufferFormat = Format;
	d3dppFS.BackBufferCount  = 1;
	d3dppFS.MultiSampleType  = D3DMULTISAMPLE_NONE;
	d3dppFS.Windowed         = FALSE;
	d3dppFS.SwapEffect       = D3DSWAPEFFECT_FLIP;
	d3dppFS.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dppFS.Flags			 = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	D3DXMatrixIdentity( &matView );
	D3DXMatrixIdentity( &matWorld );

	D3DCAPS8 caps;
	pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps );
	if( ( caps.TextureCaps & D3DPTEXTURECAPS_POW2 ) == 0 )
	{
		m_supportNPOT = true;
	}
	return true;
}

bool HGE_Impl::Gfx_CreateRenderWindow()
{
	static const char *szFormats[]={ 
		"UNKNOWN"
		,"R5G6B5"
		,"X1R5G5B5"
		,"A1R5G5B5"
		,"X8R8G8B8"
		,"A8R8G8B8"
	};

	d3dppW.BackBufferWidth  = nScreenWidth;
	d3dppW.BackBufferHeight = nScreenHeight;
	d3dppW.hDeviceWindow    = hwnd;
	//nHGEFPS = HGEFPS_VSYNC;
	if(nHGEFPS==HGEFPS_VSYNC) d3dppW.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
	else					  d3dppW.SwapEffect = D3DSWAPEFFECT_COPY;

	if(bZBuffer)
	{
		d3dppW.EnableAutoDepthStencil = TRUE;
		d3dppW.AutoDepthStencilFormat = D3DFMT_D16;
	}

	d3dppFS.BackBufferWidth  = nScreenWidth;
	d3dppFS.BackBufferHeight = nScreenHeight;
	d3dppFS.hDeviceWindow    = hwnd;


	if(nHGEFPS==HGEFPS_VSYNC) d3dppFS.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	else					  d3dppFS.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if(bZBuffer)
	{
		d3dppFS.EnableAutoDepthStencil = TRUE;
		d3dppFS.AutoDepthStencilFormat = D3DFMT_D16;
	}

	d3dpp = bWindowed ? &d3dppW : &d3dppFS;

	if(_format_id(d3dpp->BackBufferFormat) < 4) nScreenBPP=16;
	else nScreenBPP=32;

	// Create D3D Device
	HRESULT hr;

	hr = pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		d3dpp, &pD3DDevice );

	if( FAILED( hr ) )
	{
		hr = pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			d3dpp, &pD3DDevice );
	}

	if( FAILED( hr ) )
	{
		hr = pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
			D3DCREATE_MIXED_VERTEXPROCESSING,
			d3dpp, &pD3DDevice );

		if( FAILED( hr ) )
		{
			hr = pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				d3dpp, &pD3DDevice );
		}
	}

	if( FAILED ( hr ) )
	{
		System_Log( "Can't create D3D device (hr:%d, hwnd:%d)", hr, hwnd );
		return false;
	}
	/*if( FAILED( pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
	D3DCREATE_SOFTWARE_VERTEXPROCESSING,
	d3dpp, &pD3DDevice ) ) )
	{
	_PostError("Can't create D3D device");
	return false;
	}*/

	//_AdjustWindow();

	System_Log( "Mode: %d x %d x %s\n",nScreenWidth,nScreenHeight,szFormats[_format_id(d3dpp->BackBufferFormat)]);

	// Create vertex batch buffer

	VertArray = 0;
	textures = 0;

	// Init all stuff that can be lost

	_SetProjectionMatrix(nScreenWidth, nScreenHeight);
	D3DXMatrixIdentity(&matView);

	m_layer3D = false;	// starting with 2D
	if(!_init_lost()) 
	{
		return false;
	}

	Gfx_Clear(0);

	return true;
}

int HGE_Impl::_format_id(D3DFORMAT fmt)
{
	switch(fmt) {
		case D3DFMT_R5G6B5:		return 1;
		case D3DFMT_X1R5G5B5:	return 2;
		case D3DFMT_A1R5G5B5:	return 3;
		case D3DFMT_X8R8G8B8:	return 4;
		case D3DFMT_A8R8G8B8:	return 5;
		default:				return 0;
	}
}

/*void HGE_Impl::_AdjustWindow()
{
	RECT *rc;
	LONG style;

	if(bWindowed) {rc=&rectW; style=styleW; }
	else  {rc=&rectFS; style=styleFS; }
	SetWindowLong(hwnd, GWL_STYLE, style);

	style=GetWindowLong(hwnd, GWL_EXSTYLE);
	if(bWindowed)
	{
		SetWindowLong(hwnd, GWL_EXSTYLE, style & (~WS_EX_TOPMOST));
	    SetWindowPos(hwnd, HWND_NOTOPMOST, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, SWP_FRAMECHANGED);
	}
	else
	{
		SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_TOPMOST);
	    SetWindowPos(hwnd, HWND_TOPMOST, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, SWP_FRAMECHANGED);
	}
}*/

void HGE_Impl::_Resize(int width, int height)
{
	if(hwndParent)
	{
		//if(procFocusLostFunc) procFocusLostFunc();

		d3dppW.BackBufferWidth=width;
		d3dppW.BackBufferHeight=height;
		nScreenWidth=width;
		nScreenHeight=height;

		_SetProjectionMatrix(nScreenWidth, nScreenHeight);
		_GfxRestore();

		//if(procFocusGainFunc) procFocusGainFunc();
	}
}

void HGE_Impl::_GfxDone()
{

	if( m_syncTargets[0] )
	{
		m_syncTargets[0]->Release();
		m_syncTargets[0] = 0;
	}
	if( m_syncTargets[1] )
	{
		m_syncTargets[1]->Release();
		m_syncTargets[1] = 0;
	}
	if( m_syncTemp )
	{
		m_syncTemp->Release();
		m_syncTemp = 0;
	}
	if( m_syncTempTex )
	{
		m_syncTempTex->Release();
		m_syncTempTex = 0;
	}

	CRenderTargetList *target=pTargets, *next_target;
	
	while(textures)	Texture_Free(textures->tex);

	if(pScreenSurf) 
	{ 
		pScreenSurf->Release();
		pScreenSurf=0; 
	}
	if(pScreenDepth) 
	{ 
		pScreenDepth->Release();
		pScreenDepth=0; 
	}

	while(target)
	{
		if(target->pTex) target->pTex->Release();
		if(target->pDepth) target->pDepth->Release();
		next_target=target->next;
		delete target;
		target=next_target;
	}
	pTargets=0;
	
	if( pD3DDevice )
	{
		pD3DDevice->SetStreamSource( 0, NULL, sizeof(hgeVertex) );
		pD3DDevice->SetIndices(NULL,0);
	}

	if( pIB )
	{
		pIB->Release();
		pIB = 0;
	}
	if( pIB3D )
	{
		pIB3D->Release();
		pIB3D = 0;
	}
	if( pVB )
	{
		if( VertArray ) 
		{	
			pVB->Unlock(); 
			VertArray = 0;
		}
		pVB->Release();
		pVB = 0;
	}
	if( pVB3D )
	{
		pVB3D->Release();
		pVB3D = 0;
	}
	if(pD3DDevice) 
	{ 
		pD3DDevice->Release(); 
		pD3DDevice=0; 
	}
	if(pD3D) 
	{ 
		pD3D->Release();
		pD3D=0; 
	}
}


bool HGE_Impl::_GfxRestore()
{
	CRenderTargetList *target=pTargets;

	//if(!pD3DDevice) return false;
	//if(pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICELOST) return;
	if( m_syncTargets[0] )
	{
		m_syncTargets[0]->Release();
		m_syncTargets[0] = 0;
	}
	if( m_syncTargets[1] )
	{
		m_syncTargets[1]->Release();
		m_syncTargets[1] = 0;
	}
	if( m_syncTemp )
	{
		m_syncTemp->Release();
		m_syncTemp = 0;
	}
	if( m_syncTempTex )
	{
		m_syncTempTex->Release();
		m_syncTempTex = 0;
	}
	if(pScreenSurf) pScreenSurf->Release();
	if(pScreenDepth) pScreenDepth->Release();

	while(target)
	{
		if(target->pTex) target->pTex->Release();
		if(target->pDepth) target->pDepth->Release();
		target=target->next;
	}

	if( pIB || pIB3D )
	{
		pD3DDevice->SetIndices(NULL,0);
	}

	if( pVB || pVB3D )
	{
		pD3DDevice->SetStreamSource( 0, NULL, sizeof(hgeVertex) );
	}

	if(pIB)
	{
		pIB->Release();
	}
	if(pVB)
	{
		pVB->Release();
	}
	if(pIB3D)
	{
		pIB3D->Release();
	}
	if(pVB3D)
	{
		pVB3D->Release();
	}

	HRESULT hr = pD3DDevice->Reset(d3dpp);
	if( FAILED( hr ) )
	{
		return false;
	}

	if(!_init_lost()) return false;

	//if(procGfxRestoreFunc) return procGfxRestoreFunc();
	if( m_gfxListener ) m_gfxListener->onRestoreDevice();

	return true;
}


bool HGE_Impl::_init_lost()
{
	CRenderTargetList *target=pTargets;

// Store render target

	pScreenSurf=0;
	pScreenDepth=0;

	pD3DDevice->GetRenderTarget(&pScreenSurf);
	pD3DDevice->GetDepthStencilSurface(&pScreenDepth);
	
	while(target)
	{
		if(target->pTex)
		{
			D3DXCreateTexture(pD3DDevice, target->width, target->height, 1, D3DUSAGE_RENDERTARGET,
							  d3dpp->BackBufferFormat, D3DPOOL_DEFAULT, &target->pTex);
		}
		if(target->pDepth)
		{
			pD3DDevice->CreateDepthStencilSurface(target->width, target->height,
												  D3DFMT_D16, D3DMULTISAMPLE_NONE, &target->pDepth);
		}
		target=target->next;
	}

	// sync surfaces
	D3DFORMAT fmt = D3DFMT_X8R8G8B8;
	UINT w = 2;
	UINT d = 1;
	D3DXCheckTextureRequirements( pD3DDevice, &w, &w, &d, D3DUSAGE_RENDERTARGET, &fmt, D3DPOOL_DEFAULT );
	pD3DDevice->CreateRenderTarget( w, w, fmt, D3DMULTISAMPLE_NONE, TRUE, &(m_syncTargets[0]) );
	pD3DDevice->CreateRenderTarget( w, w, fmt, D3DMULTISAMPLE_NONE, TRUE, &(m_syncTargets[1]) );
	D3DXCreateTexture( pD3DDevice, w, w, d, 0, fmt, D3DPOOL_SYSTEMMEM, &m_syncTempTex );
	m_syncTempTex->GetSurfaceLevel( 0, &m_syncTemp );

// Create Vertex buffer
	
	if( FAILED (pD3DDevice->CreateVertexBuffer(VERTEX_BUFFER_SIZE*sizeof(hgeVertex),
                                              D3DUSAGE_WRITEONLY,
											  D3DFVF_HGEVERTEX,
                                              D3DPOOL_DEFAULT, &pVB )))
	{
		_PostError( "Can't create D3D vertex buffer" );
		return false;
	}

	if( FAILED (pD3DDevice->CreateVertexBuffer(VERTEX_BUFFER_SIZE*sizeof(mengeVertex),
		D3DUSAGE_WRITEONLY,
		D3DFVF_MENGEVERTEX,
		D3DPOOL_DEFAULT, &pVB3D )))
	{
		_PostError( "Can't create Menge D3D vertex buffer" );
		return false;
	}

	//pD3DDevice->SetVertexShader( D3DFVF_HGEVERTEX );
	//pD3DDevice->SetStreamSource( 0, pVB, sizeof(hgeVertex) );

// Create and setup Index buffer

	if( FAILED( pD3DDevice->CreateIndexBuffer(VERTEX_BUFFER_SIZE*6/4*sizeof(WORD),
                                              D3DUSAGE_WRITEONLY,
											  D3DFMT_INDEX16,
                                              D3DPOOL_DEFAULT, &pIB ) ) )
	{
		_PostError( "Can't create D3D index buffer" );
		return false;
	}

	WORD *pIndices, n=0;
	if( FAILED( pIB->Lock( 0, 0, (BYTE**)&pIndices, 0 ) ) )
	{
		_PostError( "Can't lock D3D index buffer" );
		return false;
	}

	for(int i=0; i<VERTEX_BUFFER_SIZE/4; i++) {
		*pIndices++=n;
		*pIndices++=n+1;
		*pIndices++=n+2;
		*pIndices++=n+2;
		*pIndices++=n+3;
		*pIndices++=n;
		n+=4;
	}

	pIB->Unlock();
	//pD3DDevice->SetIndices(pIB,0);

	if( FAILED( pD3DDevice->CreateIndexBuffer(VERTEX_BUFFER_SIZE*sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_DEFAULT, &pIB3D ) ) )
	{
		_PostError( "Can't create Menge D3D index buffer" );
		return false;
	}

// Set common render states

	//pD3DDevice->SetRenderState( D3DRS_LASTPIXEL, FALSE );
	/*pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	
	pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pD3DDevice->SetRenderState( D3DRS_ALPHAREF,        0x01 );
	pD3DDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

	pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);

	if(bTextureFilter)
	{
		pD3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
		pD3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
	}
	else
	{
		pD3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_POINT);
		pD3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_POINT);
	}

	nPrim=0;
	CurPrimType=HGEPRIM_QUADS;
	CurBlendMode = BLEND_DEFAULT;
	CurTexture = NULL;

	pD3DDevice->SetTransform(D3DTS_VIEW, &matView);
	pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);*/

	if( m_layer3D )
	{
		Gfx_Prepare3D();
	}
	else
	{
		Gfx_Prepare2D();
	}

	return true;
}

void HGE_Impl::Gfx_Snapshot( HTEXTURE tex, RECT _rect )
{
	LPDIRECT3DSURFACE8 surf;
	HRESULT hr = pD3DDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &surf );
	if( FAILED( hr ) )
	{
		System_Log( "D3D Error: failed to GetBackBuffer" );
	}

	IDirect3DTexture8* dtext = reinterpret_cast<IDirect3DTexture8*>( tex );
	LPDIRECT3DSURFACE8 dsurf;
	D3DSURFACE_DESC desc;
	dtext->GetLevelDesc(0, &desc);
	hr = dtext->GetSurfaceLevel(0, &dsurf );
	if( FAILED( hr ) )
	{
		System_Log( "D3D Error: failed to GetSurfaceLevel" );
	}

	RECT dest_rect;
	dest_rect.top = 0;
	dest_rect.left = 0;
	dest_rect.right = _rect.right - _rect.left;
	dest_rect.bottom = _rect.bottom - _rect.top;
	if( dest_rect.right > desc.Width )
	{
		dest_rect.right = desc.Width;
	}
	if( dest_rect.bottom > desc.Height )
	{
		dest_rect.bottom = desc.Height;
	}

	hr = D3DXLoadSurfaceFromSurface( dsurf, NULL, &dest_rect, surf, NULL, &_rect, D3DX_DEFAULT, 0 );
	if( FAILED( hr ) )
	{
		System_Log( "D3D Error: failed to D3DXLoadSurfaceFromSurface" );
	}
	surf->Release();
}

void HGE_Impl::Gfx_ChangeMode( int _width, int _height, int _bpp, bool _fullscreen )
{
	bWindowed = !_fullscreen;
	d3dpp = _fullscreen ? &d3dppFS : &d3dppW;
	d3dpp->BackBufferWidth = _width;
	d3dpp->BackBufferHeight = _height;

	nScreenWidth = _width;
	nScreenHeight = _height;
	nScreenBPP = _bpp;

	if( !_GfxRestore() )
	{
		System_Log( "Error: Graphics change mode failed\n" );
	}
}

const std::vector<int>& HGE_Impl::Gfx_GetModeList()
{
	return m_resList;
}

void HGE_Impl::Gfx_SetProjectionMatrix( const float* _projMat )
{
	_render_batch( false );

	std::copy( _projMat, _projMat + 16, &(matProj._11) );
	pD3DDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}

void HGE_Impl::Gfx_SetViewMatrix( const float* _viewMat )
{
	_render_batch( false );

	std::copy( _viewMat, _viewMat + 16, &(matView._11) );
	pD3DDevice->SetTransform( D3DTS_VIEW, &matView );
}

void HGE_Impl::Gfx_SetWorldMatrix( const float* _worldMat )
{
	_render_batch( false );

	std::copy( _worldMat, _worldMat + 16, &(matWorld._11) );
	pD3DDevice->SetTransform( D3DTS_WORLD, &matWorld );
}

void HGE_Impl::Gfx_Prepare2D() 
{
	_render_batch( false );

	pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );

	pD3DDevice->SetVertexShader( D3DFVF_HGEVERTEX );
	pD3DDevice->SetStreamSource( 0, pVB, sizeof(hgeVertex) );
	pD3DDevice->SetIndices(pIB,0);

	///
	pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	//Gfx_SetBlendState( BLEND_SRCALPHA, BLEND_INVSRCALPHA );

	pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pD3DDevice->SetRenderState( D3DRS_ALPHAREF,        0x01 );
	pD3DDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

	pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);

	pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
	pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
	//pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_BORDER );
	//pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_BORDER );

	if(bTextureFilter)
	{
		pD3DDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	}
	else
	{
		pD3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_POINT);
		pD3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_POINT);
	}

	nPrim=0;
	CurPrimType=HGEPRIM_QUADS;
	CurBlendMode = BLEND_DEFAULT;
	//CurTexture = NULL;

	//D3DXMatrixIdentity( &matView );
	D3DXMatrixIdentity( &matWorld );
	//_SetProjectionMatrix( nScreenWidth, nScreenHeight );
	pD3DDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	pD3DDevice->SetTransform( D3DTS_VIEW, &matView );
	pD3DDevice->SetTransform( D3DTS_WORLD, &matWorld );

	m_layer3D = false;
}

void HGE_Impl::Gfx_Prepare3D()
{
	_render_batch( false );

	pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

	pD3DDevice->SetStreamSource( 0, pVB3D, sizeof(mengeVertex) );
	pD3DDevice->SetVertexShader( D3DFVF_MENGEVERTEX );
	pD3DDevice->SetIndices( pIB3D, 0 );
	pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );

	pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
	pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

	m_layer3D = true;
}

void HGE_Impl::Gfx_RenderMesh( const mengeVertex* _vertices, size_t _verticesNum,
							   const unsigned short* _indices, size_t _indicesNum,
							   HTEXTURE _htex )
{
	HRESULT hr;
	BYTE* vertexData = 0;
	const BYTE* dstData = reinterpret_cast<const BYTE*>( &(_vertices[0]) );
	hr = pVB3D->Lock( 0, 0, &vertexData, 0 );
	std::copy( _vertices, _vertices + _verticesNum, (mengeVertex *)vertexData );
	hr = pVB3D->Unlock();

	//const BYTE* dstIData = reinterpret_cast<const BYTE*>( &(_indices[0]) );
	hr = pIB3D->Lock( 0, 0, &vertexData, 0 );
	std::copy( _indices, _indices + _indicesNum, (unsigned short *)vertexData );
	hr = pIB3D->Unlock();

	hr = pD3DDevice->SetTexture( 0, reinterpret_cast<IDirect3DTexture8*>( _htex ) );
	CurTexture = _htex;

	hr = pD3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, _verticesNum, 0, _indicesNum / 3 );
}

void HGE_Impl::Gfx_SetTextureMatrix( const float* _texMat )
{
	std::copy( _texMat, _texMat + 16, &(matTexture._11) );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	pD3DDevice->SetTransform( D3DTS_TEXTURE0, &matTexture );
}

void HGE_Impl::Gfx_SetBlendState( hgeBlendState _srcBlend, hgeBlendState _dstBlend )
{
	if( m_currSrcBlend != _srcBlend )
	{
		pD3DDevice->SetRenderState( D3DRS_SRCBLEND, static_cast<D3DBLEND>( _srcBlend ) );
		m_currSrcBlend = _srcBlend;
	}
	if( m_currDstBlend != _dstBlend )
	{
		pD3DDevice->SetRenderState( D3DRS_DESTBLEND, static_cast<D3DBLEND>( _dstBlend ) );
		m_currDstBlend = _dstBlend;
	}
}

void HGE_Impl::Gfx_SetListener( Gfx_Listener* _listener )
{
	m_gfxListener = _listener;
}

void HGE_Impl::syncCPU_()
{
	//_render_batch( false );
	pD3DDevice->BeginScene();

	pD3DDevice->SetRenderTarget( m_syncTargets[m_frames % 2], 0 );
	//m_syncTargets[m_frames % 2]->Release();
	_SetProjectionMatrix( 2, 2 );
	pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);
	pD3DDevice->SetTransform(D3DTS_VIEW, &matView);

	hgeQuad quad;
	quad.srcBlend = BLEND_SRCALPHA;
	quad.dstBlend = BLEND_INVSRCALPHA;
	quad.tex = 0;
	quad.v[0].col = 0;
	quad.v[0].x = 0;
	quad.v[0].y = 0;
	quad.v[0].z = 0;
	quad.v[1].col = 0;
	quad.v[1].x = 2;
	quad.v[1].y = 0;
	quad.v[1].z = 0;
	quad.v[2].col = 0;
	quad.v[2].x = 2;
	quad.v[2].y = 2;
	quad.v[2].z = 0;
	quad.v[3].col = 0;
	quad.v[3].x = 0;
	quad.v[3].y = 2;
	quad.v[3].z = 0;
	Gfx_RenderQuad( &quad );
	_render_batch( false );

	if( pCurTarget )
	{
		LPDIRECT3DSURFACE8 surf;
		pCurTarget->pTex->GetSurfaceLevel( 0, &surf );
		pD3DDevice->SetRenderTarget( surf, pCurTarget->pDepth );
		surf->Release();
		_SetProjectionMatrix( pCurTarget->width, pCurTarget->height );
	}
	else
	{
		pD3DDevice->SetRenderTarget( pScreenSurf, pScreenDepth );
		_SetProjectionMatrix( nScreenWidth, nScreenHeight );
	}

	pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);
	pD3DDevice->SetTransform(D3DTS_VIEW, &matView);
	
	HRESULT hr = D3DXLoadSurfaceFromSurface( m_syncTemp, NULL, NULL, m_syncTargets[(m_frames + 1) % 2], NULL, NULL, D3DX_DEFAULT, 0 );
	D3DLOCKED_RECT rect;
	hr = m_syncTemp->LockRect( &rect, NULL, D3DLOCK_READONLY );
	m_syncTemp->UnlockRect();

	pD3DDevice->EndScene();
}

bool HGE_Impl::Gfx_SupportNPOT() const
{
	return m_supportNPOT;
}

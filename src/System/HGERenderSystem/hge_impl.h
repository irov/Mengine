/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Common core implementation header
*/


#ifndef HGE_IMPL_H
#define HGE_IMPL_H

#include "hge.h"
#include <stdio.h>
#include <d3d8.h>
#include <d3dx8.h>

#	include <vector>

#define D3DFVF_HGEVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define D3DFVF_MENGEVERTEX ( D3DFVF_HGEVERTEX | D3DFVF_NORMAL )
#define VERTEX_BUFFER_SIZE 10000

struct CRenderTargetList
{
	int					width;
	int					height;
	IDirect3DTexture8*	pTex;
	IDirect3DSurface8*	pDepth;
	CRenderTargetList*	next;
};

struct CTextureList
{
	HTEXTURE			tex;
	int					width;
	int					height;
	CTextureList*		next;
};

struct CResourceList
{
	char				filename[_MAX_PATH];
	char				password[64];
	CResourceList*		next;
};

/*
** HGE Interface implementation
*/
class HGE_Impl : public HGE
{
public:
	virtual int			CALL    GetNumDips();
	virtual	void		CALL	Release();

	virtual bool		CALL	System_Initiate( Menge::LogSystemInterface* _logSystem );
	virtual void		CALL	System_SetStateBool  (hgeBoolState   state, bool        value);
	virtual void		CALL	System_SetStateHwnd  (hgeHwndState   state, HWND        value);
	virtual void		CALL	System_SetStateInt   (hgeIntState    state, int         value);
	virtual bool		CALL	System_GetStateBool  (hgeBoolState  );
	virtual HWND		CALL	System_GetStateHwnd  (hgeHwndState  );
	virtual int			CALL	System_GetStateInt   (hgeIntState   );
	virtual Menge::TCharA*		CALL	System_GetErrorMessage();
	virtual	void		CALL	System_Log( const Menge::TCharA *format, ... );
	virtual void		CALL	System_Snapshot(const char *filename=0);

	virtual bool		CALL	Gfx_CreateRenderWindow();

	virtual bool		CALL	Gfx_BeginScene(HTARGET target=0);
	virtual void		CALL	Gfx_EndScene();
	virtual void		CALL	Gfx_SwapBuffers();
	virtual void		CALL	Gfx_Clear(DWORD color);
	virtual void		CALL	Gfx_RenderLine(float x1, float y1, float x2, float y2, DWORD color=0xFFFFFFFF, float z=0.5f);
	virtual void		CALL	Gfx_RenderTriple(const hgeTriple *triple);
	virtual void		CALL	Gfx_RenderQuad(const hgeQuad *quad);
	virtual hgeVertex*	CALL	Gfx_StartBatch(int prim_type, HTEXTURE tex, int blend, int *max_prim);
	virtual void		CALL	Gfx_FinishBatch(int nprim);
	virtual void		CALL	Gfx_SetClipping(int x=0, int y=0, int w=0, int h=0);
	virtual void		CALL	Gfx_SetTransform(float x=0, float y=0, float dx=0, float dy=0, float rot=0, float hscale=0, float vscale=0); 
	virtual void		CALL	Gfx_RenderMesh( const mengeVertex* _vertices, size_t _verticesNum,
												const unsigned short* _indices, size_t _indicesNum,
												HTEXTURE _htex );

	virtual void		CALL	Gfx_Snapshot( HTEXTURE tex, RECT _rect );
	virtual void		CALL	Gfx_ChangeMode( int _width, int _height, int _bpp, bool _fullscreen );
	virtual const std::vector<int>& CALL	Gfx_GetModeList();
	virtual void		CALL	Gfx_SetProjectionMatrix( const float* _projMat );
	virtual void		CALL	Gfx_SetViewMatrix( const float* _viewMat );
	virtual void		CALL	Gfx_SetWorldMatrix( const float* _worldMat );
	virtual void		CALL	Gfx_SetTextureMatrix( const float* _texMat );
	virtual void		CALL	Gfx_Prepare2D();
	virtual void		CALL	Gfx_Prepare3D();
	virtual void		CALL	Gfx_SetBlendState( hgeBlendState _srcBlend, hgeBlendState _dstBlend );
	virtual void		CALL	Gfx_SetListener( Gfx_Listener* _listener );
	virtual bool		CALL	Gfx_SupportNPOT() const;

	virtual HTARGET		CALL	Target_Create(int width, int height, bool zbuffer);
	virtual void		CALL	Target_Free(HTARGET target);
	virtual HTEXTURE	CALL	Target_GetTexture(HTARGET target);

	virtual HTEXTURE	CALL	Texture_Create(int width, int height, int _format);
	virtual HTEXTURE	CALL	Texture_Load( const char *filename, DWORD size = 0, bool bMipmap = false );
	virtual void		CALL	Texture_LoadRawData( HTEXTURE _hTex, const char* data, int _pitch, int _width, int _height, int _format );
	virtual void		CALL	Texture_Free(HTEXTURE tex);
	virtual int			CALL	Texture_GetWidth(HTEXTURE tex, bool bOriginal=false);
	virtual int			CALL	Texture_GetHeight(HTEXTURE tex, bool bOriginal=false);
	virtual DWORD*		CALL	Texture_Lock(HTEXTURE tex, int* _pitch, bool bReadOnly=true, int left=0, int top=0, int width=0, int height=0);
	virtual void		CALL	Texture_Unlock(HTEXTURE tex);

	//////// Implementation ////////

	static HGE_Impl*	_Interface_Get();
	void				_PostError( Menge::TCharA *error );

	HWND				hwnd;
	Menge::TCharA		szError[256];

	// System States
	int					nScreenWidth;
	int					nScreenHeight;
	int					nScreenBPP;
	bool				bWindowed;
	bool				bZBuffer;
	bool				bTextureFilter;
	int					nHGEFPS;
	HWND				hwndParent;

	// Log
	Menge::LogSystemInterface* m_logSystem;

	// Graphics
	D3DPRESENT_PARAMETERS*  d3dpp;

	D3DPRESENT_PARAMETERS   d3dppW;
	RECT					rectW;
	LONG					styleW;

	D3DPRESENT_PARAMETERS   d3dppFS;
	RECT					rectFS;
	LONG					styleFS;

	IDirect3D8*				pD3D;
	IDirect3DDevice8*		pD3DDevice;
	IDirect3DVertexBuffer8*	pVB;
	IDirect3DIndexBuffer8*	pIB;
	IDirect3DVertexBuffer8*	pVB3D;
	IDirect3DIndexBuffer8*	pIB3D;

	IDirect3DSurface8*	pScreenSurf;
	IDirect3DSurface8*	pScreenDepth;
	CRenderTargetList*	pTargets;
	CRenderTargetList*	pCurTarget;

	D3DXMATRIXA16		matView;
	D3DXMATRIXA16		matProj;
	D3DXMATRIXA16		matWorld;
	D3DXMATRIXA16		matTexture;

	CTextureList*		textures;
	hgeVertex*			VertArray;

	int					nPrim;
	int					CurPrimType;
	int					CurBlendMode;
	HTEXTURE			CurTexture;

	bool				_GfxInit();
	void				_GfxDone();
	bool				_GfxRestore();
	void				_Resize(int width, int height);
	bool				_init_lost();
	void				_render_batch(bool bEndScene=false);
	int					_format_id(D3DFORMAT fmt);
	void				_SetBlendMode(int blend);
	void				_SetProjectionMatrix(int width, int height);
	
	bool				m_layer3D;

	typedef std::vector<D3DDISPLAYMODE> TDispModes;
	TDispModes m_displayModes;
	std::vector<int>	m_resList;
	hgeBlendState m_currSrcBlend, m_currDstBlend;
	//int m_viewportX, m_viewportY, m_viewportWidth, m_viewportHeight;
	int				    NumDips;
	Gfx_Listener*		m_gfxListener;

	void syncCPU_();
	unsigned int		m_frames;
	IDirect3DSurface8*	m_syncTargets[2];
	IDirect3DTexture8*	m_syncTempTex;
	IDirect3DSurface8*	m_syncTemp;
	bool m_supportNPOT;

private:
	HGE_Impl();
};

extern HGE_Impl*		pHGE;

#endif


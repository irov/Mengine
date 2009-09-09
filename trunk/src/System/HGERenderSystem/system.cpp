/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core system functions
*/

#include "hge_impl.h"

#	include "Interface/LogSystemInterface.h"

int			nRef=0;
HGE_Impl*	pHGE=0;


HGE* CALL hgeCreate( int ver )
{
	if( ver == HGE_VERSION )
	{
		return (HGE*)HGE_Impl::_Interface_Get();
	}
	else
	{
		return 0;
	}
}


HGE_Impl* HGE_Impl::_Interface_Get()
{
	if(!pHGE) pHGE=new HGE_Impl();

	nRef++;

	return pHGE;
}


void CALL HGE_Impl::Release()
{
	nRef--;

	if( !nRef )
	{
		if( pHGE->hwnd )
		{
			_GfxDone();
		}

		delete pHGE;
		pHGE = 0;
	}
}


bool CALL HGE_Impl::System_Initiate( Menge::LogSystemInterface* _logSystem )
{
	m_logSystem = _logSystem;

	if(!_GfxInit()) 
	{ 
		_GfxDone(); 
		return false; 
	}

	return true;
}

void CALL HGE_Impl::System_SetStateBool(hgeBoolState state, bool value)
{
	switch(state)
	{
		case HGE_WINDOWED:		if(VertArray || hwndParent) break;
								if(pD3DDevice && bWindowed != value)
								{
									if(d3dppW.BackBufferFormat==D3DFMT_UNKNOWN || d3dppFS.BackBufferFormat==D3DFMT_UNKNOWN) break;

									if(bWindowed) GetWindowRect(hwnd, &rectW);
									bWindowed=value;
									if(bWindowed) d3dpp=&d3dppW;
									else d3dpp=&d3dppFS;

									if(_format_id(d3dpp->BackBufferFormat) < 4) nScreenBPP=16;
									else nScreenBPP=32;

									_GfxRestore();
									//_AdjustWindow();
								}
								else bWindowed=value;
								break;

		case HGE_ZBUFFER:		if(!pD3DDevice)	bZBuffer=value;
								break;

		case HGE_TEXTUREFILTER: bTextureFilter=value;
								if(pD3DDevice)
								{
									_render_batch();
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
								}
								break;
	}
}

void CALL HGE_Impl::System_SetStateHwnd(hgeHwndState state, HWND value)
{
	switch(state)
	{
		case HGE_HWND:			hwnd = value;	break;
		case HGE_HWNDPARENT:	if(!hwnd) hwndParent=value; break;
	}
}

void CALL HGE_Impl::System_SetStateInt(hgeIntState state, int value)
{
	switch(state)
	{
		case HGE_SCREENWIDTH:	if(!pD3DDevice) nScreenWidth=value; break;

		case HGE_SCREENHEIGHT:	if(!pD3DDevice) nScreenHeight=value; break;

		case HGE_SCREENBPP:		if(!pD3DDevice) nScreenBPP=value; break;

		/*case HGE_FPS:			if(VertArray) break;

								if(pD3DDevice)
								{
									if((nHGEFPS>=0 && value <0) || (nHGEFPS<0 && value>=0))
									{
										if(value==HGEFPS_VSYNC)
										{
											d3dppW.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
											d3dppFS.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
										}
										else
										{
											d3dppW.SwapEffect = D3DSWAPEFFECT_COPY;
											d3dppFS.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
										}
										//if(procFocusLostFunc) procFocusLostFunc();
										_GfxRestore();
										//if(procFocusGainFunc) procFocusGainFunc();
									}
								}
								nHGEFPS=value;
								if(nHGEFPS>0) nFixedDelta=int(1000.0f/value);
								else nFixedDelta=0;
								break;*/
	}
}

bool CALL HGE_Impl::System_GetStateBool(hgeBoolState state)
{
	switch(state)
	{
		case HGE_WINDOWED:		return bWindowed;
		case HGE_ZBUFFER:		return bZBuffer;
		case HGE_TEXTUREFILTER:	return bTextureFilter;
	}

	return false;
}

HWND CALL HGE_Impl::System_GetStateHwnd(hgeHwndState state)
{
	switch(state)
	{
		case HGE_HWND:			return hwnd;
		case HGE_HWNDPARENT:	return hwndParent;
	}

	return 0;
}

int CALL HGE_Impl::System_GetStateInt(hgeIntState state)
{
	switch(state)
	{
		case HGE_SCREENWIDTH:	return nScreenWidth;
		case HGE_SCREENHEIGHT:	return nScreenHeight;
		case HGE_SCREENBPP:		return nScreenBPP;
		case HGE_FPS:			return nHGEFPS;
	}

	return 0;
}

Menge::TCharA* CALL HGE_Impl::System_GetErrorMessage()
{
	return szError;
}

void CALL HGE_Impl::System_Log( const Menge::TCharA *szFormat, ...)
{
	va_list ap;
	
	Menge::TCharA str[1024];

	va_start(ap, szFormat);
	vsprintf(str, szFormat, ap);
	va_end(ap);

	m_logSystem->logMessage( str );
}

void CALL HGE_Impl::System_Snapshot(const char *filename)
{
	LPDIRECT3DSURFACE8 pSurf;
//	char *shotname, tempname[_MAX_PATH];
//	int i;

	/*if(!filename)
	{
		i=0;
		shotname=Resource_EnumFiles("shot???.bmp");
		while(shotname)
		{
			i++;
			shotname=Resource_EnumFiles();
		}
		sprintf(tempname, "shot%03d.bmp", i);
		filename=Resource_MakePath(tempname);
	}*/

	if(pD3DDevice)
	{
		pD3DDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pSurf);
		D3DXSaveSurfaceToFileA(filename, D3DXIFF_BMP, pSurf, NULL, NULL);
		pSurf->Release();
	}
}

//////// Implementation ////////


HGE_Impl::HGE_Impl()
{
	hwnd = 0;
	szError[0] = 0;

	pD3D = 0;
	pD3DDevice = 0;
	d3dpp = 0;
	pTargets = 0;
	pCurTarget = 0;
	pScreenSurf = 0;
	pScreenDepth = 0;
	pVB = 0;
	pIB = 0;
	VertArray = 0;
	textures = 0;
	
	nScreenWidth=800;
	nScreenHeight=600;
	nScreenBPP=32;
	bWindowed=false;
	bZBuffer=false;
	bTextureFilter=true;
	hwndParent=0;
	m_supportNPOT = false;
}

void HGE_Impl::_PostError( Menge::TCharA *error )
{
	System_Log( error );
	strcpy( szError, error );
}


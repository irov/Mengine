/*
 *	DX9Texture.cpp
 *
 *	Created by _Berserk_ on 23.05.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "DX9Texture.h"

#	include <d3d9.h>
#	include "Interface/ImageCodecInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DX9Texture::DX9Texture( IDirect3DTexture9* _d3dInterface )
		: m_d3dInterface( _d3dInterface )
	{
		D3DSURFACE_DESC desc;
		_d3dInterface->GetLevelDesc( 0, &desc );
		m_hwWidth = desc.Width;
		m_hwHeight = desc.Height;
	}
	//////////////////////////////////////////////////////////////////////////
	DX9Texture::~DX9Texture()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char* DX9Texture::lock( int* _pitch, bool _readOnly /*= true */ )
	{
		D3DSURFACE_DESC TDesc;
		D3DLOCKED_RECT TRect;
		RECT* prec = NULL;
		int flags;

		m_d3dInterface->GetLevelDesc(0, &TDesc);
		//if(TDesc.Format!=D3DFMT_A8R8G8B8 && TDesc.Format!=D3DFMT_X8R8G8B8) return 0;

		if(_readOnly) flags=D3DLOCK_READONLY;
		else flags=0;

		if(FAILED(m_d3dInterface->LockRect(0, &TRect, prec, flags)))
		{
			//_PostError( "Can't lock texture" );
			return 0;
		}

		*_pitch = TRect.Pitch;
		return (unsigned char *)TRect.pBits;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9Texture::unlock()
	{
		m_d3dInterface->UnlockRect(0);
	}
	//////////////////////////////////////////////////////////////////////////
	IDirect3DTexture9* DX9Texture::getInterface()
	{
		return m_d3dInterface;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t DX9Texture::getWidth() const
	{
		return m_hwWidth;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t DX9Texture::getHeight() const
	{
		return m_hwHeight;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

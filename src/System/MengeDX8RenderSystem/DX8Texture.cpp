/*
 *	DX8Texture.cpp
 *
 *	Created by _Berserk_ on 22.1.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "DX8Texture.h"

#	include <d3d8.h>
#	include "Interface/ImageCodecInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DX8Texture::DX8Texture( IDirect3DTexture8* _d3dInterface )
		: m_d3dInterface( _d3dInterface )
	{
		D3DSURFACE_DESC desc;
		_d3dInterface->GetLevelDesc( 0, &desc );
		m_hwWidth = desc.Width;
		m_hwHeight = desc.Height;
	}
	//////////////////////////////////////////////////////////////////////////
	DX8Texture::~DX8Texture()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char* DX8Texture::lock( int* _pitch, bool _readOnly /*= true */ )
	{
		D3DSURFACE_DESC TDesc;
		D3DLOCKED_RECT TRect;
		RECT* prec = NULL;
		int flags;

		m_d3dInterface->GetLevelDesc(0, &TDesc);
		//if(TDesc.Format!=D3DFMT_A8R8G8B8 && TDesc.Format!=D3DFMT_X8R8G8B8) return 0;

		if(_readOnly) flags=D3DLOCK_READONLY;
		else flags=0;

		HRESULT hr = m_d3dInterface->LockRect(0, &TRect, prec, flags);
		if(FAILED( hr ))
		{
			//_PostError( "Can't lock texture" );
			return 0;
		}

		*_pitch = TRect.Pitch;
		return (unsigned char *)TRect.pBits;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8Texture::unlock()
	{
		m_d3dInterface->UnlockRect(0);
	}
	//////////////////////////////////////////////////////////////////////////
	IDirect3DTexture8* DX8Texture::getInterface()
	{
		return m_d3dInterface;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t DX8Texture::getWidth() const
	{
		return m_hwWidth;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t DX8Texture::getHeight() const
	{
		return m_hwHeight;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

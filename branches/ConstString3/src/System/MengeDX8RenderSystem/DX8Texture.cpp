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
	DX8Texture::DX8Texture( IDirect3DTexture8 * _d3dInterface, size_t _width, size_t _height )
		: m_d3dInterface(_d3dInterface)
		, m_width(_width)
		, m_height(_height)
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
	///////////////////////////////////////////////////////////////////////////
	unsigned char * DX8Texture::lock( int* _pitch, const Rect& _rect, bool _readOnly )
	{
		int flags;
		if( _readOnly )
		{
			flags=D3DLOCK_READONLY;
		}
		else 
		{
			flags=0;
		}

		RECT rect;
		rect.top = _rect.top;
		rect.bottom = _rect.bottom;
		rect.left = _rect.left;
		rect.right = _rect.right;

		D3DLOCKED_RECT TRect;
		HRESULT hr = m_d3dInterface->LockRect(0, &TRect, &rect, flags);
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
	IDirect3DTexture8 * DX8Texture::getInterface() const
	{
		return m_d3dInterface;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t DX8Texture::getHWWidth() const
	{
		return m_hwWidth;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t DX8Texture::getHWHeight() const
	{
		return m_hwHeight;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8Texture::release()
	{
		if( m_d3dInterface )
		{
			m_d3dInterface->Release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

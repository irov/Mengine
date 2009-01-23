/*
 *	DX8Texture.cpp
 *
 *	Created by _Berserk_ on 22.1.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "DX8Texture.h"

#	include <d3d8.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DX8Texture::DX8Texture( IDirect3DTexture8* _d3dInterface, const String& _name, std::size_t _width, std::size_t _height, PixelFormat _format )
		: m_d3dInterface( _d3dInterface )
		, m_name( _name )
		, m_width( _width )
		, m_height( _height )
		, m_format( _format	)
		, ref( 0 )
	{
		D3DSURFACE_DESC desc;
		_d3dInterface->GetLevelDesc( 0, &desc );
		m_uvMask[0] = static_cast<float>( m_width ) / desc.Width;
		m_uvMask[1] = static_cast<float>( m_height ) / desc.Height;
	}
	//////////////////////////////////////////////////////////////////////////
	DX8Texture::~DX8Texture()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t DX8Texture::getWidth() const 
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t DX8Texture::getHeight() const 
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	const String & DX8Texture::getDescription() const 
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char* DX8Texture::lock( int* _pitch, bool _readOnly /*= true */ )
	{
		D3DSURFACE_DESC TDesc;
		D3DLOCKED_RECT TRect;
		RECT* prec = NULL;
		int flags;

		m_d3dInterface->GetLevelDesc(0, &TDesc);
		if(TDesc.Format!=D3DFMT_A8R8G8B8 && TDesc.Format!=D3DFMT_X8R8G8B8) return 0;

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
	void DX8Texture::unlock()
	{
		m_d3dInterface->UnlockRect(0);
	}
	//////////////////////////////////////////////////////////////////////////
	PixelFormat DX8Texture::getPixelFormat()
	{
		return m_format;
	}
	//////////////////////////////////////////////////////////////////////////
	IDirect3DTexture8* DX8Texture::getInterface()
	{
		return m_d3dInterface;
	}
	//////////////////////////////////////////////////////////////////////////
	int DX8Texture::incref()
	{
		return ++ref;
	}
	//////////////////////////////////////////////////////////////////////////
	int DX8Texture::decref()
	{
		return --ref;
	}
	//////////////////////////////////////////////////////////////////////////
	const float* DX8Texture::getUVMask() const
	{
		return m_uvMask;
	}
	//////////////////////////////////////////////////////////////////////////

}	// namespace Menge

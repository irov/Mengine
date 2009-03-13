/*
 *	Texture.cpp
 *
 *	Created by _Berserk_ on 12.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "Texture.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Texture::Texture( RenderImageInterface* _interface
						, const String& _name
						, size_t _width
						, size_t _height
						, PixelFormat _format
						, size_t _hwWidth
						, size_t _hwHeight
						, PixelFormat _hwPixelFormat )
		: m_iTexture( _interface )
		, m_name( _name )
		, m_width( _width )
		, m_height( _height )
		, m_pixelFormat( _format )
		, m_hwWidth( _hwWidth )
		, m_hwHeight( _hwHeight )
		, m_hwPixelFormat( _hwPixelFormat )
		, m_ref( 1 )
		, m_uvMask( static_cast<float>( _width ) / _hwWidth, static_cast<float>( _height ) / _hwHeight )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Texture::~Texture()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface* Texture::getInterface()
	{
		return m_iTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& Texture::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Texture::getWidth() const
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Texture::getHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	PixelFormat Texture::getPixelFormat() const
	{
		return m_pixelFormat;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Texture::addRef()
	{
		return ++m_ref;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Texture::decRef()
	{
		return --m_ref;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char* Texture::lock( int* _pitch, bool _readOnly /*= true */ )
	{
		return m_iTexture->lock( _pitch, _readOnly );
	}
	//////////////////////////////////////////////////////////////////////////
	void Texture::unlock()
	{
		m_iTexture->unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Texture::getHWWidth() const
	{
		return m_hwWidth;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Texture::getHWHeight() const
	{
		return m_hwHeight;
	}
	//////////////////////////////////////////////////////////////////////////
	Menge::PixelFormat Texture::getHWPixelFormat() const
	{
		return m_hwPixelFormat;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& Texture::getUVMask() const
	{
		return m_uvMask;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

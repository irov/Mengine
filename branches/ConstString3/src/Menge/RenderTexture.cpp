/*
 *	Texture.cpp
 *
 *	Created by _Berserk_ on 12.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "RenderTexture.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderTexture::RenderTexture( RenderImageInterface* _interface
						, size_t _width
						, size_t _height
						, PixelFormat _format
						, size_t _hwWidth
						, size_t _hwHeight
						, PixelFormat _hwPixelFormat
						, size_t _id )
		: m_iTexture(_interface)
		, m_width(_width)
		, m_height(_height)
		, m_pixelFormat(_format)
		, m_hwWidth(_hwWidth)
		, m_hwHeight(_hwHeight)
		, m_hwPixelFormat(_hwPixelFormat)
		, m_ref(1)
		, m_uv(0.f, 0.f, 1.f, 1.f)
		, m_id(_id)
	{
		m_rect.left = 0;
		m_rect.top = 0;
		m_rect.right = m_width;
		m_rect.bottom = m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTexture::~RenderTexture()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface* RenderTexture::getInterface() const
	{
		return m_iTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::getId() const
	{
		return m_id;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderTexture::setFileName( const WString & _filename )
	{
		m_filename = _filename;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & RenderTexture::getFileName() const
	{
		return m_filename;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::getWidth() const
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::getHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	PixelFormat RenderTexture::getPixelFormat() const
	{
		return m_pixelFormat;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::addRef() const
	{
		return ++m_ref;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::decRef() const
	{
		return --m_ref;
	}
    /////////////////////////////////////////////////////////////////////////////
	unsigned char* RenderTexture::lock( int* _pitch, const Rect& _rect, bool _readOnly /*= true */ ) const
	{
		return m_iTexture->lock( _pitch, _rect, _readOnly );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderTexture::unlock() const
	{
		m_iTexture->unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::getHWWidth() const
	{
		return m_hwWidth;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::getHWHeight() const
	{
		return m_hwHeight;
	}
	//////////////////////////////////////////////////////////////////////////
	Menge::PixelFormat RenderTexture::getHWPixelFormat() const
	{
		return m_hwPixelFormat;
	}
	//////////////////////////////////////////////////////////////////////////
	const Rect & RenderTexture::getRect() const
	{
		return m_rect;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & RenderTexture::getUV() const
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

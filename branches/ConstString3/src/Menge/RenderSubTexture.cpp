/*
 *	Texture.cpp
 *
 *	Created by _Berserk_ on 12.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "RenderSubTexture.h"

#	include "Interface/ImageCodecInterface.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderSubTexture::RenderSubTexture( RenderTextureInterface * _texture
						, const Rect & _rect
						, size_t _id
						, RenderTextureInterfaceListener * _listener )
		: m_texture(_texture)
		, m_hwRect(_rect)
		, m_listener(_listener)
		, m_id(_id)
		, m_ref(1)
	{		
		m_rect.left = 0;
		m_rect.top = 0;
		m_rect.right = m_hwRect.getWidth();
		m_rect.bottom = m_hwRect.getHeight();

		size_t texture_width = m_texture->getWidth();
		size_t texture_height = m_texture->getHeight();

		m_uv.x = float(m_hwRect.left) / float(texture_width);
		m_uv.y = float(m_hwRect.top) / float(texture_height);
		m_uv.z = float(m_hwRect.right) / float(texture_width);
		m_uv.w = float(m_hwRect.bottom) / float(texture_height);

		//m_rectWM.left = 0;
		//m_rectWM.top = 0;
		//m_rectWM.right = m_rect.getWidth();
		//m_rectWM.bottom = m_rect.getHeight();

		m_texture->addRef();
	}
	//////////////////////////////////////////////////////////////////////////
	RenderSubTexture::~RenderSubTexture()
	{
		m_texture->decRef();
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface* RenderSubTexture::getImage() const
	{
		return m_texture->getImage();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderSubTexture::destroyImage()
	{
		m_texture = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderSubTexture::getId() const
	{
		return m_id;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderSubTexture::addRef() const
	{
		++m_ref;

		return m_ref;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderSubTexture::decRef() const
	{
		if( --m_ref == 0 )
		{
			if( m_listener != NULL )
			{
				m_listener->onRenderTextureRelease( this );
			}
		}

		return m_ref;
	}
	//////////////////////////////////////////////////////////////////////////
	const Rect & RenderSubTexture::getRect() const
	{
		return m_rect;
	}
	//////////////////////////////////////////////////////////////////////////
	const Rect & RenderSubTexture::getHWRect() const
	{
		return m_hwRect;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & RenderSubTexture::getUV() const
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderSubTexture::setFileName( const WString & _filename )
	{
		m_fileName = _filename;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & RenderSubTexture::getFileName() const
	{
		return m_fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderSubTexture::getWidth() const
	{
		return m_rect.getWidth();
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderSubTexture::getHeight() const
	{
		return m_rect.getHeight();
	}
	//////////////////////////////////////////////////////////////////////////
	PixelFormat RenderSubTexture::getPixelFormat() const
	{
		return m_texture->getPixelFormat();
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderSubTexture::getMemoryUse() const
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char * RenderSubTexture::lock( int* _pitch, const Rect& _rect, bool _readOnly ) const
	{
		Rect rect;

		rect.left = m_hwRect.left + _rect.left;
		rect.top = m_hwRect.top + _rect.top;
		rect.right = m_hwRect.left + _rect.right;
		rect.bottom = m_hwRect.top + _rect.bottom;

		return m_texture->lock( _pitch, rect, _readOnly );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderSubTexture::unlock() const
	{
		m_texture->unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderSubTexture::getHWWidth() const
	{
		return m_texture->getHWWidth();
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderSubTexture::getHWHeight() const
	{
		return m_texture->getHWHeight();
	}
	//////////////////////////////////////////////////////////////////////////
	PixelFormat RenderSubTexture::getHWPixelFormat() const
	{
		return m_texture->getHWPixelFormat();
	}
}	// namespace Menge

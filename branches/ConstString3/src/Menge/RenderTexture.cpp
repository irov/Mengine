/*
 *	Texture.cpp
 *
 *	Created by _Berserk_ on 12.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "RenderTexture.h"
#	include "Interface/ImageCodecInterface.h"
#	include "math/mat4.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderTexture::RenderTexture( RenderImageInterface* _interface
						, const ConstString & _name
						, size_t _width
						, size_t _height
						, PixelFormat _format
						, size_t _hwWidth
						, size_t _hwHeight
						, PixelFormat _hwPixelFormat
						, int _id )
		: m_iTexture(_interface)
		, m_name(_name)
		, m_width(_width)
		, m_height(_height)
		, m_pixelFormat(_format)
		, m_hwWidth(_hwWidth)
		, m_hwHeight(_hwHeight)
		, m_hwPixelFormat(_hwPixelFormat)
		, m_ref(1)
		, m_id(_id)
	{
		//if( _width != _hwWidth 
		//	|| _height != _hwHeight )
		//{
		//	m_uvMask = new mt::mat4f();
		//	mt::ident_m4( *m_uvMask );
		//	m_uvMask->v0.x = static_cast<float>( _width ) / _hwWidth;
		//	m_uvMask->v1.y = static_cast<float>( _height ) / _hwHeight;
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTexture::~RenderTexture()
	{
		//if( m_uvMask != NULL )
		//{
		//	delete m_uvMask;
		//	m_uvMask = NULL;
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface* RenderTexture::getInterface() const
	{
		return m_iTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & RenderTexture::getName() const
	{
		return m_name;
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
	//////////////////////////////////////////////////////////////////////////
	unsigned char* RenderTexture::lock( int* _pitch, bool _readOnly /*= true */ ) const
	{
		return m_iTexture->lock( _pitch, _readOnly );
	}
    /////////////////////////////////////////////////////////////////////////////
	unsigned char* RenderTexture::lockRect( int* _pitch, const Rect& _rect, bool _readOnly /*= true */ ) const
	{
		return m_iTexture->lockRect( _pitch, _rect, _readOnly );
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
	bool RenderTexture::loadImageData( ImageDecoderInterface* _imageDecoder )
	{
		int pitch = 0;
		unsigned char* textureBuffer = lock( &pitch, false );

		if( textureBuffer == NULL )
		{
			MENGE_LOG_ERROR("Texture::loadImageData: Invalid lock");

			return false;
		}


		bool result = loadImageData( textureBuffer, pitch, _imageDecoder );
		unlock();

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderTexture::loadImageData( unsigned char* _textureBuffer, size_t _texturePitch, ImageDecoderInterface* _imageDecoder )
	{
		const ImageCodecDataInfo* dataInfo = _imageDecoder->getCodecDataInfo();

		ImageCodecOptions options;

		if( dataInfo->format == PF_R8G8B8
			&& m_hwPixelFormat == PF_X8R8G8B8 )
		{
			options.flags |= DF_COUNT_ALPHA;
		}

		if( _texturePitch != m_width )
		{
			options.pitch = _texturePitch;

			options.flags |= DF_CUSTOM_PITCH;
		}		

		_imageDecoder->setOptions( &options );

		unsigned int bufferSize = _texturePitch * m_height;
		unsigned int b = _imageDecoder->decode( _textureBuffer, bufferSize );
		if( dataInfo->format == PF_A8
			&& m_hwPixelFormat == PF_A8R8G8B8 )		// need to sweezle alpha
		{
			for( size_t h = dataInfo->height-1; h != -1; --h )
			{
				int hp = h*_texturePitch;
				for( size_t w = dataInfo->width-1; w != -1; --w )
				{
					_textureBuffer[hp+w*4+3] = _textureBuffer[hp+w];
				}
			}
		}
		/*if( b == 0 )
		{
		assert( 0 );
		}*/

		// copy pixels on the edge for better image quality
		if( m_hwWidth > m_width )
		{
			unsigned char* image_data = _textureBuffer;
			unsigned int pixel_size = _texturePitch / m_hwWidth;

			for( size_t i = 0; i != m_height; ++i )
			{
				std::copy( image_data + (m_width - 1) * pixel_size, 
					image_data + m_width * pixel_size,
					image_data + m_width * pixel_size );
				image_data += _texturePitch;
			}
		}

		if( m_hwHeight > m_height )
		{
			unsigned char* image_data = _textureBuffer;
			unsigned int pixel_size = _texturePitch / m_hwWidth;
			std::copy( image_data + (m_height - 1) * _texturePitch,
				image_data + m_height * _texturePitch,
				image_data + m_height * _texturePitch );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	int RenderTexture::getID() const
	{
		return m_id;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

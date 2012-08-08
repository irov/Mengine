#	include "ResourceImageCombineRGBAndAlpha.h"

#	include "ResourceImplement.h"
#	include "CodecEngine.h"
#	include "RenderEngine.h"
#	include "FileEngine.h"
#   include "LogEngine.h"

#	include "Interface/ImageCodecInterface.h"

#	include <cstdio>

#	include "Metacode.h"

#	include "Core/ConstString.h"
#	include "Utils/Core/String.h"

#	include "Consts.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageCombineRGBAndAlpha );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageCombineRGBAndAlpha::ResourceImageCombineRGBAndAlpha()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCombineRGBAndAlpha::getMaxSize() const
	{
		return m_imageFrame.maxSize;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCombineRGBAndAlpha::getSize() const
	{
		return m_imageFrame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageCombineRGBAndAlpha::getOffset() const
	{
		return m_imageFrame.offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageCombineRGBAndAlpha::getUV() const
	{
		return m_imageFrame.uv;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageCombineRGBAndAlpha::getUVImage() const
	{
		return m_imageFrame.uv_image;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterface* ResourceImageCombineRGBAndAlpha::getTexture() const
	{
		return m_imageFrame.texture;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCombineRGBAndAlpha::isAlpha() const
	{
		return m_imageFrame.isAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & ResourceImageCombineRGBAndAlpha::getFileName() const
	{
		return m_imageDesc.fileNameAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageCombineRGBAndAlpha::getCodecType() const
	{
		return m_imageDesc.codecTypeAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCombineRGBAndAlpha::isValid() const
	{
		const ConstString & category = this->getCategory();
		
		if( FileEngine::get()
			->existFile( category, m_imageDesc.fileNameRGB ) == false )
		{
			return false;
		}

		if( FileEngine::get()
			->existFile( category, m_imageDesc.fileNameAlpha ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageCombineRGBAndAlpha::loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceImageCombineRGBAndAlpha * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceImageCombineRGBAndAlpha *>(_meta);

        m_imageDesc.uv = mt::vec4f(0.f, 0.f, 1.f, 1.f);
        m_imageDesc.maxSize = mt::vec2f(-1.f, -1.f);
        m_imageDesc.offset = mt::vec2f(0.f, 0.f);
        m_imageDesc.size = mt::vec2f(-1.f, -1.f);
        m_imageDesc.isAlpha = true;
        m_imageDesc.wrapX = false;
        m_imageDesc.wrapY = false;

        metadata->swap_File_PathAlpha( m_imageDesc.fileNameAlpha );

        if( metadata->swap_File_CodecAlpha( m_imageDesc.codecTypeAlpha ) == false )
        {
            m_imageDesc.codecTypeAlpha = s_getImageCodec( m_imageDesc.fileNameAlpha );
        }

        metadata->swap_File_PathRGB( m_imageDesc.fileNameRGB );

        if( metadata->swap_File_CodecRGB( m_imageDesc.codecTypeRGB ) == false )
        {
            m_imageDesc.codecTypeRGB = s_getImageCodec( m_imageDesc.fileNameRGB );
        }

        metadata->get_File_MaxSize( m_imageDesc.maxSize );
        metadata->get_File_UV( m_imageDesc.uv );
        metadata->get_File_Size( m_imageDesc.size );
        metadata->get_File_Alpha( m_imageDesc.isAlpha );
        metadata->get_File_WrapX( m_imageDesc.wrapX );
        metadata->get_File_WrapY( m_imageDesc.wrapY );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCombineRGBAndAlpha::_compile()
	{	
		ImageFrame frame;

		const ConstString & category = this->getCategory();
			
		if( this->loadImageFrameCombineRGBAndAlpha_( frame, category, m_imageDesc.fileNameRGB, m_imageDesc.fileNameAlpha, m_imageDesc.codecTypeRGB, m_imageDesc.codecTypeAlpha ) == false )
		{
			return false;
		}
		
		//frame.uv = m_imageDesc.uv;
		frame.uv_image = m_imageDesc.uv;

		frame.maxSize = m_imageDesc.maxSize;
		frame.offset =  m_imageDesc.offset;

		float ku = m_imageDesc.uv.z - m_imageDesc.uv.x;
		float kv = m_imageDesc.uv.w - m_imageDesc.uv.y;
		frame.size.x *= ku;
		frame.size.y *= kv;

		//frame.size.x = ::floorf( frame.size.x + 0.5f );
		//frame.size.y = ::floorf( frame.size.y + 0.5f );
		//mt::vec2f(frame.size.x * ku , frame.size.y * kv );

		frame.uv.x = frame.uv_scale.x + (frame.uv_scale.z - frame.uv_scale.x) * m_imageDesc.uv.x;
		frame.uv.y = frame.uv_scale.y + (frame.uv_scale.w - frame.uv_scale.y) * m_imageDesc.uv.y;
		frame.uv.z = frame.uv_scale.x + (frame.uv_scale.z - frame.uv_scale.x) * m_imageDesc.uv.z;
		frame.uv.w = frame.uv_scale.y + (frame.uv_scale.w - frame.uv_scale.y) * m_imageDesc.uv.w;

		if( frame.maxSize.x < 0.f || frame.maxSize.y < 0.f )
		{
			frame.maxSize = frame.size;
		}

		frame.isAlpha = m_imageDesc.isAlpha;
		frame.wrapX = m_imageDesc.wrapX;
		frame.wrapY = m_imageDesc.wrapY;

		m_imageFrame = frame;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCombineRGBAndAlpha::loadImageFrameCombineRGBAndAlpha_( ImageFrame & _frame, const ConstString& _pakName,  const WString& _fileNameRGB, const WString& _fileNameAlpha, const ConstString & _codecRGB , const ConstString & _codecAlpha  ) const
	{
		RenderTextureInterface * texture = this->createTextureRGBAndAlpha_( _pakName, _fileNameRGB, _fileNameAlpha, _codecRGB, _codecAlpha );
		if( texture == NULL )
		{
			return false;
		}		
						
		bool res = this->prepareImageFrame_( _frame, texture );

		return res;
	}
	/////////////////////////////////////////////////////////////////////////
	RenderTextureInterface * ResourceImageCombineRGBAndAlpha::createTextureRGBAndAlpha_( const ConstString& _pakName, const WString& _fileNameRGB, const WString& _fileNameAlpha, const ConstString & _codecRGB, const ConstString & _codecAlpha ) const
	{
		if( RenderEngine::get()
			->hasTexture( _fileNameAlpha ) == true )
		{
			RenderTextureInterface * texture = RenderEngine::get()
				->getTexture( _fileNameAlpha );

			return texture;
		}

		////////////////////////////////////// init RGB Decoder
		FileInputStreamInterface * streamRGB = FileEngine::get()
			->openInputFile( _pakName, _fileNameRGB );

		if( streamRGB == 0 )
		{
			MENGE_LOG_ERROR( "ResourceImageCombineRGBAndAlpha::createTextureRGBAndAlpha_: '%s' Image file with RGB data '%S' was not found"
				, this->getName().c_str()
				, _fileNameRGB.c_str() 
				);

			return NULL;
		}

		ImageDecoderInterface * imageDecoderRGB = this->createDecoder_( streamRGB, _codecRGB );

		if( imageDecoderRGB == 0 )
		{
			MENGE_LOG_ERROR( "ResourceImageCombineRGBAndAlpha::createTextureRGBAndAlpha_: '%s' Image decoder for file '%s' was not found"
				, this->getName().c_str()
				, _fileNameRGB.c_str() 
				);

			streamRGB->close();

			return NULL;
		}

		///Load Alpha data
		FileInputStreamInterface * streamAlpha = FileEngine::get()
			->openInputFile( _pakName, _fileNameAlpha );

		if( streamAlpha == 0 )
		{
			MENGE_LOG_ERROR( "ResourceImageCombineRGBAndAlpha::createTextureRGBAndAlpha_: '%s' Image file with alpha channel data '%s' was not found"
				, this->getName().c_str()
				, _fileNameAlpha.c_str() 
				);

			streamRGB->close();
			imageDecoderRGB->destroy();
			
			return NULL;
		}

		///Get Alpha Decoder
		ImageDecoderInterface * imageDecoderAlpha = this->createDecoder_( streamAlpha, _codecAlpha );

		if( imageDecoderAlpha == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::Warning: Image decoder for file '%s' was not found"
				, _fileNameAlpha.c_str() 
				);

			streamRGB->close();
			imageDecoderRGB->destroy();
			streamAlpha->close();
			
			return NULL;
		}

		const ImageCodecDataInfo* dataInfoRGB = imageDecoderRGB->getCodecDataInfo();

		RenderTextureInterface* texture = RenderEngine::get()
			->createTexture( dataInfoRGB->width, dataInfoRGB->height, PF_A8R8G8B8 );

		if( texture == NULL )
		{
			MENGE_LOG_ERROR( "ResourceImageCombineRGBAndAlpha::createTextureRGBAndAlpha_: '%s' can`t create texture %s width %i height %i "
				, this->getName().c_str()
				, _fileNameAlpha.c_str() 
				, dataInfoRGB->width
				, dataInfoRGB->height
				);

			imageDecoderRGB->destroy();
			streamRGB->close();

			imageDecoderAlpha->destroy();
			streamAlpha->close();
			
			return NULL;
		}

		Rect rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = dataInfoRGB->width;
		rect.bottom = dataInfoRGB->height;

		int pitch = 0;
		unsigned char * buffer = texture->lock( &pitch, rect, false );
		
		if( buffer == NULL )
		{
			MENGE_LOG_ERROR( "ResourceImageCombineRGBAndAlpha::createTextureRGBAndAlpha_: '%s' can`t lock buffer '%s'"
				, this->getName().c_str()
				, _fileNameAlpha.c_str() 
				);

			imageDecoderRGB->destroy();
			streamRGB->close();

			imageDecoderAlpha->destroy();
			streamAlpha->close();

			return NULL;
		}

		this->loadRGBAndAlphaData_( buffer, pitch, imageDecoderRGB, imageDecoderAlpha );
		texture->unlock();
		
		imageDecoderRGB->destroy();
		streamRGB->close();

		imageDecoderAlpha->destroy();
		streamAlpha->close();
		
		RenderEngine::get()
			->cacheFileTexture( _fileNameAlpha, texture );

		return texture;
	}
	/////////////////////////////////////////////////////////////////////////
	void ResourceImageCombineRGBAndAlpha::loadRGBAndAlphaData_( unsigned char * _buffer, int _pitch, ImageDecoderInterface * _imageDecoderRGB, ImageDecoderInterface * _imageDecoderAlpha ) const
	{
		//RGB
		ImageCodecOptions optionsRGB;

		optionsRGB.flags |= DF_COUNT_ALPHA;
        optionsRGB.flags |= DF_CUSTOM_PITCH;
		optionsRGB.pitch = _pitch;

		_imageDecoderRGB->setOptions( &optionsRGB );
		
		RenderEngine::get()
			->loadBufferImageData( _buffer, _pitch, PF_X8R8G8B8, _imageDecoderRGB );
		
		//Alpha
		const ImageCodecDataInfo* dataInfoAlpha = _imageDecoderAlpha->getCodecDataInfo();

		ImageCodecOptions optionsAlpha;

		optionsAlpha.flags |= DF_WRITE_ALPHA_ONLY;
        optionsAlpha.flags |= DF_CUSTOM_PITCH;
		optionsAlpha.pitch = _pitch;

		_imageDecoderAlpha->setOptions( &optionsAlpha );
		
		RenderEngine::get()
			->loadBufferImageData( _buffer, _pitch, PF_A8R8G8B8, _imageDecoderAlpha );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageCombineRGBAndAlpha::_release()
	{
		this->releaseImageFrame_( m_imageFrame );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCombineRGBAndAlpha::getWrapX() const 
	{
		return m_imageFrame.wrapX;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCombineRGBAndAlpha::getWrapY() const 
	{
		return m_imageFrame.wrapY;
	}
	/////////////////////////////////////////////////////////////////////////
	bool ResourceImageCombineRGBAndAlpha::loadBuffer( unsigned char * _buffer, int _pitch )
	{
		const ConstString & category = this->getCategory();

		////////////////////////////////////// init RGB Decoder
		FileInputStreamInterface * streamRGB = FileEngine::get()
			->openInputFile( category, m_imageDesc.fileNameRGB );

		if( streamRGB == 0 )
		{
			MENGE_LOG_ERROR( "ResourceImageCombineRGBAndAlpha::loadTextureCombineRGBAndAlpha: Image file with RGB data '%S' was not found"
				, m_imageDesc.fileNameRGB.c_str() 
				);

			return false;
		}

		ImageDecoderInterface * imageDecoderRGB = this->createDecoder_( streamRGB, m_imageDesc.codecTypeRGB );

		if( imageDecoderRGB == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::Warning: Image decoder for file '%s' was not found"
				, m_imageDesc.fileNameRGB.c_str() 
				);

			streamRGB->close();

			return false;
		}

		///Load Alpha data
		FileInputStreamInterface * streamAlpha = FileEngine::get()
			->openInputFile( category, m_imageDesc.fileNameAlpha );

		if( streamAlpha == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::Warning: Image file with alpha channel data '%s' was not found"
				, m_imageDesc.fileNameAlpha.c_str() 
				);

			streamRGB->close();
			imageDecoderRGB->destroy();

			return false;
		}

		///Get Alpha Decoder
		ImageDecoderInterface * imageDecoderAlpha = this->createDecoder_( streamAlpha, m_imageDesc.codecTypeAlpha );

		if( imageDecoderAlpha == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::Warning: Image decoder for file '%s' was not found"
				, m_imageDesc.fileNameAlpha.c_str() 
				);

			streamRGB->close();
			imageDecoderRGB->destroy();
			streamAlpha->close();

			return false;
		}

		this->loadRGBAndAlphaData_( _buffer, _pitch, imageDecoderRGB, imageDecoderAlpha );

		imageDecoderRGB->destroy();
		streamRGB->close();

		imageDecoderAlpha->destroy();
		streamAlpha->close();

		return true;
	}
	/////////////////////////////////////////////////////////////////////////
}

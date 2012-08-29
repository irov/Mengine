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
    const WString & ResourceImageCombineRGBAndAlpha::getFileRGBName() const
    {
        return m_fileNameRGB;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceImageCombineRGBAndAlpha::getCodecRGBType() const
    {
        return m_codecTypeRGB;
    }
	//////////////////////////////////////////////////////////////////////////
	const WString & ResourceImageCombineRGBAndAlpha::getFileAlphaName() const
	{
		return m_fileNameAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageCombineRGBAndAlpha::getCodecAlphaType() const
	{
		return m_codecTypeAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCombineRGBAndAlpha::isValid() const
	{
		const ConstString & category = this->getCategory();
		
		if( FileEngine::get()
			->existFile( category, m_fileNameRGB ) == false )
		{
			return false;
		}

		if( FileEngine::get()
			->existFile( category, m_fileNameAlpha ) == false )
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

        m_imageFrame.uv = mt::vec4f(0.f, 0.f, 1.f, 1.f);
        m_imageFrame.maxSize = mt::vec2f(-1.f, -1.f);
        m_imageFrame.size = mt::vec2f(-1.f, -1.f);
        m_imageFrame.isAlpha = true;
        m_imageFrame.wrapX = false;
        m_imageFrame.wrapY = false;

        metadata->swap_File_PathAlpha( m_fileNameAlpha );
        metadata->swap_File_CodecAlpha( m_codecTypeAlpha );

        metadata->swap_File_PathRGB( m_fileNameRGB );
        metadata->swap_File_CodecRGB( m_codecTypeRGB );

        //metadata->get_File_MaxSize( m_imageFrame.maxSize );
        metadata->get_File_UV( m_imageFrame.uv );
        //metadata->get_File_Size( m_imageFrame.size );
        metadata->get_File_Alpha( m_imageFrame.isAlpha );
        metadata->get_File_WrapX( m_imageFrame.wrapX );
        metadata->get_File_WrapY( m_imageFrame.wrapY );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCombineRGBAndAlpha::_compile()
	{			
        if( m_codecTypeAlpha.empty() == true )
        {
            m_codecTypeAlpha = s_getImageCodec( m_fileNameAlpha );
        }

        if( m_codecTypeRGB.empty() == true )
        {
            m_codecTypeRGB = s_getImageCodec( m_fileNameRGB );
        }

		const ConstString & category = this->getCategory();
			
		if( this->loadImageFrameCombineRGBAndAlpha_( m_imageFrame, category, m_fileNameRGB, m_fileNameAlpha, m_codecTypeRGB, m_codecTypeAlpha ) == false )
		{
			return false;
		}

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

		this->loadRGBData_( buffer, pitch, imageDecoderRGB );
        this->loadAlphaData_( buffer, pitch, imageDecoderAlpha );

        RenderEngine::get()
            ->sweezleAlpha( texture, buffer, pitch );

        RenderEngine::get()
            ->imageQuality( texture, buffer, pitch );
        
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
	void ResourceImageCombineRGBAndAlpha::loadRGBData_( unsigned char * _buffer, int _pitch, ImageDecoderInterface * _imageDecoderRGB ) const
	{
		//RGB
		ImageCodecOptions optionsRGB;

		optionsRGB.flags |= DF_COUNT_ALPHA;
        optionsRGB.flags |= DF_CUSTOM_PITCH;
		optionsRGB.pitch = _pitch;

		_imageDecoderRGB->setOptions( &optionsRGB );
		
        const ImageCodecDataInfo * data = _imageDecoderRGB->getCodecDataInfo();

        unsigned int bufferSize = _pitch * data->height;
        unsigned int b = _imageDecoderRGB->decode( _buffer, bufferSize );
	}
    /////////////////////////////////////////////////////////////////////////
    void ResourceImageCombineRGBAndAlpha::loadAlphaData_( unsigned char * _buffer, int _pitch, ImageDecoderInterface * _imageDecoderAlpha ) const
    {
        //Alpha
        const ImageCodecDataInfo* dataInfoAlpha = _imageDecoderAlpha->getCodecDataInfo();

        ImageCodecOptions optionsAlpha;

        optionsAlpha.flags |= DF_WRITE_ALPHA_ONLY;
        optionsAlpha.flags |= DF_CUSTOM_PITCH;
        optionsAlpha.pitch = _pitch;

        _imageDecoderAlpha->setOptions( &optionsAlpha );

        const ImageCodecDataInfo * data = _imageDecoderAlpha->getCodecDataInfo();

        unsigned int bufferSize = _pitch * data->height;
        unsigned int b = _imageDecoderAlpha->decode( _buffer, bufferSize );
    }
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageCombineRGBAndAlpha::_release()
	{
		this->releaseImageFrame_( m_imageFrame );
	}
	/////////////////////////////////////////////////////////////////////////
	bool ResourceImageCombineRGBAndAlpha::loadBufferAlpha( unsigned char * _buffer, int _pitch )
	{
		const ConstString & category = this->getCategory();

		///Load Alpha data
		FileInputStreamInterface * streamAlpha = FileEngine::get()
			->openInputFile( category, m_fileNameAlpha );

		if( streamAlpha == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::Warning: Image file with alpha channel data '%s' was not found"
				, m_fileNameAlpha.c_str() 
				);

			return false;
		}

		///Get Alpha Decoder
		ImageDecoderInterface * imageDecoderAlpha = this->createDecoder_( streamAlpha, m_codecTypeAlpha );

		if( imageDecoderAlpha == 0 )
		{
			MENGE_LOG_ERROR( "RenderEngine::Warning: Image decoder for file '%s' was not found"
				, m_fileNameAlpha.c_str() 
				);

			streamAlpha->close();

			return false;
		}

        const ImageCodecDataInfo* dataInfoAlpha = imageDecoderAlpha->getCodecDataInfo();

        ImageCodecOptions optionsAlpha;

        optionsAlpha.flags |= DF_READ_ALPHA_ONLY;
        optionsAlpha.flags |= DF_CUSTOM_PITCH;
        optionsAlpha.pitch = _pitch;

        imageDecoderAlpha->setOptions( &optionsAlpha );

        const ImageCodecDataInfo * data = imageDecoderAlpha->getCodecDataInfo();

        unsigned int bufferSize = _pitch * data->height;
        unsigned int b = imageDecoderAlpha->decode( _buffer, bufferSize );

		imageDecoderAlpha->destroy();
		streamAlpha->close();

		return true;
	}
	/////////////////////////////////////////////////////////////////////////
}

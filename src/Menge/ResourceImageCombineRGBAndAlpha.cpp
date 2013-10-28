#	include "ResourceImageCombineRGBAndAlpha.h"
#	include "Kernel/ResourceImplement.h"

#   include "Interface/CodecInterface.h"
#   include "Interface/RenderSystemInterface.h"
#   include "Interface/FileSystemInterface.h"

#   include "Logger/Logger.h"

#	include "Interface/ImageCodecInterface.h"

#	include <cstdio>

#	include "Core/ConstString.h"
#	include "Core/String.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageCombineRGBAndAlpha );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageCombineRGBAndAlpha::ResourceImageCombineRGBAndAlpha()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    const FilePath & ResourceImageCombineRGBAndAlpha::getFileRGBName() const
    {
        return m_fileNameRGB;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceImageCombineRGBAndAlpha::getCodecRGBType() const
    {
        return m_codecTypeRGB;
    }
	//////////////////////////////////////////////////////////////////////////
	const FilePath & ResourceImageCombineRGBAndAlpha::getFileAlphaName() const
	{
		return m_fileNameAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageCombineRGBAndAlpha::getCodecAlphaType() const
	{
		return m_codecTypeAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCombineRGBAndAlpha::_isValid() const
	{
		const ConstString & category = this->getCategory();
		
		if( FILE_SERVICE(m_serviceProvider)
			->existFile( category, m_fileNameRGB, NULL ) == false )
		{
			return false;
		}

		if( FILE_SERVICE(m_serviceProvider)
			->existFile( category, m_fileNameAlpha, NULL ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCombineRGBAndAlpha::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceImageCombineRGBAndAlpha * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceImageCombineRGBAndAlpha *>(_meta);

        m_uv = mt::vec4f(0.f, 0.f, 1.f, 1.f);
        m_textureSize = mt::vec2f(0.f, 0.f);
        m_maxSize = mt::vec2f(0.f, 0.f);
        m_size = mt::vec2f(0.f, 0.f);
        m_isAlpha = true;
        m_wrapX = false;  
        m_wrapY = false;

        metadata->swap_File_PathAlpha( m_fileNameAlpha );
        metadata->swap_File_CodecAlpha( m_codecTypeAlpha );

        metadata->swap_File_PathRGB( m_fileNameRGB );
        metadata->swap_File_CodecRGB( m_codecTypeRGB );

        metadata->get_File_UV( m_uv );
        metadata->get_File_Alpha( m_isAlpha );
        metadata->get_File_WrapX( m_wrapX );
        metadata->get_File_WrapY( m_wrapY );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCombineRGBAndAlpha::_compile()
	{			
        if( m_codecTypeAlpha.empty() == true )
        {
            m_codecTypeAlpha = CODEC_SERVICE(m_serviceProvider)
                ->findCodecType( m_fileNameAlpha );
        }

        if( m_codecTypeRGB.empty() == true )
        {
            m_codecTypeRGB = CODEC_SERVICE(m_serviceProvider)
                ->findCodecType( m_fileNameRGB );
        }

		const ConstString & category = this->getCategory();
			
		if( this->loadImageFrameCombineRGBAndAlpha_( category, m_fileNameRGB, m_fileNameAlpha, m_codecTypeRGB, m_codecTypeAlpha ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageCombineRGBAndAlpha::loadImageFrameCombineRGBAndAlpha_( const ConstString& _pakName,  const FilePath& _fileNameRGB, const FilePath& _fileNameAlpha, const ConstString & _codecRGB , const ConstString & _codecAlpha  )
	{
		RenderTextureInterfacePtr texture = this->createTextureRGBAndAlpha_( _pakName, _fileNameRGB, _fileNameAlpha, _codecRGB, _codecAlpha );

		if( texture == nullptr )
		{
			return false;
		}		
						
		bool res = this->prepareImageFrame_( texture );

        m_isAlpha = true;

		return res;
	}
	/////////////////////////////////////////////////////////////////////////
	RenderTextureInterfacePtr ResourceImageCombineRGBAndAlpha::createTextureRGBAndAlpha_( const ConstString& _pakName, const FilePath& _fileNameRGB, const FilePath& _fileNameAlpha, const ConstString & _codecRGB, const ConstString & _codecAlpha ) const
	{
        RenderTextureInterfacePtr texture_alpha;
		if( RENDERTEXTURE_SERVICE(m_serviceProvider)
			->hasTexture( _fileNameAlpha, &texture_alpha ) == true )
		{
			return texture_alpha;
		}

		////////////////////////////////////// init RGB Decoder
		InputStreamInterfacePtr streamRGB = FILE_SERVICE(m_serviceProvider)
			->openInputFile( _pakName, _fileNameRGB );

		if( streamRGB == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "ResourceImageCombineRGBAndAlpha::createTextureRGBAndAlpha_: '%s' Image file with RGB data '%s' was not found"
				, this->getName().c_str()
				, _fileNameRGB.c_str()
				);

			return NULL;
		}

		ImageDecoderInterfacePtr imageDecoderRGB = this->createDecoder_( streamRGB, _codecRGB );

		if( imageDecoderRGB == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "ResourceImageCombineRGBAndAlpha::createTextureRGBAndAlpha_: '%s' Image decoder for file '%s' was not found"
				, this->getName().c_str()
				, _fileNameRGB.c_str() 
				);

			return nullptr;
		}

		///Load Alpha data
		InputStreamInterfacePtr streamAlpha = FILE_SERVICE(m_serviceProvider)
			->openInputFile( _pakName, _fileNameAlpha );

		if( streamAlpha == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "ResourceImageCombineRGBAndAlpha::createTextureRGBAndAlpha_: '%s' Image file with alpha channel data '%s' was not found"
				, this->getName().c_str()
				, _fileNameAlpha.c_str() 
				);
            		
			return nullptr;
		}

		///Get Alpha Decoder
		ImageDecoderInterfacePtr imageDecoderAlpha = this->createDecoder_( streamAlpha, _codecAlpha );

		if( imageDecoderAlpha == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "ResourceImageCombineRGBAndAlpha::createTextureRGBAndAlpha_: Image decoder for file '%s' was not found"
				, _fileNameAlpha.c_str() 
				);
			
			return nullptr;
		}

		const ImageCodecDataInfo* dataInfoRGB = imageDecoderRGB->getCodecDataInfo();

		RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE(m_serviceProvider)
			->createTexture( dataInfoRGB->width, dataInfoRGB->height, 4, PF_UNKNOWN );

		if( texture == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "ResourceImageCombineRGBAndAlpha::createTextureRGBAndAlpha_: '%s' can`t create texture %s width %i height %i "
				, this->getName().c_str()
				, _fileNameAlpha.c_str()
				, dataInfoRGB->width
				, dataInfoRGB->height
				);
			
			return nullptr;
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
			LOGGER_ERROR(m_serviceProvider)( "ResourceImageCombineRGBAndAlpha::createTextureRGBAndAlpha_: '%s' can`t lock buffer '%s'"
				, this->getName().c_str()
				, _fileNameAlpha.c_str() 
				);

            texture = nullptr;

			return nullptr;
		}

        if( this->loadAlphaData_( buffer, pitch, imageDecoderAlpha ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ResourceImageCombineRGBAndAlpha::createTextureRGBAndAlpha_: '%s' can`t load alpha data '%s'"
                , this->getName().c_str()
                , _fileNameAlpha.c_str() 
                );

            texture->unlock();

            texture = nullptr;

            return nullptr;
        }

		if( this->loadRGBData_( buffer, pitch, imageDecoderRGB ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ResourceImageCombineRGBAndAlpha::createTextureRGBAndAlpha_: '%s' can`t load rgb data '%s'"
                , this->getName().c_str()
                , _fileNameAlpha.c_str() 
                );

            texture->unlock();

            texture = nullptr;

            return nullptr;
        }

        //RENDER_SERVICE(m_serviceProvider)
        //    ->sweezleAlpha( texture, buffer, pitch );

        RENDERTEXTURE_SERVICE(m_serviceProvider)
            ->imageQuality( texture, buffer, pitch );
        
		texture->unlock();
		
		RENDERTEXTURE_SERVICE(m_serviceProvider)
			->cacheFileTexture( _fileNameAlpha, texture );

		return texture;
	}
	/////////////////////////////////////////////////////////////////////////
	bool ResourceImageCombineRGBAndAlpha::loadRGBData_( unsigned char * _buffer, int _pitch, const ImageDecoderInterfacePtr & _imageDecoderRGB ) const
	{
		//RGB
		ImageCodecOptions optionsRGB;

		optionsRGB.flags |= DF_COUNT_ALPHA;
        optionsRGB.flags |= DF_NOT_ADD_ALPHA;        

		optionsRGB.pitch = _pitch;
        optionsRGB.channels = 4;

		if( _imageDecoderRGB->setOptions( &optionsRGB ) == false )
        {
            return false;
        }
		
        const ImageCodecDataInfo * data = _imageDecoderRGB->getCodecDataInfo();

        unsigned int bufferSize = _pitch * data->height;
        unsigned int b = _imageDecoderRGB->decode( _buffer, bufferSize );

        return bufferSize == b;
	}
    /////////////////////////////////////////////////////////////////////////
    bool ResourceImageCombineRGBAndAlpha::loadAlphaData_( unsigned char * _buffer, int _pitch, const ImageDecoderInterfacePtr & _imageDecoderAlpha ) const
    {
        //Alpha
        ImageCodecOptions optionsAlpha;

        optionsAlpha.flags |= DF_WRITE_ALPHA_ONLY;
        
        optionsAlpha.pitch = _pitch;
        optionsAlpha.channels = 4;

        if( _imageDecoderAlpha->setOptions( &optionsAlpha ) == false )
        {
            return false;
        }

        const ImageCodecDataInfo * data = _imageDecoderAlpha->getCodecDataInfo();

        unsigned int bufferSize = _pitch * data->height;
        unsigned int b = _imageDecoderAlpha->decode( _buffer, bufferSize );

        return bufferSize == b;
    }
	/////////////////////////////////////////////////////////////////////////
}

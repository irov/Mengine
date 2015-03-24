#	include "ResourceImageDefault.h"

#	include "Kernel/ResourceImplement.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ConfigInterface.h"

#   include "Logger/Logger.h"

#	include "Core/CacheMemoryBuffer.h"
#	include "Core/ConstString.h"
#	include "Core/String.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageDefault );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDefault::ResourceImageDefault()
		: m_validNoExist(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & ResourceImageDefault::getFilePath() const
	{
		return m_filePath;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageDefault::getCodecType() const
	{
		return m_codecType;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_checkRowTransparency( const void * _buffer, size_t _width, size_t _height, size_t _row )
	{				
		(void)_height;

		const uint8_t * pixel_memory = static_cast<const uint8_t *>(_buffer) + _row * _width * 4;

		for( size_t i = 0; i != _width; ++i )
		{
			if( pixel_memory[i * 4 + 3] != 0 )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_checkColumnTransparency( const void * _buffer, size_t _width, size_t _height, size_t _column )
	{				
		const uint8_t * pixel_memory = static_cast<const uint8_t *>(_buffer);

		for( size_t i = 0; i != _height; ++i )
		{
			if( pixel_memory[i * _width * 4 + _column * 4 + 3] != 0 )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_checkRowColumnTransparency( const void * _buffer, size_t _width, size_t _height )
	{	
		for( size_t i = 0; i != _width; ++i )
		{
			if( s_checkColumnTransparency( _buffer, _width, _height, i ) == true )
			{
				return true;
			}
		}

		for( size_t j = 0; j != _height; ++j )
		{
			if( s_checkRowTransparency( _buffer, _width, _height, j ) == true )
			{
				return true;
			}
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_allPixelsTransparency( const void * _buffer, size_t _size )
	{
		const uint8_t * pixel_memory = static_cast<const uint8_t *>(_buffer);

		for( size_t i = 0; i != _size; i += 4 )
		{
			if( pixel_memory[i + 3] != 0 )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_isValid() const
	{
		const ConstString & category = this->getCategory();

		bool exist = FILE_SERVICE(m_serviceProvider)
			->existFile( category, m_filePath, nullptr );
		
		if( exist == false )
		{
			if( m_validNoExist == true )
			{
				return true;
			}

			LOGGER_ERROR(m_serviceProvider)("ResourceImageDefault::_isValid %s not exist file %s:%s"
				, m_name.c_str()
				, category.c_str()
				, m_filePath.c_str()
				);

			return false;
		}
        
        InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( category, m_filePath, false );

        if( stream == nullptr )
        {
			LOGGER_ERROR(m_serviceProvider)("ResourceImageDefault::_isValid %s invalid open file %s:%s"
				, m_name.c_str()
				, category.c_str()
				, m_filePath.c_str()
				);

            return false;
        }

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(m_serviceProvider)
            ->createDecoderT<ImageDecoderInterfacePtr>( m_codecType );

        if( imageDecoder == nullptr )
        {
			LOGGER_ERROR(m_serviceProvider)("ResourceImageDefault::_isValid %s file %s:%s invalid decoder %s"
				, m_name.c_str()
				, category.c_str()
				, m_filePath.c_str()
				, m_codecType.c_str()
				);

            return false;
        }

		if( imageDecoder->prepareData( stream ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceImageDefault::_isValid %s file %s:%s decoder initialize failed %s"
				, m_name.c_str()
				, category.c_str()
				, m_filePath.c_str()
				, m_codecType.c_str()
				);

			return false;
		}

		const ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

		uint32_t limitTextureWidth = CONFIG_SERVICE(m_serviceProvider)
			->getValue( "Limit", "TextureWidth", 2048U );

		uint32_t limitTextureHeight = CONFIG_SERVICE(m_serviceProvider)
			->getValue( "Limit", "TextureHeight", 2048U );

		float width = (float)dataInfo->width;
		float height = (float)dataInfo->height;
		
		if( width > limitTextureWidth || height > limitTextureHeight )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceImageDefault::_isValid %s file %s:%s invalid limit %d:%d texture size %d:%d "
				, m_name.c_str()
				, category.c_str()
				, m_filePath.c_str()
				, limitTextureWidth
				, limitTextureHeight
				, dataInfo->width
				, dataInfo->height
				);

			return false;
		}

		mt::vec2f test_size;
		if( m_size.x < 1.f || m_size.y < 1.f )
		{
			test_size.x = m_maxSize.x;
			test_size.y = m_maxSize.y;
		}
		else
		{
			test_size.x = m_size.x;
			test_size.y = m_size.y;
		}

		if( (test_size.x != width || test_size.y != height) && ( test_size.x > 4.f && test_size.y > 4.f ) )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceImage::_isValid: '%s' file '%s:%s' incorrect size %f:%f texture %f:%f"
				, this->getName().c_str()
				, category.c_str()
				, m_filePath.c_str()
				, test_size.x
				, test_size.y
				, width
				, height
				);

			return false;
		}

		if( CONFIG_SERVICE(m_serviceProvider)->getValue( "Check", "ImageTransparency", false ) == true && dataInfo->channels == 4 )
		{
			size_t texture_size = dataInfo->getSize();
			CacheMemoryBuffer buffer(m_serviceProvider, texture_size, "ResourceImageDefault::_isValid" );
			void * buffer_memory = buffer.getMemory();

			ImageCodecOptions options;
			options.channels = 4;
			options.pitch = dataInfo->width * options.channels;

			if( imageDecoder->setOptions( &options ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceImageDefault::_isValid %s file %s:%s invalid optionizing"
					, m_name.c_str()
					, category.c_str()
					, m_filePath.c_str()
					, m_codecType.c_str()
					);

				return false;
			}

			if( imageDecoder->decode( buffer_memory, texture_size ) == 0 )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceImageDefault::_isValid %s file %s:%s invalid decode %s"
					, m_name.c_str()
					, category.c_str()
					, m_filePath.c_str()
					, m_codecType.c_str()
					);

				return false;
			}

			if( s_allPixelsTransparency( buffer_memory, texture_size ) == true )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceImageDefault::_isValid %s file %s:%s codec %s all pixels transparency!"
					, m_name.c_str()
					, category.c_str()
					, m_filePath.c_str()
					, m_codecType.c_str()
					);

				return false;
			}

			if( CONFIG_SERVICE(m_serviceProvider)->getValue( "Check", "ImageRowColumnTransparency", false ) == true )
			{
				if( s_checkRowColumnTransparency( buffer_memory, dataInfo->width, dataInfo->height ) == true )
				{
					LOGGER_ERROR(m_serviceProvider)("ResourceImageDefault::_isValid %s file %s:%s codec %s row or column pixels transparency!"
						, m_name.c_str()
						, category.c_str()
						, m_filePath.c_str()
						, m_codecType.c_str()
						);

					return false;
				}
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceImageDefault * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceImageDefault *>(_meta);
		        
        m_isAlpha = true;

        m_wrapU = false;
        m_wrapV = false;
        
        metadata->swap_File_Path( m_filePath );
        metadata->swap_File_Codec( m_codecType );

        if( m_codecType.empty() == true )
        {
            m_codecType = CODEC_SERVICE(m_serviceProvider)
                ->findCodecType( m_filePath );
        }

        metadata->get_File_Alpha( m_isAlpha );

        metadata->get_File_WrapX( m_wrapU );
        metadata->get_File_WrapY( m_wrapV );
        		
		metadata->get_File_MaxSize( m_maxSize );

		m_size = m_maxSize;
		metadata->get_File_Size( m_size );
		metadata->get_File_Offset( m_offset );

		m_validNoExist = false;
		metadata->get_File_NoExist( m_validNoExist );
		
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_compile()
	{				
		LOGGER_INFO(m_serviceProvider)("ResourceImageDefault::loadImageFrame_ %s load texture %s"
			, this->getName().c_str()
			, m_filePath.c_str()
			);

		const ConstString & category = this->getCategory();

		RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE(m_serviceProvider)
			->loadTexture( category, m_filePath, m_codecType );

		if( texture == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceImageDefault::loadImageFrame_: '%s' can't load image file '%s'"
				, this->getName().c_str()
				, m_filePath.c_str()
				);

			return false;
		}

		m_texture = texture;

		this->prepareImageFrame_();
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::_release()
	{
		float width = (float)m_texture->getWidth();
		float height = (float)m_texture->getHeight();

		const RenderImageInterfacePtr & image = m_texture->getImage();

		float hwWidth = (float)image->getHWWidth();
		float hwHeight = (float)image->getHWHeight();

		mt::vec2f uv_unscale;
		uv_unscale.x = hwWidth / width;
		uv_unscale.y = hwHeight / height;

		for( size_t i = 0; i != 4; ++i )
		{
			m_uv_image[i] *= uv_unscale;
		}

		ResourceImage::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::setup( const FilePath & _imagePath, const ConstString & _codecType, const mt::uv4f & _uv_image, const mt::uv4f & _uv_alpha, bool _wrapU, bool _wrapV )
	{
        m_filePath = _imagePath;

		if( _codecType.empty() == true )
		{
			m_codecType = CODEC_SERVICE(m_serviceProvider)
				->findCodecType( m_filePath );
		}

        m_uv_image = _uv_image;
		m_uv_alpha = _uv_alpha;

		m_wrapU = _wrapU;
		m_wrapV = _wrapV;

		m_texture = nullptr;
		m_textureAlpha = nullptr;
		
		m_isAlpha = true;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::prepareImageFrame_()
	{
		float width = (float)m_texture->getWidth();
		float height = (float)m_texture->getHeight();

		const RenderImageInterfacePtr & image = m_texture->getImage();

		float hwWidth = (float)image->getHWWidth();
		float hwHeight = (float)image->getHWHeight();

		mt::vec2f uv_scale;
		uv_scale.x = width / hwWidth;
		uv_scale.y = height / hwHeight;

		for( size_t i = 0; i != 4; ++i )
		{
			m_uv_image[i] *= uv_scale;
		}

		if( m_maxSize.x < 1.f || m_maxSize.y < 1.f )
		{
			m_maxSize.x = width;
			m_maxSize.y = height;

			m_size.x = m_maxSize.x;
			m_size.y = m_maxSize.y;
		}

		uint32_t channels = m_texture->getChannels();

		if( channels == 3 )
		{
			m_isAlpha = false;
		}
		else
		{
			m_isAlpha = true;
		}
	}
}

#include "ResourceImageDefault.h"

#include "Interface/FileSystemInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/ConfigInterface.h"

#include "Metacode/Metacode.h"

#include "Core/MemoryHelper.h"

#include "Logger/Logger.h"

#include "Core/ConstString.h"
#include "Core/String.h"

namespace Mengine
{
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

		bool exist = FILE_SERVICE()
			->existFile( category, m_filePath, nullptr );
		
		if( exist == false )
		{
			if( m_validNoExist == true )
			{
				return true;
			}

			LOGGER_ERROR("ResourceImageDefault::_isValid %s not exist file %s:%s"
				, m_name.c_str()
				, category.c_str()
				, m_filePath.c_str()
				);

			return false;
		}
        
        InputStreamInterfacePtr stream = FILE_SERVICE()
			->openInputFile( category, m_filePath, false );

        if( stream == nullptr )
        {
			LOGGER_ERROR("ResourceImageDefault::_isValid %s invalid open file %s:%s"
				, m_name.c_str()
				, category.c_str()
				, m_filePath.c_str()
				);

            return false;
        }

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
            ->createDecoderT<ImageDecoderInterfacePtr>( m_codecType );

        if( imageDecoder == nullptr )
        {
			LOGGER_ERROR("ResourceImageDefault::_isValid %s file %s:%s invalid decoder %s"
				, m_name.c_str()
				, category.c_str()
				, m_filePath.c_str()
				, m_codecType.c_str()
				);

            return false;
        }

		if( imageDecoder->prepareData( stream ) == false )
		{
			LOGGER_ERROR("ResourceImageDefault::_isValid %s file %s:%s decoder initialize failed %s"
				, m_name.c_str()
				, category.c_str()
				, m_filePath.c_str()
				, m_codecType.c_str()
				);

			return false;
		}

		const ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

		uint32_t limitTextureWidth = CONFIG_VALUE( "Limit", "TextureWidth", 2048U );
		uint32_t limitTextureHeight = CONFIG_VALUE( "Limit", "TextureHeight", 2048U );

		float width = (float)dataInfo->width;
		float height = (float)dataInfo->height;
		
		if( (width > limitTextureWidth && limitTextureWidth != 0U) || (height > limitTextureHeight && limitTextureHeight != 0U) )
		{
			LOGGER_ERROR("ResourceImageDefault::_isValid %s file %s:%s invalid limit %d:%d texture size %d:%d "
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
			LOGGER_ERROR("ResourceImage::_isValid: '%s' file '%s:%s' incorrect size %f:%f texture %f:%f"
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

		bool check_imageTransparency = CONFIG_VALUE( "Check", "ImageTransparency", false );

		if( check_imageTransparency == true && dataInfo->channels == 4 )
		{
			size_t texture_size = dataInfo->getSize();

			MemoryInterfacePtr buffer = Helper::createMemoryCacheBuffer( texture_size, __FILE__, __LINE__ );

			if( buffer == nullptr )
			{
				return false;
			}

			void * buffer_memory = buffer->getMemory();

			ImageCodecOptions options;
			options.channels = 4;
			options.pitch = dataInfo->width * options.channels;

			if( imageDecoder->setOptions( &options ) == false )
			{
				LOGGER_ERROR("ResourceImageDefault::_isValid %s file %s:%s invalid optionizing"
					, m_name.c_str()
					, category.c_str()
					, m_filePath.c_str()
					, m_codecType.c_str()
					);

				return false;
			}

			if( imageDecoder->decode( buffer_memory, texture_size ) == 0 )
			{
				LOGGER_ERROR("ResourceImageDefault::_isValid %s file %s:%s invalid decode %s"
					, m_name.c_str()
					, category.c_str()
					, m_filePath.c_str()
					, m_codecType.c_str()
					);

				return false;
			}

			if( s_allPixelsTransparency( buffer_memory, texture_size ) == true )
			{
				LOGGER_ERROR("ResourceImageDefault::_isValid %s file %s:%s codec %s all pixels transparency!"
					, m_name.c_str()
					, category.c_str()
					, m_filePath.c_str()
					, m_codecType.c_str()
					);

				return false;
			}

			bool check_imageRowColumnTransparency = CONFIG_VALUE( "Check", "ImageRowColumnTransparency", false );

			if( check_imageRowColumnTransparency == true )
			{
				if( s_checkRowColumnTransparency( buffer_memory, dataInfo->width, dataInfo->height ) == true )
				{
					LOGGER_ERROR("ResourceImageDefault::_isValid %s file %s:%s codec %s row or column pixels transparency!"
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
        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageDefault * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageDefault *>(_meta);
		       
        m_filePath = metadata->get_File_Path();

        metadata->get_File_Codec( &m_codecType );
		metadata->get_File_Converter( &m_converter );

		if( m_codecType.empty() == true )
		{
			m_codecType = CODEC_SERVICE()
				->findCodecType( m_filePath );
		}
        
		m_hasAlpha = true;
        metadata->get_File_Alpha( &m_hasAlpha );

		m_isPremultiply = false;
		metadata->get_File_Premultiply( &m_isPremultiply );
        		
		m_maxSize = metadata->get_File_MaxSize();

		m_size = m_maxSize;
		metadata->get_File_Size( &m_size );
		metadata->get_File_Offset( &m_offset );

		m_validNoExist = false;
		metadata->get_File_NoExist( &m_validNoExist );
		
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_convert()
	{
		bool result = this->convertDefault_( m_converter, m_filePath, m_filePath, m_codecType );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_compile()
	{				
		LOGGER_INFO("ResourceImageDefault::loadImageFrame_ %s load texture %s"
			, this->getName().c_str()
			, m_filePath.c_str()
			);

		const ConstString & category = this->getCategory();

		RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
			->loadTexture( category, m_filePath, m_codecType );

		if( texture == nullptr )
		{
			LOGGER_ERROR("ResourceImageDefault::loadImageFrame_: '%s' category '%s' group '%s' can't load image file '%s'"
				, this->getName().c_str()
				, this->getCategory().c_str()
				, this->getGroup().c_str()
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
        float width_inv = m_texture->getWidthInv();
        float height_inv = m_texture->getHeightInv();

		const RenderImageInterfacePtr & image = m_texture->getImage();

        float hwWidth = (float)image->getHWWidth();
        float hwHeight = (float)image->getHWHeight();

		mt::vec2f uv_unscale;
        uv_unscale.x = hwWidth * width_inv;
        uv_unscale.y = hwHeight * height_inv;

		for( size_t i = 0; i != 4; ++i )
		{
			m_uvImage[i] *= uv_unscale;
		}

		ResourceImage::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::setup( const FilePath & _imagePath, const ConstString & _codecType, const mt::uv4f & _uv_image, const mt::uv4f & _uv_alpha, const mt::vec2f & _maxSize )
	{
        m_filePath = _imagePath;

		if( _codecType.empty() == true )
		{
			m_codecType = CODEC_SERVICE()
				->findCodecType( m_filePath );
		}

        m_uvImage = _uv_image;
		m_uvAlpha = _uv_alpha;

		m_maxSize = _maxSize;
		m_size = m_maxSize;
		m_offset = mt::vec2f( 0.f, 0.f );

		m_texture = nullptr;
		m_textureAlpha = nullptr;
		
		m_hasAlpha = true;
		
		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::prepareImageFrame_()
	{
		float width = (float)m_texture->getWidth();
		float height = (float)m_texture->getHeight();

		const RenderImageInterfacePtr & image = m_texture->getImage();

		float hwWidthInv = image->getHWWidthInv();
		float hwHeightInv = image->getHWHeightInv();

		mt::vec2f uv_scale;
		uv_scale.x = width * hwWidthInv;
		uv_scale.y = height * hwHeightInv;

		for( size_t i = 0; i != 4; ++i )
		{
			m_uvImage[i] *= uv_scale;
		}

		uint32_t hwChannels = image->getHWChannels();

		if( hwChannels == 3 )
		{
			m_hasAlpha = false;
		}
		else
		{
			m_hasAlpha = true;
		}

        bool pow2 = m_texture->isPow2();

        this->setPow2( pow2 );
	}
}

#	include "ResourceImageDefault.h"

#	include "Kernel/ResourceImplement.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ConfigInterface.h"

#   include "Logger/Logger.h"

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

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceImageDefault * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceImageDefault *>(_meta);

        m_uv = mt::vec4f(0.f, 0.f, 1.f, 1.f);
        m_maxSize = mt::vec2f(0.f, 0.f);
        m_size = mt::vec2f(0.f, 0.f);
		m_offset = mt::vec2f(0.f, 0.f);
        m_isAlpha = true;
		m_isUVRotate = false;
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
		const ConstString & category = this->getCategory();
					
		if( this->loadImageFrame_( category, m_filePath, m_codecType ) == false )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::setImagePath( const FilePath& _imagePath )
	{
        m_filePath = _imagePath;

        m_codecType = CODEC_SERVICE(m_serviceProvider)
            ->findCodecType( _imagePath );

        m_texture = nullptr;
        m_uv = mt::vec4f(0.f,0.f,1.f,1.f);
		m_isAlpha = true;
        m_isUVRotate = false;
		m_wrapU = false;
		m_wrapV = false;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & ResourceImageDefault::getImagePath() const
	{
		return m_filePath;
	}
}

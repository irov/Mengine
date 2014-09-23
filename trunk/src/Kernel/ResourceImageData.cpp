#	include "ResourceImageData.h"

#	include "Kernel/ResourceImplement.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ConfigInterface.h"

#   include "Logger/Logger.h"

#	include "Core/ConstString.h"
#	include "Core/String.h"
#	include "Core/Memory.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageData );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageData::ResourceImageData()
		: m_maxSize(0.f, 0.f)
		, m_buffer(nullptr)
		, m_validNoExist(false)		
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & ResourceImageData::getFileName() const
	{
		return m_fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageData::getCodecType() const
	{
		return m_codecType;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageData::_isValid() const
	{
		const ConstString & category = this->getCategory();

		bool exist = FILE_SERVICE(m_serviceProvider)
			->existFile( category, m_fileName, nullptr );
		
		if( exist == false )
		{
			if( m_validNoExist == true )
			{
				return true;
			}

			LOGGER_ERROR(m_serviceProvider)("ResourceImageDefault::_isValid %s not exist file %s:%s"
				, m_name.c_str()
				, category.c_str()
				, m_fileName.c_str()
				);

			return false;
		}
        
        InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( category, m_fileName, false );

        if( stream == nullptr )
        {
			LOGGER_ERROR(m_serviceProvider)("ResourceImageDefault::_isValid %s invalid open file %s:%s"
				, m_name.c_str()
				, category.c_str()
				, m_fileName.c_str()
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
				, m_fileName.c_str()
				, m_codecType.c_str()
				);

            return false;
        }

		if( imageDecoder->prepareData( stream ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceImageDefault::_isValid %s file %s:%s decoder initialize failed %s"
				, m_name.c_str()
				, category.c_str()
				, m_fileName.c_str()
				, m_codecType.c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageData::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceImageData * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceImageData *>(_meta);
		        
        metadata->swap_File_Path( m_fileName );
        metadata->swap_File_Codec( m_codecType );

        if( m_codecType.empty() == true )
        {
            m_codecType = CODEC_SERVICE(m_serviceProvider)
                ->findCodecType( m_fileName );
        }
		        		
		m_maxSize = mt::vec2f(0.f, 0.f);
		metadata->get_File_MaxSize( m_maxSize );

		m_validNoExist = false;
		metadata->get_File_NoExist( m_validNoExist );
		
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageData::_compile()
	{	
		const ConstString & category = this->getCategory();
					
		InputStreamInterfacePtr stream = 
			FILE_SERVICE(m_serviceProvider)->openInputFile( category, m_fileName, false );

		if( stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceImageData::_compile: Image file '%s:%s' was not found"
				, category.c_str()
				, m_fileName.c_str() 
				);

			return false;
		}	

		ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(m_serviceProvider)
			->createDecoderT<ImageDecoderInterfacePtr>( m_codecType );

		if( imageDecoder == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceImageData::_compile: Image decoder '%s' for file '%s:%s' was not found"
				, m_codecType.c_str()
				, category.c_str()
				, m_fileName.c_str() 
				);

			return false;
		}

		if( imageDecoder->prepareData( stream ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceImageData::_compile: Image decoder '%s' for file '%s:%s' was not found"
				, m_codecType.c_str()
				, category.c_str()
				, m_fileName.c_str()  
				);

			return nullptr;
		}

		const ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

		uint32_t width = dataInfo->width;
		uint32_t height = dataInfo->height;
		uint32_t channels = dataInfo->channels;
		PixelFormat format = dataInfo->format;

		size_t memorySize = Helper::getTextureMemorySize( width, height, channels, 1, format );

		m_buffer = Helper::allocateMemory<unsigned char>( memorySize );

		ImageCodecOptions options;
		options.pitch = width * channels;
		options.channels = channels;

		if( imageDecoder->setOptions( &options ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceImageData::_compile: Image decoder '%s' for file '%s:%s' invalid optionize"
				, m_codecType.c_str()
				, category.c_str()
				, m_fileName.c_str()  
				);

			return false;
		}
		
		if( imageDecoder->decode( m_buffer, memorySize ) == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceImageData::_compile: Image decoder '%s' for file '%s:%s' invalid decode"
				, m_codecType.c_str()
				, category.c_str()
				, m_fileName.c_str()  
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageData::_release()
	{
		Helper::freeMemory( m_buffer );
		m_buffer = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageData::setImagePath( const FilePath & _imagePath )
	{
        m_fileName = _imagePath;

        m_codecType = CODEC_SERVICE(m_serviceProvider)
            ->findCodecType( _imagePath );

		m_maxSize = mt::vec2f(0.f, 0.f);
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & ResourceImageData::getImagePath() const
	{
		return m_fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageData::getImageMaxSize() const
	{
		return m_maxSize;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char * ResourceImageData::getImageBuffer() const
	{
		return m_buffer;
	}
}

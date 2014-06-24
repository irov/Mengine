#	include "ResourceImageDefault.h"

#	include "Kernel/ResourceImplement.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"
#	include "Interface/RenderSystemInterface.h"

#   include "Logger/Logger.h"

#	include "Core/ConstString.h"
#	include "Core/String.h"

#	include <cstdio>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageDefault );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDefault::ResourceImageDefault()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & ResourceImageDefault::getFileName() const
	{
		return m_fileName;
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
			->existFile( category, m_fileName, nullptr );
				
		if( exist == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceImageDefault::_isValid %s not exist file %s:%s"
				, m_name.c_str()
				, category.c_str()
				, m_fileName.c_str()
				);

			return false;
		}
        
        InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( category, m_fileName );

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

		//ArchivatorInterfacePtr archivator = ARCHIVE_SERVICE(m_serviceProvider)
		//	->getArchivator( CONST_STRING_LOCAL(m_serviceProvider, "zip") );

		//MemoryInputPtr memory = ARCHIVE_SERVICE(m_serviceProvider)
		//	->compressStream( archivator, stream );

		//size_t stream_size = stream->size();
		//size_t memory_size = memory->size();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceImageDefault * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceImageDefault *>(_meta);

        m_uv = mt::vec4f(0.f, 0.f, 1.f, 1.f);
        m_textureSize = mt::vec2f(0.f, 0.f);
        m_maxSize = mt::vec2f(0.f, 0.f);
        m_size = mt::vec2f(0.f, 0.f);
        m_isAlpha = true;
        m_isUVRotate = false;
        m_wrapU = false;
        m_wrapV = false;
        
        metadata->swap_File_Path( m_fileName );
        metadata->swap_File_Codec( m_codecType );

        if( m_codecType.empty() == true )
        {
            m_codecType = CODEC_SERVICE(m_serviceProvider)
                ->findCodecType( m_fileName );
        }

        metadata->get_File_UV( m_uv );
        metadata->get_File_Alpha( m_isAlpha );
        metadata->get_File_Rotate( m_isUVRotate );
        metadata->get_File_WrapX( m_wrapU );
        metadata->get_File_WrapY( m_wrapV );        
        metadata->get_File_TextureSize( m_textureSize );
        
		metadata->get_File_MaxSize( m_maxSize );
		
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_compile()
	{	
		const ConstString & category = this->getCategory();
					
		if( this->loadImageFrame_( category, m_fileName, m_codecType ) == false )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::setImagePath( const FilePath& _imagePath )
	{
        m_fileName = _imagePath;

        m_codecType = CODEC_SERVICE(m_serviceProvider)
            ->findCodecType( _imagePath );

        m_texture = nullptr;
        m_uv = mt::vec4f(0.f,0.f,1.f,1.f);
		m_isAlpha = true;
        m_isUVRotate = false;
		m_wrapU = false;
		m_wrapV = false;
	}
}

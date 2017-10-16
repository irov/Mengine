#	include "ResourceMovie2.h"

#	include "Interface/ResourceInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Menge/ResourceImageDefault.h"
#	include "Menge/ResourceVideo.h"
#	include "Menge/ResourceSound.h"

#	include "Metacode/Metacode.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static size_t Mengine_read_stream( void * _data, void * _buff, size_t _carriage, size_t _size )
	{
        (void)_carriage;

		InputStreamInterface * stream = (InputStreamInterface *)_data;

		size_t bytes = stream->read( _buff, _size );

		return bytes;
	}
	//////////////////////////////////////////////////////////////////////////
	static void Mengine_copy_stream( void * _data, const void * _src, void * _dst, size_t _size )
	{
		(void)_data;

		memcpy( _dst, _src, _size );
	}
    //////////////////////////////////////////////////////////////////////////
    static ae_voidptr_t Mengine_resource_provider( const aeMovieResource * _resource, void * _data )
    {
        ResourceMovie2 * resourceMovie2 = (ResourceMovie2 *)_data;

        uint8_t resource_type = _resource->type;

        switch( resource_type )
        {
        case AE_MOVIE_RESOURCE_IMAGE:
            {
                const aeMovieResourceImage * resource_image = (const aeMovieResourceImage *)_resource;

                ResourceReference * data_resource = resourceMovie2->createResourceImage_( resource_image->path, resource_image->trim_width, resource_image->trim_height );

                return data_resource;
            }break;
        case AE_MOVIE_RESOURCE_VIDEO:
            {
                const aeMovieResourceVideo * resource_video = (const aeMovieResourceVideo *)_resource;

                ResourceReference * data_resource = resourceMovie2->createResourceVideo_( resource_video );

                return data_resource;
            }
        case AE_MOVIE_RESOURCE_SOUND:
            {
                const aeMovieResourceSound * resource_sound = (const aeMovieResourceSound *)_resource;

                ResourceReference * data_resource = resourceMovie2->createResourceSound_( resource_sound );

                return data_resource;
            }
        }

        return AE_NULL;
    }
    //////////////////////////////////////////////////////////////////////////
    static void Mengine_resource_deleter( aeMovieResourceTypeEnum _type, const ae_voidptr_t * _data, ae_voidptr_t _ud )
    {
        (void)_type;
        (void)_data;
        (void)_ud;
    }
	//////////////////////////////////////////////////////////////////////////
	ResourceMovie2::ResourceMovie2()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceMovie2::~ResourceMovie2()
	{
	}	
	//////////////////////////////////////////////////////////////////////////
	const aeMovieData * ResourceMovie2::getMovieData() const
	{
		return m_movieData;
	}
	//////////////////////////////////////////////////////////////////////////
	const aeMovieCompositionData * ResourceMovie2::getCompositionData( const ConstString & _name ) const
	{
		if( this->isCompile() == false )
		{
			return nullptr;
		}

		const aeMovieCompositionData * compositionData = ae_get_movie_composition_data( m_movieData, _name.c_str() );

		if( compositionData == nullptr )
		{
			return nullptr;
		}
				
		return compositionData;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie2::setMovieInstance( aeMovieInstance * _instance )
	{
		m_instance = _instance;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie2::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceMovie2 * metadata
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceMovie2 *>(_meta);
               
        metadata->swap_File_Path( m_filePath );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMovie2::_compile()
	{
		if( ResourceReference::_compile() == false )
		{
			return false;
		}

		if( m_filePath.empty() == true )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceMovie::_compile: '%s' group '%s' don`t have Key Frames Pack Path"
				, this->getName().c_str()
				, this->getGroup().c_str()
				);

			return false;
		}

		const ConstString & category = this->getCategory();

		InputStreamInterfacePtr stream = FILE_SERVICE( m_serviceProvider )
			->openInputFile( category, m_filePath, false );

		if( stream == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("ResourceMovie2::_compile: '%s' group '%s' can`t open file '%s'"
				, this->getName().c_str()
				, this->getGroup().c_str()
				, m_filePath.c_str()
				);

			return false;
		}

		aeMovieData * movieData = ae_create_movie_data( m_instance, &Mengine_resource_provider, &Mengine_resource_deleter, this );

		aeMovieStream * movie_stream = ae_create_movie_stream( m_instance, &Mengine_read_stream, &Mengine_copy_stream, stream.get() );

		if( ae_load_movie_data( movieData, movie_stream ) != AE_MOVIE_SUCCESSFUL )
		{
			LOGGER_ERROR( m_serviceProvider )("ResourceMovie2::_compile: '%s' group '%s' invalid load data from file '%s'"
				, this->getName().c_str()
				, this->getGroup().c_str()
				, m_filePath.c_str()
				);

			return 0;
		}

		ae_delete_movie_stream( movie_stream );

		m_movieData = movieData;
		stream = nullptr;
	
		for( TVectorResources::const_iterator
			it = m_resources.begin(),
			it_end = m_resources.end();
		it != it_end;
		++it )
		{
			const ResourceReferencePtr & resource = *it;

			if( resource->compile() == false )
			{
				return false;
			}
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMovie2::_release()
	{
		ae_delete_movie_data( m_movieData );

		ResourceReference::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceMovie2::createResourceImage_( const ae_string_t _path, float _width, float _height )
	{
		ResourceImageDefaultPtr image = RESOURCE_SERVICE( m_serviceProvider )
			->generateResourceT<ResourceImageDefaultPtr>( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ResourceImageDefault" ) );

		const ConstString & category = this->getCategory();

		image->setCategory( category );

		PathString full_path;

		ConstString folder = Helper::getPathFolder( m_serviceProvider, m_filePath );

		full_path += folder.c_str();
		full_path += _path;

		FilePath c_path = Helper::stringizeFilePath( m_serviceProvider, full_path );

		mt::uv4f uv_image;
		mt::uv4f uv_alpha;

		mt::vec2f size( _width, _height );

		image->setup( c_path, ConstString::none(), uv_image, uv_alpha, size );

		m_resources.push_back( image );

		return image.get();
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceMovie2::createResourceVideo_( const aeMovieResourceVideo * _resource )
	{
		ResourceVideoPtr video = RESOURCE_SERVICE( m_serviceProvider )
			->generateResourceT<ResourceVideoPtr>( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ResourceVideo" ) );

		const ConstString & category = this->getCategory();

		video->setCategory( category );

		PathString full_path;

		ConstString folder = Helper::getPathFolder( m_serviceProvider, m_filePath );

		full_path += folder.c_str();
		full_path += _resource->path;

		FilePath fullPath = Helper::stringizeFilePath( m_serviceProvider, full_path );

		video->setFilePath( fullPath );

		video->setFrameRate( _resource->frameRate );
		video->setDuration( _resource->duration );

		if( _resource->alpha == AE_TRUE )
		{
			video->setAlpha( true );
			video->setCodecType( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ogvaVideo" ) );
		}
		else
		{
			video->setAlpha( false );
			video->setCodecType( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ogvVideo" ) );
		}

		m_resources.push_back( video );

		return video.get();
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference * ResourceMovie2::createResourceSound_( const aeMovieResourceSound * _resource )
	{
		ResourceSoundPtr sound = RESOURCE_SERVICE( m_serviceProvider )
			->generateResourceT<ResourceSoundPtr>( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ResourceSound" ) );

		const ConstString & category = this->getCategory();

		sound->setCategory( category );

		PathString full_path;

		ConstString folder = Helper::getPathFolder( m_serviceProvider, m_filePath );

		full_path += folder.c_str();
		full_path += _resource->path;

		FilePath c_path = Helper::stringizeFilePath( m_serviceProvider, full_path );
		
		sound->setFilePath( c_path );
		
		sound->setCodecType( STRINGIZE_STRING_LOCAL( m_serviceProvider, "oggSound" ) );

		m_resources.push_back( sound );

		return sound.get();
	}
}

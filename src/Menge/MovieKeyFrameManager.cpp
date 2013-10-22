#	include "MovieKeyFrameManager.h"

#	include "Interface/LoaderInterface.h"
#	include "Interface/ArchiveInterface.h"

#	include "Consts.h"

#	include "Archive/ArchiveRead.hpp"
#	include "Config/Blobject.h"

#	include "Logger/Logger.h"

#   include "Codec/ConverterFactory.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( MovieKeyFrameService, Menge::MovieKeyFrameServiceInterface, Menge::MovieKeyFrameManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	////////////////////////////////////////////////////////////////////////////////////////////
	MovieKeyFrameManager::MovieKeyFrameManager()
        : m_serviceProvider(nullptr)
	{
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	MovieKeyFrameManager::~MovieKeyFrameManager()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void MovieKeyFrameManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * MovieKeyFrameManager::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MovieKeyFrameManager::initialize()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieKeyFrameManager::finalize()
	{
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	MovieFramePackInterface * MovieKeyFrameManager::getMovieFramePak( const ConstString & _pak, const FilePath & _path )
	{
		MovieFramePackInterface * framePak = this->createMovieFramePak_( _pak, _path );
		 
		if( framePak == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)( "MovieKeyFrameManager: can`t load frame pak category '%s' path '%s'"
				, _pak.c_str()
				, _path.c_str()
				);

			return nullptr;
		}

		return framePak;
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	MovieFramePackInterface * MovieKeyFrameManager::createMovieFramePak_( const ConstString & _pak, const FilePath & _path )
	{
		InputStreamInterfacePtr input_stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( _pak, _path );

		if( input_stream == nullptr )
		{
			return false;
		}

		size_t binary_aek_header;
		input_stream->read( &binary_aek_header, sizeof(binary_aek_header) );

		if( binary_aek_header != 0xAEAEBABE )
		{
			LOGGER_ERROR(m_serviceProvider)( "MovieKeyFrameManager::createMovieFramePak_: aek invalid header category '%s' path '%s'"
				, _pak.c_str()
				, _path.c_str()
				);

			return nullptr;
		}

		size_t binary_aek_version_valid = 2;

		size_t binary_aek_version;
		input_stream->read( &binary_aek_version, sizeof(binary_aek_version) );

		if( binary_aek_version != binary_aek_version_valid )
		{
			LOGGER_ERROR(m_serviceProvider)( "MovieKeyFrameManager::createMovieFramePak_: aek invalid version %d:%d category '%s' path '%s'"
				, binary_aek_version
				, binary_aek_version_valid
				, _pak.c_str()
				, _path.c_str()
				);

			return nullptr;
		}

		size_t binary_aek_size;
		input_stream->read( &binary_aek_size, sizeof(binary_aek_size) );

		size_t compress_size;
		input_stream->read( &compress_size, sizeof(compress_size) );

		static TBlobject compress;
		compress.resize(compress_size);

		input_stream->read( &compress[0], compress_size );

		static TBlobject binary_aek;
		binary_aek.resize(binary_aek_size);

		size_t uncompressSize = 0;
		if( ARCHIVE_SERVICE(m_serviceProvider)
			->uncompress( &binary_aek[0], binary_aek_size, uncompressSize, &compress[0], compress_size ) == false )
		{
			return false;
		}

		ArchiveRead ar(binary_aek.begin(), binary_aek.end());

		ar.begin();

		size_t layers_framepack_size;
		ar >> layers_framepack_size;

		MovieFramePack * pack = m_poolMovieFramePack.createObjectT();

		pack->initialize( layers_framepack_size );
		
		for( size_t it_layer = 0; it_layer != layers_framepack_size; ++it_layer )
		{
			bool immutable;
			ar >> immutable;

			if( immutable == true )
			{
				MovieFrameSource frame;
				ar >> frame;

				pack->initializeLayer( it_layer + 1, 0, true );
				pack->setLayerImmutableFrame( it_layer + 1, frame );
			}
			else
			{
				size_t frames_size;
				ar >> frames_size;

				pack->initializeLayer( it_layer + 1, frames_size, false );

				if( frames_size > 0 )
				{
					TVectorMovieFrameSource & frames = pack->mutableLayerFrames( it_layer + 1 );
					frames.resize(frames_size);

					ar.readPODs( &frames[0], frames_size );
				}
			}
		}

		size_t layers_timeremap_size;
		ar >> layers_timeremap_size;

		for( size_t it_layer = 0; it_layer != layers_timeremap_size; ++it_layer )
		{
			size_t times_size;
			ar >> times_size;

			if( times_size > 0 )
			{
				MovieLayerTimeRemap & timeremap = pack->mutableLayerTimeRemap( it_layer + 1 );
				timeremap.times.resize( times_size );

				ar.readPODs( &timeremap.times[0], times_size );
			}
		}

		size_t layers_shapes_size;
		ar >> layers_shapes_size;

		for( size_t it_layer = 0; it_layer != layers_shapes_size; ++it_layer )
		{
			size_t shapes_size;
			ar >> shapes_size;

			if( shapes_size > 0 )
			{
				MovieLayerShapes & shapes = pack->mutableLayerShape( it_layer + 1 );
				shapes.shapes.resize( shapes_size );

				for( size_t it_shapes = 0; it_shapes != shapes_size; ++it_shapes )
				{
					MovieFrameShape & shape = shapes.shapes[it_shapes];

					ar >> shape.vertexCount;

					if( shape.vertexCount > 0 )
					{
						ar.readPODs( shape.pos, shape.vertexCount );
						ar.readPODs( shape.uv, shape.vertexCount );

						ar >> shape.indexCount;
						ar.readPODs( shape.indecies, shape.indexCount );
					}
				}
			}
		}

		return pack;
	}
	////////////////////////////////////////////////////////////////////////////////
}

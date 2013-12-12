#	include "DataflowAEK.h"

#	include "Interface/ArchiveInterface.h"

#	include "Archive/ArchiveRead.hpp"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DataflowAEK::DataflowAEK()
		: m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DataflowAEK::~DataflowAEK()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void DataflowAEK::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * DataflowAEK::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	DataInterfacePtr DataflowAEK::load( const InputStreamInterfacePtr & _stream )
	{
		if( _stream == nullptr )
		{
			return nullptr;
		}

		size_t binary_aek_header;
		_stream->read( &binary_aek_header, sizeof(binary_aek_header) );

		if( binary_aek_header != 0xAEAEBABE )
		{
			LOGGER_ERROR(m_serviceProvider)( "DataflowAEK::load: aek invalid header"
				);

			return nullptr;
		}

		size_t binary_aek_version_valid = DATAFLOW_VERSION_AEK;

		size_t binary_aek_version;
		_stream->read( &binary_aek_version, sizeof(binary_aek_version) );

		if( binary_aek_version != binary_aek_version_valid )
		{
			LOGGER_ERROR(m_serviceProvider)( "DataflowAEK::load: aek invalid version %d:%d"
				, binary_aek_version
				, binary_aek_version_valid
				);

			return nullptr;
		}

		size_t binary_aek_size;
		_stream->read( &binary_aek_size, sizeof(binary_aek_size) );

		size_t compress_size;
		_stream->read( &compress_size, sizeof(compress_size) );

		static TBlobject compress;
		compress.resize(compress_size);

		_stream->read( &compress[0], compress_size );

		m_cacheBinary.resize(binary_aek_size);

		TBlobject::value_type * cacheBinaryBuffer = &m_cacheBinary[0];
		TBlobject::value_type * compressBuffer = &compress[0];

		size_t uncompressSize = 0;
		if( ARCHIVE_SERVICE(m_serviceProvider)
			->uncompress( cacheBinaryBuffer, binary_aek_size, uncompressSize, compressBuffer, compress_size ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "DataflowAEK::load: aek invalid uncompress"
				);

			return nullptr;
		}

		ArchiveRead ar(m_cacheBinary.begin(), m_cacheBinary.end());

		ar.begin();

		size_t maxIndex;
		ar >> maxIndex;

		MovieFramePack * pack = m_poolMovieFramePack.createObjectT();

		pack->initialize( maxIndex );

		for( size_t it_layer = 0; it_layer != maxIndex; ++it_layer )
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

		for( size_t it_layer = 0; it_layer != maxIndex; ++it_layer )
		{
			size_t times_size;
			ar >> times_size;

			if( times_size == 0 )
			{
				continue;
			}
			
			MovieLayerTimeRemap & timeremap = pack->mutableLayerTimeRemap( it_layer + 1 );
			timeremap.times.resize( times_size );

			ar.readPODs( &timeremap.times[0], times_size );
		}

		for( size_t it_layer = 0; it_layer != maxIndex; ++it_layer )
		{
			size_t shapes_size;
			ar >> shapes_size;

			if( shapes_size == 0 )
			{
				continue;
			}
			
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
				else
				{
					shape.indexCount = 0;
				}
			}
		}

		return pack;
	}
}
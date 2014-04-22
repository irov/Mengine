#	include "DataflowAEK.h"

#	include "Interface/ArchiveInterface.h"
#	include "Interface/CacheInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Core/BlobjectRead.h"
#	include "Core/CacheMemoryBuffer.h"

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
	DataInterfacePtr DataflowAEK::create()
	{
		MovieFramePack * pack = m_poolMovieFramePack.createObjectT();

		return pack;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DataflowAEK::load( const DataInterfacePtr & _data, const InputStreamInterfacePtr & _stream )
	{
		uint32_t binary_aek_header;
		_stream->read( &binary_aek_header, sizeof(binary_aek_header) );

		if( binary_aek_header != 0xAEAEBABE )
		{
			LOGGER_ERROR(m_serviceProvider)("DataflowAEK::load: aek invalid magic header"
				);

			return false;
		}

		uint32_t binary_aek_version_valid = DATAFLOW_VERSION_AEK;

		uint32_t binary_aek_version;
		_stream->read( &binary_aek_version, sizeof(binary_aek_version) );

		if( binary_aek_version != binary_aek_version_valid )
		{
			LOGGER_ERROR(m_serviceProvider)("DataflowAEK::load: aek invalid version %d:%d"
				, binary_aek_version
				, binary_aek_version_valid
				);

			return false;
		}

		uint32_t binary_size;
		_stream->read( &binary_size, sizeof(binary_size) );

		uint32_t compress_size;
		_stream->read( &compress_size, sizeof(compress_size) );

		CacheMemoryBuffer binary_buffer(m_serviceProvider, binary_size, "DataflowAEK_binary");
		TBlobject::value_type * binary_memory = binary_buffer.getMemoryT<TBlobject::value_type>();

		size_t uncompressSize = 0;
		if( ARCHIVE_SERVICE(m_serviceProvider)
			->decompress( CONST_STRING_LOCAL(zip), _stream, compress_size, binary_memory, binary_size, uncompressSize ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("DataflowAEK::load: aek invalid uncompress"
				);

			return false;
		}

		BlobjectRead ar(binary_memory, binary_size);

		size_t maxIndex;
		ar >> maxIndex;

		MovieFramePack * pack = stdex::intrusive_get<MovieFramePack>(_data);

		pack->initialize( maxIndex );

		for( size_t it_layer = 0; it_layer != maxIndex; ++it_layer )
		{
			bool immutable;
			ar >> immutable;

			size_t frames_size;
			ar >> frames_size;
						
			MovieLayerFrame & frame = pack->initializeLayer( it_layer + 1, frames_size, immutable );

			if( frames_size == 0 )
			{
				continue;
			}

			if( immutable == true )
			{
				ar >> frame.source;
			}
			else
			{
#	define READ_FRAME_SOURCE( Type, Member, Mask )\
				{ \
					bool value_immutable; \
					ar >> value_immutable; \
					if( value_immutable == true ) \
					{ \
						ar >> frame.source.Member; \
						frame.immutable_mask |= Mask; \
					} \
					else \
					{ \
						frame.Member = new Type [frames_size]; \
						ar.readPODs( frame.Member, frames_size );\
					} \
				}
			
				READ_FRAME_SOURCE( mt::vec3f, anchorPoint, MOVIE_KEY_FRAME_IMMUTABLE_ANCHOR_POINT );
				READ_FRAME_SOURCE( mt::vec3f, position, MOVIE_KEY_FRAME_IMMUTABLE_POSITION );
				READ_FRAME_SOURCE( mt::vec3f, rotation, MOVIE_KEY_FRAME_IMMUTABLE_ROTATION );
				READ_FRAME_SOURCE( mt::vec3f, scale, MOVIE_KEY_FRAME_IMMUTABLE_SCALE );
				READ_FRAME_SOURCE( float, opacity, MOVIE_KEY_FRAME_IMMUTABLE_OPACITY );
				READ_FRAME_SOURCE( float, volume, MOVIE_KEY_FRAME_IMMUTABLE_VOLUME );

#	undef READ_FRAME_SOURCE
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

			float * times_buff = &timeremap.times[0];
			ar.readPODs( times_buff, times_size );
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
					ar.readPODs( shape.indices, shape.indexCount );
				}
				else
				{
					shape.indexCount = 0;
				}
			}
		}

		return true;
	}
}
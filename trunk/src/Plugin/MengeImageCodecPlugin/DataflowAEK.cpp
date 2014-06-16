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
	bool DataflowAEK::initialize()
	{
		m_archivator = ARCHIVE_SERVICE(m_serviceProvider)
			->getArchivator( CONST_STRING_LOCAL(m_serviceProvider, "zip") );

		if( m_archivator == nullptr )
		{
			return false;
		}

		return true;
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
		unsigned char * binary_memory;
		size_t binary_size;

		uint32_t bufferId = CACHE_SERVICE(m_serviceProvider)
			->getArchiveData( _stream, m_archivator, GET_MAGIC_NUMBER(MAGIC_AEK), GET_MAGIC_VERSION(MAGIC_AEK), &binary_memory, binary_size );

		if( bufferId == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("DataflowAEK::load: invalid get data"
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

		CACHE_SERVICE(m_serviceProvider)
			->unlockBuffer( bufferId );

		return true;
	}
}
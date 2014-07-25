#	include "DataflowAEK.h"

#	include "Interface/ArchiveInterface.h"
#	include "Interface/CacheInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Core/BlobjectRead.h"
#	include "Core/CacheMemoryBuffer.h"

#	include "stdex/memory_reader.h"

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

		MovieFramePack * pack = stdex::intrusive_get<MovieFramePack>(_data);

		bool successful = this->loadBuffer_( pack, binary_memory, binary_size );

		CACHE_SERVICE(m_serviceProvider)
			->unlockBuffer( bufferId );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DataflowAEK::loadBuffer_( MovieFramePack * _pack, const unsigned char * _buffer, size_t _size )
	{
		try
		{
			size_t binary_read = 0;
			stdex::memory_reader ar(_buffer, _size, binary_read);

			uint32_t maxIndex;
			ar << maxIndex;
			
			_pack->initialize( maxIndex );

			for( uint32_t it_layer = 0; it_layer != maxIndex; ++it_layer )
			{
				uint32_t frames_size;
				ar << frames_size;

				uint8_t immutable;
				ar << immutable;
									
				MovieLayerFrame & frame = _pack->initializeLayer( it_layer, frames_size, (bool)immutable );

				if( frames_size == 0 )
				{
					continue;
				}

				if( immutable == 1 )
				{
					ar << frame.source.anchorPoint;
					ar << frame.source.position;
					ar << frame.source.rotation;
					ar << frame.source.scale;
					ar << frame.source.opacity;
					ar << frame.source.volume;
				}
				else
				{
	#	define READ_FRAME_SOURCE( Type, Member1, Member2, Mask )\
					{ \
						uint8_t value_immutable; \
						ar << value_immutable; \
						if( value_immutable == 1 ) \
						{ \
							ar << frame.source.Member1; \
							frame.immutable_mask |= Mask; \
						} \
						else \
						{ \
							frame.Member2 = new Type [frames_size]; \
							ar.readPODs( frame.Member2, frames_size );\
						} \
					}
			
					READ_FRAME_SOURCE( mt::vec3f, anchorPoint, anchorPoint, MOVIE_KEY_FRAME_IMMUTABLE_ANCHOR_POINT );
					READ_FRAME_SOURCE( mt::vec3f, position, position, MOVIE_KEY_FRAME_IMMUTABLE_POSITION );
					READ_FRAME_SOURCE( float, rotation.x, rotation_x, MOVIE_KEY_FRAME_IMMUTABLE_ROTATION_X );
					READ_FRAME_SOURCE( float, rotation.y, rotation_y, MOVIE_KEY_FRAME_IMMUTABLE_ROTATION_Y );
					READ_FRAME_SOURCE( float, rotation.z, rotation_z, MOVIE_KEY_FRAME_IMMUTABLE_ROTATION_Z );
					READ_FRAME_SOURCE( mt::vec3f, scale, scale, MOVIE_KEY_FRAME_IMMUTABLE_SCALE );
					READ_FRAME_SOURCE( float, opacity, opacity, MOVIE_KEY_FRAME_IMMUTABLE_OPACITY );
					READ_FRAME_SOURCE( float, volume, volume, MOVIE_KEY_FRAME_IMMUTABLE_VOLUME );

	#	undef READ_FRAME_SOURCE
				}
			}

			uint32_t times_count;
			ar << times_count;

			_pack->initializeTimeremap( times_count );

			for( uint32_t i = 0; i != times_count; ++i )
			{			
				MovieLayerTimeRemap & timeremap = _pack->mutableLayerTimeRemap( i );

				uint32_t layerId;
				ar << layerId;

				timeremap.layerId = layerId;

				uint32_t times_size;
				ar << times_size;

				if( times_size == 0 )
				{
					continue;
				}

				timeremap.times.resize( times_size );

				float * times_buff = &timeremap.times[0];
				ar.readPODs( times_buff, times_size );
			}

			uint32_t shapes_count;
			ar << shapes_count;

			_pack->initializeShapes( shapes_count );

			for( uint32_t i = 0; i != shapes_count; ++i )
			{
				MovieLayerShapes & shapes = _pack->mutableLayerShape( i );

				uint32_t layerId;
				ar << layerId;

				shapes.layerId = layerId;

				uint32_t shapes_size;
				ar << shapes_size;

				if( shapes_size == 0 )
				{
					continue;
				}

				shapes.shapes.resize( shapes_size );

				for( size_t j = 0; j != shapes_size; ++j )
				{
					MovieFrameShape & shape = shapes.shapes[j];

					ar << shape.vertexCount;

					if( shape.vertexCount > 0 )
					{
						ar.readPODs( shape.pos, shape.vertexCount );
						ar.readPODs( shape.uv, shape.vertexCount );

						ar << shape.indexCount;
						ar.readPODs( shape.indices, shape.indexCount );
					}
					else
					{
						shape.indexCount = 0;
					}
				}
			}

			uint32_t polygons_count;
			ar << polygons_count;

			_pack->initializePolygons( polygons_count );

			for( uint32_t i = 0; i != polygons_count; ++i )
			{
				MovieLayerPolygon & polygon = _pack->mutableLayerPolygon( i );

				uint32_t layerId;
				ar << layerId;

				polygon.layerId = layerId;

				uint8_t vertexCount;
				ar << vertexCount;

				polygon.vertexCount = vertexCount;

				for( size_t j = 0; j != vertexCount; ++j )
				{
					mt::vec2f v;
					ar << v.x;
					ar << v.y;

					polygon.polygon[j] = v;
				}
			}
						
			if( _size != binary_read )
			{
				return false;
			}
		}
		catch( const stdex::memory_reader_exception & )
		{
			return false;
		}

		return true;
	}
}
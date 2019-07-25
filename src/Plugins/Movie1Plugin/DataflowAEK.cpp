#include "DataflowAEK.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Stream.h"
#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"

#include "Metacode/Metacode.h"

#include "stdex/memory_reader.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DataflowAEK::DataflowAEK()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DataflowAEK::~DataflowAEK()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowAEK::initialize()
    {
        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator, false );

        m_archivator = archivator;

        m_factoryMovieFramePack = new FactoryPool<MovieFramePack, 32>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowAEK::finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMovieFramePack );

        m_factoryMovieFramePack = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowAEK::isThreadFlow() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    DataInterfacePtr DataflowAEK::create( const Char * _doc )
    {
        MovieFramePackPtr data = m_factoryMovieFramePack->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data, nullptr );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DataflowAEK::load( const InputStreamInterfacePtr & _stream, const Char * _doc )
    {
        MemoryInterfacePtr memory = Helper::loadStreamArchiveData( _stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_AEK ), GET_MAGIC_VERSION( MAGIC_AEK ), _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowAEK::flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const Char * _doc )
    {
        MovieFramePack * pack = stdex::intrusive_get<MovieFramePack *>( _data );

        void * binaryBuffer_memory = _memory->getBuffer();
        size_t binaryBuffer_size = _memory->getSize();

        if( this->loadBuffer_( pack, binaryBuffer_memory, binaryBuffer_size ) == false )
        {
            LOGGER_ERROR( "invalid load buffer (doc: %s)"
                , _doc
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowAEK::loadBuffer_( MovieFramePack * _pack, const void * _buffer, size_t _size )
    {
        try
        {
            size_t binary_read = 0;
            stdex::memory_reader ar( _buffer, _size, binary_read );

            uint32_t metacode_version;
            ar << metacode_version;

            uint32_t true_metacode_version = Metacode::get_metacode_version();

            if( true_metacode_version != metacode_version )
            {
                LOGGER_ERROR( "invalid metacode version %d (need %d)"
                    , metacode_version
                    , true_metacode_version
                );

                return false;
            }

            uint32_t format_version;
            ar << format_version;

            if( format_version != MOVIE_KEY_FRAME_FORMAT_VERSION )
            {
                LOGGER_ERROR( "invalid format version %d (need %d)"
                    , format_version
                    , MOVIE_KEY_FRAME_FORMAT_VERSION
                );

                return false;
            }

            uint32_t maxIndex;
            ar << maxIndex;

            if( maxIndex != 0 )
            {
                _pack->initializeLayers( maxIndex );

                for( uint32_t it_layer = 0; it_layer != maxIndex; ++it_layer )
                {
                    uint32_t frames_size;
                    ar << frames_size;

                    uint8_t immutable;
                    ar << immutable;

                    MovieLayerFrame & frame = _pack->setupLayer( it_layer, frames_size, !!immutable );

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
#define READ_FRAME_SOURCE( Type, Member1, Member2, Mask )\
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
							frame.Member2 = _pack->allocateMemoryT<Type>( frames_size ); \
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
            }

            uint32_t times_count;
            ar << times_count;

            if( times_count != 0 )
            {
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

                    timeremap.times = _pack->allocateMemoryT<float>( times_size );

                    float * times_buff = &timeremap.times[0];
                    ar.readPODs( times_buff, times_size );
                }
            }

            uint32_t shapes_count;
            ar << shapes_count;

            if( shapes_count != 0 )
            {
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

                    shapes.shapes_size = shapes_size;
                    shapes.shapes = _pack->allocateMemoryT<MovieFrameShape>( shapes_size );

                    for( uint32_t j = 0; j != shapes_size; ++j )
                    {
                        MovieFrameShape & shape = shapes.shapes[j];

                        ar << shape.vertexCount;

                        if( shape.vertexCount > 0 )
                        {
                            shape.pos = _pack->allocateMemoryT<mt::vec2f>( shape.vertexCount );
                            shape.uv = _pack->allocateMemoryT<mt::vec2f>( shape.vertexCount );

                            ar.readPODs( shape.pos, shape.vertexCount );
                            ar.readPODs( shape.uv, shape.vertexCount );

                            ar << shape.indexCount;

                            shape.indices = _pack->allocateMemoryT<RenderIndex>( shape.indexCount );

                            ar.readPODs( shape.indices, shape.indexCount );
                        }
                        else
                        {
                            shape.indexCount = 0;

                            shape.pos = nullptr;
                            shape.uv = nullptr;
                            shape.indices = nullptr;
                        }
                    }
                }
            }

            uint32_t polygons_count;
            ar << polygons_count;

            if( polygons_count != 0 )
            {
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
                    polygon.polygon = _pack->allocateMemoryT<mt::vec2f>( vertexCount );

                    for( uint32_t j = 0; j != vertexCount; ++j )
                    {
                        mt::vec2f v;
                        ar << v.x;
                        ar << v.y;

                        polygon.polygon[j] = v;
                    }
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
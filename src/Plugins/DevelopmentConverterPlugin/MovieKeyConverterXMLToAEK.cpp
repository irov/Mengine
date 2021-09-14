#include "MovieKeyConverterXMLToAEK.h"

#include "Interface/LoaderServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/DataServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Plugins/Movie1Plugin/MovieKeyFrameInterface.h"

#include "Metacode/Metacode.h"

#include "Kernel/PolygonHelper.h"
#include "Kernel/Stream.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/PathString.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/MemoryAllocator.h"
#include "Kernel/Vector.h"
#include "Kernel/ContainerWriter.h"
#include "Kernel/FileGroupHelper.h"

#include "Config/Iterator.h"

#include "math/quat.h"

#include "poly2tri/poly2tri.h"

//////////////////////////////////////////////////////////////////////////
void * _metabuf_malloc( size_t _size )
{
    void * p = Mengine::Helper::allocateMemory( _size, "metabuf_aek" );

    return p;
}
//////////////////////////////////////////////////////////////////////////
void _metabuf_free( void * _ptr )
{
    Mengine::Helper::deallocateMemory( _ptr, "metabuf_aek" );
}
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<MovieFrameSource> VectorMovieFrameSource;
    //////////////////////////////////////////////////////////////////////////
    struct ConverterMovieLayerFrame
    {
        VectorMovieFrameSource frames;
        uint32_t count;

        MovieFrameSource source;
        uint8_t immutable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<ConverterMovieLayerFrame> VectorConverterMovieFrameLayer;
    //////////////////////////////////////////////////////////////////////////
    MovieKeyConverterXMLToAEK::MovieKeyConverterXMLToAEK()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MovieKeyConverterXMLToAEK::~MovieKeyConverterXMLToAEK()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool MovieKeyConverterXMLToAEK::_initialize()
    {
        m_convertExt = STRINGIZE_STRING_LOCAL( ".aek" );

        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator );

        m_archivator = archivator;

        m_dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "aekMovie" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( m_dataflow );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieKeyConverterXMLToAEK::_finalize()
    {
        m_archivator = nullptr;
        m_dataflow = nullptr;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool MovieKeyConverterXMLToAEK::convert()
    {
        Blobject buffer;
        if( this->loadFramePackage_( buffer ) == false )
        {
            return false;
        }

        if( this->writeFramePackage_( buffer ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MovieKeyConverterXMLToAEK::validateVersion( const InputStreamInterfacePtr & _stream ) const
    {
        MovieFramePackInterfacePtr framePack = DATA_SERVICE()
            ->dataflow( m_dataflow, _stream, nullptr, MENGINE_DOCUMENT_FACTORABLE );

        if( framePack == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MovieKeyConverterXMLToAEK::loadFramePackage_( Blobject & _buffer )
    {
        Metacode::Meta_Data::Meta_KeyFramesPack keyFramesPack;

        PathString binPath;

        binPath += m_options.inputFilePath;
        binPath.replace_last( "bin" );

        FilePath path_bin = Helper::stringizeFilePath( binPath );

        bool exist = false;
        if( LOADER_SERVICE()
            ->load( m_options.fileGroup, path_bin, &keyFramesPack, Metacode::Meta_Data::getVersion(), &exist ) == false )
        {
            if( exist == false )
            {
                LOGGER_ERROR( "KeyFramesFile '%s' not found"
                    , m_options.inputFilePath.c_str()
                );
            }
            else
            {
                LOGGER_ERROR( "KeyFramesFile invalid parse '%s' "
                    , m_options.inputFilePath.c_str()
                );
            }

            return false;
        }

        stdex::container_writer<Blobject> aw( _buffer );

        uint32_t true_metacode_version = Metacode::get_metacode_version();

        aw << true_metacode_version;

        uint32_t keyframes_version = keyFramesPack.get_Version();

        aw << keyframes_version;

        uint32_t maxIndex = keyFramesPack.get_MaxIndex();

        aw << maxIndex;

        VectorConverterMovieFrameLayer frameLayers;
        frameLayers.resize( maxIndex );

        for( ConverterMovieLayerFrame & layer : frameLayers )
        {
            layer.count = 0;
            layer.immutable = 0;
        }

        const Metacode::Meta_Data::Meta_KeyFramesPack::VectorMeta_KeyFrames2D & includes_frames2d = keyFramesPack.get_Includes_KeyFrames2D();

        for( Metacode::Meta_Data::Meta_KeyFramesPack::VectorMeta_KeyFrames2D::const_iterator
            it = includes_frames2d.begin(),
            it_end = includes_frames2d.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_KeyFramesPack::Meta_KeyFrames2D & meta_frames2d = *it;

            uint32_t layerIndex = meta_frames2d.get_LayerIndex();

            ConverterMovieLayerFrame & frameLayer = frameLayers[layerIndex - 1];

            bool immutable = false;
            meta_frames2d.get_Immutable( &immutable );

            frameLayer.immutable = immutable ? 1 : 0;

            if( frameLayer.immutable == 1 )
            {
                const Metacode::Meta_Data::Meta_KeyFramesPack::Meta_KeyFrames2D::VectorMeta_KeyFrame2D & includes_frame2d = meta_frames2d.get_Includes_KeyFrame2D();

                for( Metacode::Meta_Data::Meta_KeyFramesPack::Meta_KeyFrames2D::VectorMeta_KeyFrame2D::const_iterator
                    it_frame = includes_frame2d.begin(),
                    it_frame_end = includes_frame2d.end();
                    it_frame != it_frame_end;
                    ++it_frame )
                {
                    const Metacode::Meta_Data::Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D & meta_frame2d = *it_frame;

                    mt::vec2f anchorPoint2d( 0.f, 0.f );
                    mt::vec2f position2d( 0.f, 0.f );
                    mt::vec2f scale2d( 0.f, 0.f );
                    float angle = 0.f;
                    float opacity = 1.f;

                    uint32_t count = 1;

                    float volume = 1.f;

                    meta_frame2d.get_AnchorPoint( &anchorPoint2d );
                    meta_frame2d.get_Position( &position2d );
                    meta_frame2d.get_Scale( &scale2d );
                    meta_frame2d.get_Rotation( &angle );
                    meta_frame2d.get_Opacity( &opacity );
                    meta_frame2d.get_Count( &count );
                    meta_frame2d.get_Volume( &volume );

                    MovieFrameSource frame;
                    frame.anchorPoint = mt::vec3f( anchorPoint2d, 0.f );
                    frame.position = mt::vec3f( position2d, 0.f );
                    frame.scale = mt::vec3f( scale2d, 1.f );
                    frame.opacity = opacity;
                    frame.volume = volume;

                    frame.rotation.x = -angle;
                    frame.rotation.y = 0.f;
                    frame.rotation.z = 0.f;

                    frameLayer.source = frame;
                    frameLayer.count = count;
                }
            }
            else
            {
                uint32_t count = 0;
                meta_frames2d.get_Count( &count );

                frameLayer.count = count;

                MovieFrameSource frame;
                frame.anchorPoint = mt::vec3f( 0.f, 0.f, 0.f );
                frame.position = mt::vec3f( 0.f, 0.f, 0.f );
                frame.rotation = mt::vec3f( 0.f, 0.f, 0.f );
                frame.scale = mt::vec3f( 1.f, 1.f, 1.f );
                frame.opacity = 0.f;
                frame.volume = 0.f;

                bool frameLast = false;

                const Metacode::Meta_Data::Meta_KeyFramesPack::Meta_KeyFrames2D::VectorMeta_KeyFrame2D & includes_frame2d = meta_frames2d.get_Includes_KeyFrame2D();

                for( Metacode::Meta_Data::Meta_KeyFramesPack::Meta_KeyFrames2D::VectorMeta_KeyFrame2D::const_iterator
                    it_frame = includes_frame2d.begin(),
                    it_frame_end = includes_frame2d.end();
                    it_frame != it_frame_end;
                    ++it_frame )
                {
                    const Metacode::Meta_Data::Meta_KeyFramesPack::Meta_KeyFrames2D::Meta_KeyFrame2D & meta_frame2d = *it_frame;

                    uint32_t count_frame = 1;

                    mt::vec2f anchorPoint2d( 0.f, 0.f );
                    mt::vec2f position2d( 0.f, 0.f );
                    mt::vec2f scale2d( 1.f, 1.f );

                    float angle = 0.f;

                    float volume = 1.f;

                    if( frameLast == true )
                    {
                        angle = -frame.rotation.x;

                        anchorPoint2d = frame.anchorPoint.to_vec2f();
                        position2d = frame.position.to_vec2f();
                        scale2d = frame.scale.to_vec2f();
                        volume = frame.volume;
                    }
                    else
                    {
                        frameLast = true;
                    }

                    meta_frame2d.get_AnchorPoint( &anchorPoint2d );
                    meta_frame2d.get_Position( &position2d );
                    meta_frame2d.get_Scale( &scale2d );
                    meta_frame2d.get_Rotation( &angle );
                    meta_frame2d.get_Opacity( &frame.opacity );
                    meta_frame2d.get_Count( &count_frame );
                    meta_frame2d.get_Volume( &volume );

                    frame.anchorPoint = mt::vec3f( anchorPoint2d, 0.f );
                    frame.position = mt::vec3f( position2d, 0.f );
                    frame.scale = mt::vec3f( scale2d, 1.f );

                    frame.volume = volume;

                    frame.rotation.x = -angle;
                    frame.rotation.y = 0.f;
                    frame.rotation.z = 0.f;

                    for( uint32_t i = 0; i != count_frame; ++i )
                    {
                        frameLayer.frames.emplace_back( frame );
                    }
                }

                if( frameLayer.frames.size() != frameLayer.count )
                {
                    return false;
                }
            }
        }

        const Metacode::Meta_Data::Meta_KeyFramesPack::VectorMeta_KeyFrames3D & includes_frames3d = keyFramesPack.get_Includes_KeyFrames3D();

        for( Metacode::Meta_Data::Meta_KeyFramesPack::VectorMeta_KeyFrames3D::const_iterator
            it = includes_frames3d.begin(),
            it_end = includes_frames3d.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_KeyFramesPack::Meta_KeyFrames3D & meta_frames3d = *it;

            uint32_t layerIndex = meta_frames3d.get_LayerIndex();

            ConverterMovieLayerFrame & frameLayer = frameLayers[layerIndex - 1];

            uint32_t count = 0;
            meta_frames3d.get_Count( &count );

            frameLayer.count = count;

            bool immutable = false;
            meta_frames3d.get_Immutable( &immutable );

            frameLayer.immutable = immutable ? 1 : 0;

            MovieFrameSource frame;
            frame.anchorPoint = mt::vec3f( 0.f, 0.f, 0.f );
            frame.position = mt::vec3f( 0.f, 0.f, 0.f );
            frame.rotation = mt::vec3f( 0.f, 0.f, 0.f );
            frame.scale = mt::vec3f( 1.f, 1.f, 1.f );
            frame.opacity = 0.f;
            frame.volume = 0.f;

            mt::vec3f rotation( 0.f, 0.f, 0.f );
            mt::vec3f orientation( 0.f, 0.f, 0.f );

            const Metacode::Meta_Data::Meta_KeyFramesPack::Meta_KeyFrames3D::VectorMeta_KeyFrame3D & includes_frame3d = meta_frames3d.get_Includes_KeyFrame3D();

            for( Metacode::Meta_Data::Meta_KeyFramesPack::Meta_KeyFrames3D::VectorMeta_KeyFrame3D::const_iterator
                it_frame = includes_frame3d.begin(),
                it_frame_end = includes_frame3d.end();
                it_frame != it_frame_end;
                ++it_frame )
            {
                const Metacode::Meta_Data::Meta_KeyFramesPack::Meta_KeyFrames3D::Meta_KeyFrame3D & meta_frame3d = *it_frame;

                uint32_t count_frame = 1;

                meta_frame3d.get_AnchorPoint( &frame.anchorPoint );
                meta_frame3d.get_Position( &frame.position );
                meta_frame3d.get_Rotation( &rotation );
                meta_frame3d.get_Orientation( &orientation );
                meta_frame3d.get_Scale( &frame.scale );
                meta_frame3d.get_Opacity( &frame.opacity );
                meta_frame3d.get_Count( &count_frame );

                frame.volume = 1.f;
                meta_frame3d.get_Volume( &frame.volume );

                mt::quatf qo;
                mt::make_quat_from_euler( qo, orientation );

                mt::quatf qr;
                mt::make_quat_from_euler( qr, rotation );

                mt::quatf qor;
                mt::mul_q_q( qor, qo, qr );

                mt::quat_to_euler( qor, frame.rotation );

                if( frameLayer.immutable == 0 )
                {
                    for( uint32_t i = 0; i != count_frame; ++i )
                    {
                        frameLayer.frames.emplace_back( frame );
                    }
                }
                else
                {
                    frameLayer.source = frame;
                    frameLayer.count = count_frame;
                }
            }

            if( frameLayer.immutable == 0 )
            {
                if( frameLayer.frames.size() != frameLayer.count )
                {
                    return false;
                }
            }
        }

        for( const ConverterMovieLayerFrame & frame : frameLayers )
        {
            aw << frame.count;
            aw << frame.immutable;

            if( frame.count == 0 )
            {
                continue;
            }

            if( frame.immutable == 1 )
            {
                aw << frame.source.anchorPoint;
                aw << frame.source.position;
                aw << frame.source.rotation;
                aw << frame.source.scale;
                aw << frame.source.opacity;
                aw << frame.source.volume;
            }
            else
            {
                const VectorMovieFrameSource & frames = frame.frames;

#define MENGINE_WRITE_FRAME_SOURCE( Type, Member )\
            { \
                Type value = frames[0].Member; \
                uint8_t value_immutable = 1; \
                for( uint32_t i = 1; i != frame.count; ++i ) \
                { \
                    const MovieFrameSource & source = frames[i]; \
                    if( source.Member == value ) \
                    { \
                        continue; \
                    } \
                    value_immutable = 0; \
                    break; \
                } \
                aw << value_immutable; \
                if( value_immutable == 1 ) \
                { \
                    aw << value; \
                } \
                else \
                { \
                    for( uint32_t i = 0; i != frame.count; ++i ) \
                    { \
                        const MovieFrameSource & source = frames[i]; \
                        aw << source.Member; \
                    } \
                } \
            }

                MENGINE_WRITE_FRAME_SOURCE( mt::vec3f, anchorPoint );
                MENGINE_WRITE_FRAME_SOURCE( mt::vec3f, position );
                MENGINE_WRITE_FRAME_SOURCE( float, rotation.x );
                MENGINE_WRITE_FRAME_SOURCE( float, rotation.y );
                MENGINE_WRITE_FRAME_SOURCE( float, rotation.z );
                MENGINE_WRITE_FRAME_SOURCE( mt::vec3f, scale );
                MENGINE_WRITE_FRAME_SOURCE( float, opacity );
                MENGINE_WRITE_FRAME_SOURCE( float, volume );

#undef MENGINE_WRITE_FRAME_SOURCE
            }
        }

        const Metacode::Meta_Data::Meta_KeyFramesPack::VectorMeta_TimeRemap & includes_timeremaps = keyFramesPack.get_Includes_TimeRemap();

        uint32_t remapsSize = (uint32_t)includes_timeremaps.size();
        aw << remapsSize;

        for( Metacode::Meta_Data::Meta_KeyFramesPack::VectorMeta_TimeRemap::const_iterator
            it = includes_timeremaps.begin(),
            it_end = includes_timeremaps.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_KeyFramesPack::Meta_TimeRemap & meta_timeremap = *it;

            uint32_t layerIndex = meta_timeremap.get_LayerIndex();

            aw << layerIndex;

            const Floats & floats = meta_timeremap.get_Time();

            uint32_t floats_size = (uint32_t)floats.size();
            aw << floats_size;

            if( floats_size > 0 )
            {
                aw.writePODs( &floats[0], floats_size );
            }
        }

        const Metacode::Meta_Data::Meta_KeyFramesPack::VectorMeta_ImageShape & includes_imageshapes = keyFramesPack.get_Includes_ImageShape();

        uint32_t shapesSize = (uint32_t)includes_imageshapes.size();
        aw << shapesSize;

        for( Metacode::Meta_Data::Meta_KeyFramesPack::VectorMeta_ImageShape::const_iterator
            it = includes_imageshapes.begin(),
            it_end = includes_imageshapes.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_KeyFramesPack::Meta_ImageShape & meta_imageshape = *it;

            uint32_t layerIndex = meta_imageshape.get_LayerIndex();

            aw << layerIndex;

            const mt::vec2f & imageMaxSize = meta_imageshape.get_ImageMaxSize();

            mt::vec2f imageSize;
            if( meta_imageshape.get_ImageSize( &imageSize ) == false )
            {
                imageSize = imageMaxSize;
            }

            mt::vec2f imageOffset( 0.f, 0.f );
            meta_imageshape.get_ImageOffset( &imageOffset );

            bool subtract = false;
            meta_imageshape.get_Subtract( &subtract );

            const Metacode::Meta_Data::Meta_KeyFramesPack::Meta_ImageShape::VectorMeta_Shape & includes_shapes = meta_imageshape.get_Includes_Shape();

            uint32_t includes_shapes_size = (uint32_t)includes_shapes.size();

            aw << includes_shapes_size;

            for( Metacode::Meta_Data::Meta_KeyFramesPack::Meta_ImageShape::VectorMeta_Shape::const_iterator
                it_shape = includes_shapes.begin(),
                it_shape_end = includes_shapes.end();
                it_shape != it_shape_end;
                ++it_shape )
            {
                const Metacode::Meta_Data::Meta_KeyFramesPack::Meta_ImageShape::Meta_Shape & meta_shape = *it_shape;

                const Polygon & polygon = meta_shape.get_Polygon();

                mt::vec2f v0( 0.f, 0.f );
                mt::vec2f v1( imageMaxSize.x, 0.f );
                mt::vec2f v2( imageMaxSize.x, imageMaxSize.y );
                mt::vec2f v3( 0.f, imageMaxSize.y );

                Polygon imagePolygon;
                imagePolygon.append( v0 );
                imagePolygon.append( v1 );
                imagePolygon.append( v2 );
                imagePolygon.append( v3 );

                VectorGeolygon output;
                if( subtract == false )
                {
                    if( Helper::intersection( polygon, imagePolygon, &output ) == false )
                    {
                        LOGGER_ERROR( "layer [%u] shapes invalid"
                            , layerIndex
                        );

                        return false;
                    }
                }
                else
                {
                    if( Helper::difference( imagePolygon, polygon, &output ) == false )
                    {
                        LOGGER_ERROR( "layer [%u] shapes invalid"
                            , layerIndex
                        );

                        return false;
                    }
                }

                MovieFrameShape shape;
                if( output.empty() == true )
                {
                    shape.pos = nullptr;
                    shape.uv = nullptr;
                    shape.indices = nullptr;

                    shape.vertexCount = 0;
                    shape.indexCount = 0;

                    aw << shape.vertexCount;
                }
                else
                {
                    Vector<p2t::Point> p2t_points;

                    uint32_t max_points = 0;

                    for( const Geolygon & geolygon : output )
                    {
                        const Polygon & outer = geolygon.getOuter();

                        Polygon::size_type outer_count = outer.size();

                        max_points += outer_count;

                        const VectorPolygon & inners = geolygon.getInners();

                        for( const Polygon & inner : inners )
                        {
                            uint32_t inner_count = inner.size();

                            max_points += inner_count;
                        }
                    }

                    if( max_points >= MENGINE_MOVIE_SHAPE_MAX_VERTEX )
                    {
                        LOGGER_ERROR( "layer [%u] vertex overflow [%u] (max %u)"
                            , layerIndex
                            , max_points
                            , MENGINE_MOVIE_SHAPE_MAX_VERTEX
                        );

                        return false;
                    }

                    p2t_points.reserve( max_points );

                    VectorPolygonIndices shape_indices;

                    for( const Geolygon & geolygon : output )
                    {
                        //p2t::CDT take only std::vector
                        std::vector<p2t::Point *> p2t_polygon;

                        const Polygon & outer = geolygon.getOuter();

                        for( const mt::vec2f & v : outer )
                        {
                            p2t_points.emplace_back( p2t::Point( v.x, v.y ) );
                            p2t::Point * p = &p2t_points.back();

                            p2t_polygon.emplace_back( p );
                        }

                        p2t::CDT cdt( p2t_polygon );

                        const VectorPolygon & inners = geolygon.getInners();

                        for( const Polygon & inner : inners )
                        {
                            //cdt::AddHole take only std::vector
                            std::vector<p2t::Point *> p2t_hole;

                            for( const mt::vec2f & v : inner )
                            {
                                p2t_points.emplace_back( p2t::Point( v.x, v.y ) );
                                p2t::Point * p = &p2t_points.back();

                                p2t_hole.emplace_back( p );
                            }

                            cdt.AddHole( p2t_hole );
                        }

                        cdt.Triangulate();

                        //cdt::GetTriangles return std::vector
                        std::vector<p2t::Triangle *> triangles = cdt.GetTriangles();

                        for( p2t::Triangle * tr : triangles )
                        {
                            p2t::Point * p0 = tr->GetPoint( 0 );
                            p2t::Point * p1 = tr->GetPoint( 1 );
                            p2t::Point * p2 = tr->GetPoint( 2 );

                            p2t::Point * pb = p2t_points.data();

                            uint32_t i0 = (uint32_t)Iterator::distance( pb, p0 );
                            uint32_t i1 = (uint32_t)Iterator::distance( pb, p1 );
                            uint32_t i2 = (uint32_t)Iterator::distance( pb, p2 );

                            shape_indices.emplace_back( i0 );
                            shape_indices.emplace_back( i1 );
                            shape_indices.emplace_back( i2 );
                        }
                    }

                    Vector<p2t::Point>::size_type shapeVertexCount = p2t_points.size();
                    VectorPolygonIndices::size_type shapeIndicesCount = shape_indices.size();

                    if( shapeIndicesCount >= MENGINE_MOVIE_SHAPE_MAX_INDICES )
                    {
                        LOGGER_ERROR( "layer %u index overflow %u (max %u)"
                            , layerIndex
                            , shapeIndicesCount
                            , MENGINE_MOVIE_SHAPE_MAX_INDICES
                        );

                        return false;
                    }

                    shape.vertexCount = (uint16_t)shapeVertexCount;
                    shape.indexCount = (uint16_t)shapeIndicesCount;

                    shape.pos = Helper::allocateArrayT<mt::vec2f>( shapeVertexCount );
                    shape.uv = Helper::allocateArrayT<mt::vec2f>( shapeVertexCount );

                    shape.indices = Helper::allocateArrayT<RenderIndex>( shapeIndicesCount );

                    for( uint32_t i = 0; i != shapeVertexCount; ++i )
                    {
                        const p2t::Point & shape_pos = p2t_points[i];

                        shape.pos[i].x = (float)shape_pos.x;
                        shape.pos[i].y = (float)shape_pos.y;

                        shape.uv[i].x = ((float)shape_pos.x - imageOffset.x) / imageSize.x;
                        shape.uv[i].y = ((float)shape_pos.y - imageOffset.y) / imageSize.y;
                    }

                    for( uint32_t i = 0; i != shapeIndicesCount; ++i )
                    {
                        shape.indices[i] = (RenderIndex)shape_indices[i];
                    }

                    aw << shape.vertexCount;

                    if( shape.vertexCount > 0 )
                    {
                        aw.writePODs( shape.pos, shape.vertexCount );
                        aw.writePODs( shape.uv, shape.vertexCount );

                        aw << shape.indexCount;

                        aw.writePODs( shape.indices, shape.indexCount );
                    }

                    Helper::freeArrayT( shape.pos );
                    Helper::freeArrayT( shape.uv );
                    Helper::freeArrayT( shape.indices );
                }
            }
        }

        const Metacode::Meta_Data::Meta_KeyFramesPack::VectorMeta_Polygon & includes_polygons = keyFramesPack.get_Includes_Polygon();

        uint32_t polygonsSize = (uint32_t)includes_polygons.size();
        aw << polygonsSize;

        for( Metacode::Meta_Data::Meta_KeyFramesPack::VectorMeta_Polygon::const_iterator
            it = includes_polygons.begin(),
            it_end = includes_polygons.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_KeyFramesPack::Meta_Polygon & meta_polygon = *it;

            uint32_t layerIndex = meta_polygon.get_LayerIndex();

            aw << layerIndex;

            const Polygon & polygon = meta_polygon.get_Value();

            uint32_t polygon_size = polygon.size();

            if( polygon_size >= MENGINE_MOVIE_POLYGON_MAX_VERTEX )
            {
                LOGGER_ERROR( "layer %u polygon vertex overflow %u (max %u)"
                    , layerIndex
                    , polygon_size
                    , MENGINE_MOVIE_POLYGON_MAX_VERTEX
                );

                return false;
            }

            aw << (uint8_t)polygon_size;

            if( polygon_size > 0 )
            {
                for( const mt::vec2f & p : polygon )
                {
                    aw << p.x;
                    aw << p.y;
                }
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MovieKeyConverterXMLToAEK::writeFramePackage_( const Blobject & _buffer )
    {
        OutputStreamInterfacePtr output_stream = Helper::openOutputStreamFile( m_options.fileGroup, m_options.outputFilePath, true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( output_stream, "invalid open file '%s'"
            , Helper::getFileGroupFullPath( m_options.fileGroup, m_options.outputFilePath )
        );

        const void * buffer_memory = _buffer.data();
        size_t buffer_size = _buffer.size();

        if( Helper::writeStreamArchiveData( output_stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_AEK ), GET_MAGIC_VERSION( MAGIC_AEK ), false, buffer_memory, buffer_size, EAC_BEST ) == false )
        {
            LOGGER_ERROR( "invalid write stream '%s'"
                , Helper::getFileGroupFullPath( m_options.fileGroup, m_options.outputFilePath )
            );

            return false;
        }

        return true;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
}
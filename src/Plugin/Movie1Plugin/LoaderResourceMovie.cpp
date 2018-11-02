#include "LoaderResourceMovie.h"

#include "ResourceMovie.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceMovie::LoaderResourceMovie()        
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceMovie::~LoaderResourceMovie()
    {
    }    
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceMoviePtr resource = stdex::intrusive_static_cast<ResourceMoviePtr>(_loadable);

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie *>(_meta);

        m_duration = metadata->get_Duration_Value();
        m_frameDuration = metadata->get_FrameDuration_Value();
        m_size.x = metadata->get_Width_Value();
        m_size.y = metadata->get_Height_Value();

        metadata->get_Loop_Segment( &m_loopSegment );

        m_hasBoundBox = metadata->get_Bounds_Box( &m_boundBox );
        m_hasAnchorPoint = metadata->get_Anchor_Point( &m_anchorPoint );
        m_hasOffsetPoint = metadata->get_Offset_Point( &m_offsetPoint );

        m_filePath = metadata->get_KeyFramesPackPath_Path();

        metadata->get_KeyFramesPackPath_Codec( &m_dataflowType );
        metadata->get_KeyFramesPackPath_Converter( &m_converterType );

        //FIX THIS
        if( m_dataflowType.empty() == true )
        {
            m_converterType = STRINGIZE_STRING_LOCAL( "xmlToAekMovie" );
        }

        m_layers.clear();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie::VectorMeta_MovieLayer2D & includes_layer2d = metadata->get_Includes_MovieLayer2D();

        for( Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie::VectorMeta_MovieLayer2D::const_iterator
            it = includes_layer2d.begin(),
            it_end = includes_layer2d.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D & meta_layer2d = *it;

            m_layers.emplace_back( MovieLayer() );
            MovieLayer & ml = m_layers.back();

            ml.state = 0;

            ml.name = meta_layer2d.get_Name();
            ml.source = meta_layer2d.get_Source();
            ml.type = meta_layer2d.get_Type();

            ml.index = meta_layer2d.get_Index();
            ml.in = meta_layer2d.get_In();
            ml.out = meta_layer2d.get_Out();

            meta_layer2d.get_Parent( &ml.parent );
            meta_layer2d.get_StartInterval( &ml.startInterval );
            meta_layer2d.get_BlendingMode( &ml.blendingMode );

            meta_layer2d.get_TimeRemap( &ml.timeRemap );
            meta_layer2d.get_Shape( &ml.shape );
            meta_layer2d.get_Polygon( &ml.polygon );
            meta_layer2d.get_PlayCount( &ml.playCount );
            meta_layer2d.get_Stretch( &ml.stretch );
            meta_layer2d.get_Switch( &ml.switcher );
            meta_layer2d.get_Loop( &ml.loop );
            meta_layer2d.get_AnchorPoint( &ml.anchorPoint );
            meta_layer2d.get_Position( &ml.position );
            meta_layer2d.get_Scale( &ml.scale );
            meta_layer2d.get_Rotation( &ml.rotation );
            ml.hasViewport = meta_layer2d.get_Viewport( &ml.viewport );
            meta_layer2d.get_Params( &ml.params );

            ml.scale.z = 1.f;

            if( ml.loop == false )
            {
                if( ml.in < 0.f )
                {
                    ml.startInterval -= ml.in;
                    ml.in = 0.f;
                }

                if( ml.out > m_duration )
                {
                    ml.out = m_duration;
                }
            }
            else
            {
                ml.startInterval = 0.f;

                ml.in = 0.f;
                ml.out = m_duration;
            }
        }

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie::VectorMeta_MovieLayer3D & includes_layer3d = metadata->get_Includes_MovieLayer3D();

        for( Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie::VectorMeta_MovieLayer3D::const_iterator
            it = includes_layer3d.begin(),
            it_end = includes_layer3d.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D & meta_layer3d = *it;

            m_layers.emplace_back( MovieLayer() );
            MovieLayer & ml = m_layers.back();

            ml.state |= MOVIE_LAYER_THREED;

            ml.name = meta_layer3d.get_Name();
            ml.source = meta_layer3d.get_Source();
            ml.type = meta_layer3d.get_Type();

            ml.index = meta_layer3d.get_Index();
            ml.in = meta_layer3d.get_In();
            ml.out = meta_layer3d.get_Out();

            meta_layer3d.get_Parent( &ml.parent );
            meta_layer3d.get_StartInterval( &ml.startInterval );
            meta_layer3d.get_BlendingMode( &ml.blendingMode );

            meta_layer3d.get_TimeRemap( &ml.timeRemap );
            meta_layer3d.get_Shape( &ml.shape );
            meta_layer3d.get_Polygon( &ml.polygon );
            meta_layer3d.get_PlayCount( &ml.playCount );
            meta_layer3d.get_Stretch( &ml.stretch );
            meta_layer3d.get_Switch( &ml.switcher );
            meta_layer3d.get_Loop( &ml.loop );
            meta_layer3d.get_AnchorPoint( &ml.anchorPoint );
            meta_layer3d.get_Position( &ml.position );
            meta_layer3d.get_Scale( &ml.scale );
            meta_layer3d.get_Rotation( &ml.rotation );
            meta_layer3d.get_Params( &ml.params );

            if( ml.loop == false )
            {
                if( ml.in < 0.f )
                {
                    ml.startInterval -= ml.in;
                    ml.in = 0.f;
                }

                if( ml.out > m_duration )
                {
                    ml.out = m_duration;
                }
            }
            else
            {
                ml.startInterval = 0.f;

                ml.in = 0.f;
                ml.out = m_duration;
            }
        }


        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie::VectorMeta_MovieCamera3D & includes_camera3d = metadata->get_Includes_MovieCamera3D();

        for( Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie::VectorMeta_MovieCamera3D::const_iterator
            it = includes_camera3d.begin(),
            it_end = includes_camera3d.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D & meta_camera3d = *it;

            m_camera3D.cameraPosition = meta_camera3d.get_CameraPosition();
            m_camera3D.cameraInterest = meta_camera3d.get_CameraInterest();
            m_camera3D.cameraFOV = meta_camera3d.get_CameraFOV();
            m_camera3D.cameraAspect = meta_camera3d.get_CameraAspect();
            m_camera3D.width = meta_camera3d.get_Width();
            m_camera3D.height = meta_camera3d.get_Height();

            m_hasCamera3D = true;
        }

        for( MovieLayer & layer : m_layers )
        {
            m_maxLayerIndex = m_maxLayerIndex > layer.index ? m_maxLayerIndex : layer.index;

            if( layer.type == STRINGIZE_STRING_LOCAL( "MovieSlot" ) )
            {
                layer.state |= MOVIE_LAYER_NODE;
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieSceneEffect" ) )
            {
                layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_SCENE_EFFECT;

                layer.parent = movie_layer_parent_none;
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieText" ) )
            {
                layer.state |= MOVIE_LAYER_NODE;
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieTextCenter" ) )
            {
                layer.state |= MOVIE_LAYER_NODE;
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieNullObject" ) )
            {
                layer.state |= MOVIE_LAYER_NODE;
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "Image" ) )
            {
                layer.state |= MOVIE_LAYER_NODE;

                if( layer.shape == true )
                {
                    layer.state |= MOVIE_LAYER_MESH_2D;
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "SolidSprite" ) )
            {
                layer.state |= MOVIE_LAYER_NODE;

                if( layer.shape == true )
                {
                    layer.state |= MOVIE_LAYER_MESH_2D;
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieSocketImage" ) )
            {
                layer.state |= MOVIE_LAYER_NODE;
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieSocketShape" ) )
            {
                layer.state |= MOVIE_LAYER_NODE;
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "Animation" ) )
            {
                layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE;
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "Video" ) )
            {
                layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE;
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "Sound" ) )
            {
                layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE | MOVIE_LAYER_AUDIO;
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "SoundId" ) )
            {
                layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE | MOVIE_LAYER_AUDIO | MOVIE_LAYER_UNSTOPPABLE;
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieSprite" ) )
            {
                layer.state |= MOVIE_LAYER_NODE;
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "ParticleEmitter" ) )
            {
                layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE;
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ) )
            {
                layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE;
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieParticle" ) )
            {
                //Empty
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "Movie" ) )
            {
                layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE | MOVIE_LAYER_MOVIE;
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "SubMovie" ) )
            {
                layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_ANIMATABLE | MOVIE_LAYER_MOVIE | MOVIE_LAYER_SUB_MOVIE;
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieInternalObject" ) )
            {
                layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_INTERNAL;
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieEvent" ) )
            {
                layer.state |= MOVIE_LAYER_EXTRA;
            }
            else
            {
                LOGGER_ERROR( "ResourceMovie: '%s' group '%s' can't setup layer2d '%s' type '%s'"
                    , this->getName().c_str()
                    , this->getGroupName().c_str()
                    , layer.source.c_str()
                    , layer.type.c_str()
                );

                return false;
            }

            if( mt::equal_f_z( layer.in ) == true &&
                mt::equal_f_f_e( layer.out, m_duration, m_frameDuration ) == true &&
                mt::equal_f_z( layer.startInterval ) == true )
            {
                layer.loop = true;
            }
        }

        for( MovieLayer & layer : m_layers )
        {
            if( this->isThreeDNode( layer.index ) == true )
            {
                layer.state |= MOVIE_LAYER_THREED;
            }
        }

        m_frameCount = (uint32_t)((m_duration / m_frameDuration) + 0.5f);

        return true;
    }
}
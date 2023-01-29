#include "LoaderResourceMovie.h"

#include "Interface/VocabularyServiceInterface.h"

#include "ResourceMovie.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {        
        static bool isThreeDNode( const VectorMovieLayers & _layers, uint32_t _index )
        {
            for( const MovieLayer & layer : _layers )
            {
                if( layer.index != _index )
                {
                    continue;
                }

                if( layer.isThreeD() == true )
                {
                    return true;
                }

                if( layer.parent == 0 || layer.parent == movie_layer_parent_none )
                {
                    return false;
                }

                bool threeD = Detail::isThreeDNode( _layers, layer.parent );

                return threeD;
            }

            return false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceMovie::LoaderResourceMovie()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceMovie::~LoaderResourceMovie()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceMovie::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceMovie * resource = _loadable.getT<ResourceMovie *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie *>(_meta);

        float duration = metadata->get_Duration_Value();
        resource->setDuration( duration );

        float frameDuration = metadata->get_FrameDuration_Value();
        resource->setFrameDuration( frameDuration );

        float width = metadata->get_Width_Value();
        float height = metadata->get_Height_Value();
        resource->setSize( mt::vec2f( width, height ) );

        metadata->getm_Loop_Segment( resource, &ResourceMovie::setLoopSegment );
        metadata->getm_Bounds_Box( resource, &ResourceMovie::setBoundBox );
        metadata->getm_Anchor_Point( resource, &ResourceMovie::setAnchorPoint );
        metadata->getm_Offset_Point( resource, &ResourceMovie::setOffsetPoint );

        const ContentInterfacePtr & content = resource->getContent();

        metadata->getm_KeyFramesPackPath_Path( content.get(), &ContentInterface::setFilePath );
        metadata->getm_KeyFramesPackPath_Codec( content.get(), &ContentInterface::setCodecType );
        metadata->getm_KeyFramesPackPath_Converter( content.get(), &ContentInterface::setConverterType );

        const ConstString & codecType = content->getCodecType();

        //FIX THIS
        if( codecType.empty() == true )
        {
            content->setConverterType( STRINGIZE_STRING_LOCAL( "xmlToAekMovie" ) );
        }


        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), codecType );
        content->setDataflow( dataflow );

        VectorMovieLayers layers;

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie::VectorMeta_MovieLayer2D & includes_layer2d = metadata->get_Includes_MovieLayer2D();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer2D & meta_layer2d : includes_layer2d )
        {
            MovieLayer ml;

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

                if( ml.out > duration )
                {
                    ml.out = duration;
                }
            }
            else
            {
                ml.startInterval = 0.f;

                ml.in = 0.f;
                ml.out = duration;
            }

            layers.emplace_back( ml );
        }

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie::VectorMeta_MovieLayer3D & includes_layer3d = metadata->get_Includes_MovieLayer3D();

        for( Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie::VectorMeta_MovieLayer3D::const_iterator
            it = includes_layer3d.begin(),
            it_end = includes_layer3d.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie::Meta_MovieLayer3D & meta_layer3d = *it;

            MovieLayer ml;

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

                if( ml.out > duration )
                {
                    ml.out = duration;
                }
            }
            else
            {
                ml.startInterval = 0.f;

                ml.in = 0.f;
                ml.out = duration;
            }

            layers.emplace_back( ml );
        }


        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie::VectorMeta_MovieCamera3D & includes_camera3d = metadata->get_Includes_MovieCamera3D();

        for( Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie::VectorMeta_MovieCamera3D::const_iterator
            it = includes_camera3d.begin(),
            it_end = includes_camera3d.end();
            it != it_end;
            ++it )
        {
            const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie::Meta_MovieCamera3D & meta_camera3d = *it;

            MovieLayerCamera3D camera3D;
            camera3D.cameraPosition = meta_camera3d.get_CameraPosition();
            camera3D.cameraInterest = meta_camera3d.get_CameraInterest();
            camera3D.cameraFOV = meta_camera3d.get_CameraFOV();
            camera3D.cameraAspect = meta_camera3d.get_CameraAspect();
            camera3D.width = meta_camera3d.get_Width();
            camera3D.height = meta_camera3d.get_Height();

            resource->setCamera3D( camera3D );
        }

        uint32_t maxLayerIndex = 0U;

        for( MovieLayer & layer : layers )
        {
            maxLayerIndex = maxLayerIndex > layer.index ? maxLayerIndex : layer.index;

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
#ifdef MENGINE_USE_SCRIPT_SERVICE
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieInternalObject" ) )
            {
                layer.state |= MOVIE_LAYER_NODE | MOVIE_LAYER_INTERNAL;
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieEvent" ) )
            {
                layer.state |= MOVIE_LAYER_EXTRA;
            }
#endif
            else
            {
                LOGGER_ERROR( "'%s' group '%s' can't setup layer2d '%s' type '%s'"
                    , resource->getName().c_str()
                    , resource->getGroupName().c_str()
                    , layer.source.c_str()
                    , layer.type.c_str()
                );

                return false;
            }

            if( mt::equal_f_z( layer.in ) == true &&
                mt::equal_f_f_e( layer.out, duration, frameDuration ) == true &&
                mt::equal_f_z( layer.startInterval ) == true )
            {
                layer.loop = true;
            }
        }

        resource->setMaxLayerIndex( maxLayerIndex );

        for( MovieLayer & layer : layers )
        {
            if( Detail::isThreeDNode( layers, layer.index ) == true )
            {
                layer.state |= MOVIE_LAYER_THREED;
            }
        }

        resource->setLayers( layers );

        uint32_t frameCount = (uint32_t)((duration / frameDuration) + 0.5f);

        resource->setFrameCount( frameCount );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
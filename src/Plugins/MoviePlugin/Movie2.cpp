#include "Movie2.h"

#include "Interface/TimelineServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/RenderSystemInterface.h"

#include "Plugins/AstralaxParticlePlugin/UnknownParticleEmitterInterface.h"
#include "Plugins/VideoPlugin/VideoUnknownInterface.h"

#include "Movie2Data.h"

#include "Engine/SurfaceImage.h"
#include "Engine/SurfaceSound.h"
#include "Engine/SurfaceTrackMatte.h"

#include "Kernel/Materialable.h"
#include "Kernel/Layer.h"
#include "Kernel/NodeHelper.h"
#include "Kernel/NodeRenderHierarchy.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/EventableHelper.h"

#include "Config/Stringstream.h"
#include "Config/ArrayString.h"

#include "math/quat.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Movie2::Movie2()
        : m_composition( nullptr )
        , m_duration( 0.f )
        , m_frameDuration( 0.f )
        , m_hasBounds( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2::~Movie2()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::setResourceMovie2( const ResourcePtr & _resource )
    {
        ResourceMovie2Ptr resourceMovie2 = stdex::intrusive_static_cast<ResourceMovie2Ptr>(_resource);

        if( m_resourceMovie2 == resourceMovie2 )
        {
            return;
        }

        this->recompile( [this, resourceMovie2]()
        {
            m_resourceMovie2 = resourceMovie2;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & Movie2::getResourceMovie2() const
    {
        return m_resourceMovie2;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::setCompositionName( const ConstString & _compositionName )
    {
        if( m_compositionName == _compositionName )
        {
            return true;
        }

        m_compositionName = _compositionName;

        if( m_resourceMovie2 == nullptr )
        {
            return true;
        }

        this->destroyCompositionLayers_();

        if( this->createCompositionLayers_() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Movie2::getCompositionName() const
    {
        return m_compositionName;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::setTextAliasEnvironment( const ConstString & _aliasEnvironment )
    {
        if( m_aliasEnvironment == _aliasEnvironment )
        {
            return;
        }

        m_aliasEnvironment = _aliasEnvironment;

        for( const HashtableTexts::value_type & value : m_texts )
        {
            const TextFieldPtr & text = value.element;

            text->setTextAliasEnvironment( m_aliasEnvironment );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Movie2::getTextAliasEnvironment() const
    {
        return m_aliasEnvironment;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::interruptElements_()
    {
        for( const SurfacePtr & surface : m_surfaces )
        {
            AnimationInterface * animation = surface->getAnimation();

            if( animation == nullptr )
            {
                continue;
            }

            if( animation->isPlay() == false )
            {
                continue;
            }

            if( animation->isInterrupt() == true )
            {
                return false;
            }

            if( animation->interrupt() == false )
            {
                return false;
            }
        }

        for( const HashtableParticleEmitter2s::value_type & value : m_particleEmitters )
        {
            const NodePtr & particle = value.element;

            AnimationInterface * animation = particle->getAnimation();

            if( animation == nullptr )
            {
                continue;
            }

            if( animation->isPlay() == false )
            {
                continue;
            }

            if( animation->isInterrupt() == true )
            {
                return false;
            }

            if( animation->interrupt() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::checkInterruptElement_() const
    {
        for( const SurfacePtr & surface : m_surfaces )
        {
            AnimationInterface * animation = surface->getAnimation();

            if( animation != nullptr )
            {
                if( animation->isInterrupt() == true )
                {
                    return false;
                }
            }
        }

        for( const HashtableParticleEmitter2s::value_type & value : m_particleEmitters )
        {
            const NodePtr & particle = value.element;

            AnimationInterface * animation = particle->getAnimation();

            if( animation != nullptr )
            {
                if( animation->isInterrupt() == true )
                {
                    return false;
                }
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::getWorldBoundingBox( mt::box2f * _bb ) const
    {
        if( this->isCompile() == false )
        {
            return false;
        }

        bool successful = false;

        const mt::mat4f & wm = this->getWorldMatrix();

        mt::box2f bbwm;
        mt::insideout_box( bbwm );

        ae_uint32_t compute_movie_mesh_iterator = 0;

        aeMovieRenderMesh mesh;
        while( ae_compute_movie_mesh( m_composition, &compute_movie_mesh_iterator, &mesh ) == AE_TRUE )
        {
            if( mesh.track_matte_userdata == nullptr )
            {
                switch( mesh.layer_type )
                {
                case AE_MOVIE_LAYER_TYPE_SLOT:
                    {
                    }break;
                case AE_MOVIE_LAYER_TYPE_SOCKET:
                    {
                    }break;
                case AE_MOVIE_LAYER_TYPE_SPRITE:
                    {
                        ShapeQuadFixed * node = Helper::reinterpretNodeCast<ShapeQuadFixed *>( mesh.element_userdata );

                        const mt::box2f * bb = node->getBoundingBox();

                        if( bb != nullptr )
                        {
                            mt::merge_box( bbwm, *bb );

                            successful = true;
                        }
                    }break;
                case AE_MOVIE_LAYER_TYPE_TEXT:
                    {
                        TextField * node = Helper::reinterpretNodeCast<TextField *>( mesh.element_userdata );

                        const mt::box2f * bb = node->getBoundingBox();

                        if( bb != nullptr )
                        {
                            mt::merge_box( bbwm, *bb );

                            successful = true;
                        }
                    }break;
                case AE_MOVIE_LAYER_TYPE_PARTICLE:
                    {
                        //Node * node = reinterpret_node_cast<Node *>(mesh.element_userdata);

                        //const mt::box2f * bb = node->getBoundingBox();

                        //if( bb == nullptr )
                        //{
                        //    mt::merge_box( bb, *bb );
                        //}
                    }break;
                case AE_MOVIE_LAYER_TYPE_SHAPE:
                case AE_MOVIE_LAYER_TYPE_SOLID:
                case AE_MOVIE_LAYER_TYPE_SEQUENCE:
                case AE_MOVIE_LAYER_TYPE_IMAGE:
                    {
                        if( mesh.vertexCount == 0 )
                        {
                            continue;
                        }

                        for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            mt::vec3f p;
                            p.from_f3( mesh.position[index] );

                            mt::vec2f pwm;
                            mt::mul_v2_v3_m4( pwm, p, wm );

                            mt::add_internal_point( bbwm, pwm );
                        }

                        successful = true;
                    }break;
                default:
                    break;
                }
            }
            else
            {
                switch( mesh.layer_type )
                {
                case AE_MOVIE_LAYER_TYPE_SEQUENCE:
                case AE_MOVIE_LAYER_TYPE_IMAGE:
                    {
                        if( mesh.vertexCount == 0 )
                        {
                            continue;
                        }

                        for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            mt::vec3f p;
                            p.from_f3( mesh.position[index] );

                            mt::vec2f pwm;
                            mt::mul_v2_v3_m4( pwm, p, wm );

                            mt::add_internal_point( bbwm, pwm );
                        }

                        successful = true;
                    }break;
                default:
                    break;
                }
            }
        }

        *_bb = bbwm;

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::createCompositionLayers_()
    {
        const ResourceMovie2CompositionDesc * composition = m_resourceMovie2->getCompositionDesc( m_compositionName );

        if( composition == nullptr )
        {
            Stringstream ss;

            m_resourceMovie2->foreachCompositionDesc( [&ss]( const ConstString & _name, const ResourceMovie2CompositionDesc & _desc )
            {
                if( _desc.master == false )
                {
                    return;
                }

                ss << _name.c_str() << ", ";
            } );

            LOGGER_ERROR( "name '%s' resource '%s' path '%s' invalid get composition name '%s' in [%s]"
                , this->getName().c_str()
                , this->getResourceMovie2()->getName().c_str()
                , m_resourceMovie2->getFilePath().c_str()
                , m_compositionName.c_str()
                , ss.str().c_str()
            );

            return false;
        }

        m_duration = AE_TIME_MILLISECOND( composition->duration );
        m_frameDuration = AE_TIME_MILLISECOND( composition->frameDuration );
        m_hasBounds = composition->has_bounds;
        m_bounds = composition->bounds;

        for( const ResourceMovie2CompositionLayer & layer : composition->layers )
        {
            if( layer.type == STRINGIZE_STRING_LOCAL( "TextField" ) )
            {
                TextFieldPtr node = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "TextField" )
                        , MENGINE_DOCUMENT( "name '%s' composition '%s'", this->getName().c_str(), m_compositionName.c_str() )
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( node, false );

                node->setName( layer.name );
                node->setExternalRender( true );

                node->setTextID( layer.name );
                node->setTextAliasEnvironment( m_aliasEnvironment );

                node->setPixelsnap( false );

                this->addText_( layer.index, node );

                MatrixProxyPtr matrixProxy = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MatrixProxy" ), MENGINE_DOCUMENT_FUNCTION );

                MENGINE_ASSERTION_MEMORY_PANIC( matrixProxy, false );

                matrixProxy->setName( layer.name );

                matrixProxy->setProxyMatrix( layer.matrix );
                matrixProxy->setLocalColor( layer.color );

                matrixProxy->addChild( node );

                this->addChild( matrixProxy );

                this->addMatrixProxy_( matrixProxy );
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "Movie2Slot" ) )
            {
                Movie2SlotPtr node = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2Slot" ), MENGINE_DOCUMENT_FUNCTION );

                MENGINE_ASSERTION_MEMORY_PANIC( node, false );

                node->setName( layer.name );

                RenderInterface * render = node->getRender();
                render->setExternalRender( true );

                const ConstString & movieName = this->getName();
                node->setMovieName( movieName );

                this->addSlot_( layer.index, node );

                MatrixProxyPtr matrixProxy = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MatrixProxy" ), MENGINE_DOCUMENT_FUNCTION );

                MENGINE_ASSERTION_MEMORY_PANIC( matrixProxy, false );

                matrixProxy->setName( layer.name );

                matrixProxy->setProxyMatrix( layer.matrix );
                matrixProxy->setLocalColor( layer.color );

                matrixProxy->addChild( node );

                this->addChild( matrixProxy );

                this->addMatrixProxy_( matrixProxy );
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "HotSpotPolygon" ) )
            {
                HotSpotPolygonPtr node = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "HotSpotPolygon" )
                        , MENGINE_DOCUMENT( "name '%s' composition '%s'", this->getName().c_str(), m_compositionName.c_str() )
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( node, false );

                node->setName( layer.name );

                this->addSocket_( layer.index, node );

                MatrixProxyPtr matrixProxy = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MatrixProxy" )
                        , MENGINE_DOCUMENT( "name '%s' composition '%s'", this->getName().c_str(), m_compositionName.c_str() )
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( matrixProxy, false );

                matrixProxy->setName( layer.name );

                matrixProxy->setProxyMatrix( layer.matrix );
                matrixProxy->setLocalColor( layer.color );

                matrixProxy->addChild( node );

                this->addChild( matrixProxy );

                this->addMatrixProxy_( matrixProxy );
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ) || layer.type == STRINGIZE_STRING_LOCAL( "AstralaxEmitter" ) )
            {
                NodePtr node = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "AstralaxEmitter" )
                        , MENGINE_DOCUMENT( "name '%s' composition '%s'", this->getName().c_str(), m_compositionName.c_str() )
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( node, false );

                node->setName( layer.name );

                RenderInterface * render = node->getRender();
                render->setExternalRender( true );

                this->addParticle_( layer.index, node );

                MatrixProxyPtr matrixProxy = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MatrixProxy" )
                        , MENGINE_DOCUMENT( "name '%s' composition '%s'", this->getName().c_str(), m_compositionName.c_str() )
                    );

                if( matrixProxy == nullptr )
                {
                    return false;
                }

                matrixProxy->setName( layer.name );

                matrixProxy->setProxyMatrix( layer.matrix );
                matrixProxy->setLocalColor( layer.color );

                matrixProxy->addChild( node );

                this->addChild( matrixProxy );

                this->addMatrixProxy_( matrixProxy );
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ) )
            {
                ResourceImagePtr resourceImage = RESOURCE_SERVICE()
                    ->getResourceReference( layer.name );

                MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, false, "name '%s' resource '%s' composition '%s' layer '%s' invalid get resource for image '%s'"
                    , this->getName().c_str()
                    , this->getResourceMovie2()->getName().c_str()
                    , this->getCompositionName().c_str()
                    , layer.name.c_str()
                    , layer.name.c_str()
                );

                SurfaceImagePtr surface = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceImage" )
                        , MENGINE_DOCUMENT( "name '%s' composition '%s'", this->getName().c_str(), m_compositionName.c_str() )
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( surface, false );

                surface->setResourceImage( resourceImage );

                ShapeQuadFixedPtr node = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), layer.type, MENGINE_DOCUMENT_FUNCTION );

                MENGINE_ASSERTION_MEMORY_PANIC( node, false, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'Sprite'"
                    , this->getName().c_str()
                    , this->getResourceMovie2()->getName().c_str()
                    , this->getCompositionName().c_str()
                    , layer.name.c_str()
                );

                node->setName( layer.name );
                node->setExternalRender( true );

                node->setSurface( surface );

                this->addSprite_( layer.index, node );

                MatrixProxyPtr matrixProxy = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MatrixProxy" )
                        , MENGINE_DOCUMENT( "name '%s' composition '%s'", this->getName().c_str(), m_compositionName.c_str() )
                    );

                MENGINE_ASSERTION_MEMORY_PANIC( matrixProxy, false );

                matrixProxy->setName( layer.name );

                matrixProxy->setProxyMatrix( layer.matrix );
                matrixProxy->setLocalColor( layer.color );

                matrixProxy->addChild( node );

                this->addChild( matrixProxy );

                this->addMatrixProxy_( matrixProxy );
            }
            else
            {
                LOGGER_ERROR( "name '%s' resource '%s' composition '%s' layer '%s' invalid create type '%s'"
                    , this->getName().c_str()
                    , this->getResourceMovie2()->getName().c_str()
                    , this->getCompositionName().c_str()
                    , layer.name.c_str()
                    , layer.type.c_str()
                );

                return false;
            }
        }

        for( const ResourceMovie2CompositionSubComposition & subcomposition : composition->subcompositions )
        {
            Movie2SubCompositionPtr node = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2SubComposition" ), MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( node, false );

            node->setMovie( this );

            node->setName( subcomposition.name );
            node->setType( STRINGIZE_STRING_LOCAL( "Movie2SubComposition" ) );
            node->setDuration( subcomposition.duration );
            node->setFrameDuration( subcomposition.frameDuration );

            this->addSubMovieComposition_( subcomposition.name, node );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::destroyCompositionLayers_()
    {
        for( const MatrixProxyPtr & proxy : m_matrixProxies )
        {
            proxy->removeFromParent();
            proxy->removeChildren( []( const NodePtr & )
            {} );
        }

        m_matrixProxies.clear();

        m_slots.clear();

        for( const HashtableSockets::value_type & value : m_sockets )
        {
            const HotSpotPolygonPtr & hotspot = value.element;
            EventationInterface * eventation = hotspot->getEventation();
            eventation->removeEvents();
            hotspot->removeFromParent();
        }

        m_sockets.clear();

        m_texts.clear();
        m_particleEmitters.clear();
        m_sprites.clear();
        m_subCompositions.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::setWorkAreaFromEvent( const ConstString & _eventName )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_composition, false, "name '%s' invalid setup '%s' not compile"
            , this->getName().c_str()
            , _eventName.c_str()
        );

        float a, b;
        ae_bool_t ok = ae_get_movie_composition_node_in_out_time( m_composition, _eventName.c_str(), AE_MOVIE_LAYER_TYPE_EVENT, &a, &b );

        if( ok == AE_FALSE )
        {
            return false;
        }

        ae_set_movie_composition_work_area( m_composition, a, b );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::removeWorkArea()
    {
        if( m_composition == nullptr )
        {
            LOGGER_ERROR( "name '%s' not compile"
                , this->getName().c_str()
            );

            return false;
        }

        ae_remove_movie_composition_work_area( m_composition );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasCompositionBounds() const
    {
        return m_hasBounds;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::box2f & Movie2::getCompositionBounds() const
    {
        return m_bounds;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasSubComposition( const ConstString & _name ) const
    {
        bool result = m_subCompositions.exist( _name );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    const Movie2SubCompositionPtr & Movie2::getSubComposition( const ConstString & _name ) const
    {
        const Movie2SubCompositionPtr & subComposition = m_subCompositions.find( _name );

        return subComposition;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasMovieLayers( const ConstString & _name ) const
    {
        if( m_composition == nullptr )
        {
            LOGGER_ERROR( "movie2 '%s' not compile (layer '%s')"
                , this->getName().c_str()
                , _name.c_str()
            );

            return false;
        }

        if( ae_has_movie_composition_node_any( m_composition, _name.c_str() ) == AE_FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::setEnableMovieLayers( const ConstString & _name, bool _enable )
    {
        if( m_composition == nullptr )
        {
            LOGGER_ERROR( "name '%s' invalid get layer '%s' not compile"
                , this->getName().c_str()
                , _name.c_str()
            );

            return;
        }

#ifdef MENGINE_DEBUG
        if( ae_has_movie_composition_node_any( m_composition, _name.c_str() ) == AE_FALSE )
        {
            LOGGER_ERROR( "name '%s' layer '%s' not found"
                , this->getName().c_str()
                , _name.c_str()
            );

            return;
        }
#endif

        ae_set_movie_composition_nodes_enable_any( m_composition, _name.c_str(), _enable ? AE_TRUE : AE_FALSE );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::foreachRenderSlots( const RenderContext * _context, const LambdaMovieRenderSlot & _lambda )
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        ae_userdata_t composition_camera_userdata = ae_get_movie_composition_camera_userdata( m_composition );
        AE_UNUSED( composition_camera_userdata );

        ae_uint32_t compute_movie_mesh_iterator = 0;

        aeMovieRenderMesh mesh;
        while( ae_compute_movie_mesh( m_composition, &compute_movie_mesh_iterator, &mesh ) == AE_TRUE )
        {
            RenderContext context;

            if( mesh.camera_userdata != nullptr )
            {
                Movie2::Camera * camera = reinterpret_cast<Movie2::Camera *>(mesh.camera_userdata);

                context.camera = camera->projection;
                context.viewport = camera->viewport;
                context.transformation = _context->transformation;
                context.scissor = _context->scissor;
                context.target = _context->target;
            }
            else
            {
                context = *_context;
            }

            if( mesh.viewport != nullptr )
            {
                Movie2ScissorPtr scissor = Helper::makeFactorableUnique<Movie2Scissor>();

                scissor->setViewport( wm, mesh.viewport );

                context.scissor = scissor;
            }
            else
            {
                context.scissor = _context->scissor;
            }

            if( mesh.track_matte_userdata == nullptr )
            {
                switch( mesh.layer_type )
                {
                case AE_MOVIE_LAYER_TYPE_SLOT:
                    {
                        Movie2Slot * node = Helper::reinterpretNodeCast<Movie2Slot *>( mesh.element_userdata );

                        _lambda( node, &context );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SOCKET:
                    {
                        HotSpotPolygon * node = Helper::reinterpretNodeCast<HotSpotPolygon *>( mesh.element_userdata );

                        _lambda( node, &context );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SPRITE:
                    {
                        ShapeQuadFixed * node = Helper::reinterpretNodeCast<ShapeQuadFixed *>( mesh.element_userdata );

                        _lambda( node, &context );
                    }break;
                case AE_MOVIE_LAYER_TYPE_TEXT:
                    {
                        TextField * node = Helper::reinterpretNodeCast<TextField *>( mesh.element_userdata );

                        _lambda( node, &context );
                    }break;
                case AE_MOVIE_LAYER_TYPE_PARTICLE:
                    {
                        Node * node = Helper::reinterpretNodeCast<Node *>( mesh.element_userdata );

                        _lambda( node, &context );
                    }break;
                default:
                    break;
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::_play( uint32_t _playId, float _time )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "name '%s' is not compile"
                , this->getName().c_str()
            );

            return false;
        }

        float timing = this->getTime();

        ae_play_movie_composition( m_composition, timing * 0.001f );

        EVENTABLE_METHOD( EVENT_ANIMATION_PLAY )
            ->onAnimationPlay( _playId, _time );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::_restart( uint32_t _playId, float _time )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_RESTART )
            ->onAnimationRestart( _playId, _time );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_pause( uint32_t _playId )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "name '%s' is not compile"
                , this->getName().c_str()
            );

            return;
        }

        ae_pause_movie_composition( m_composition );

        EVENTABLE_METHOD( EVENT_ANIMATION_PAUSE )
            ->onAnimationPause( _playId );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_resume( uint32_t _playId, float _time )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "name '%s' is not compile"
                , this->getName().c_str()
            );

            return;
        }

        ae_resume_movie_composition( m_composition );

        EVENTABLE_METHOD( EVENT_ANIMATION_RESUME )
            ->onAnimationResume( _playId, _time );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::_stop( uint32_t _playId )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "name '%s' is not compile"
                , this->getName().c_str()
            );

            return false;
        }

        ae_stop_movie_composition( m_composition );

        EVENTABLE_METHOD( EVENT_ANIMATION_STOP )
            ->onAnimationStop( _playId );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_end( uint32_t _playId )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_END )
            ->onAnimationEnd( _playId );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::_interrupt( uint32_t _playId )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "name '%s' is not compile"
                , this->getName().c_str()
            );

            return false;
        }

        ae_interrupt_movie_composition( m_composition, AE_FALSE );

        EVENTABLE_METHOD( EVENT_ANIMATION_INTERRUPT )
            ->onAnimationInterrupt( _playId );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_bool_t __movie_composition_camera_provider( const aeMovieCameraProviderCallbackData * _callbackData, ae_voidptrptr_t _cd, ae_voidptr_t _ud )
    {
        Movie2 * movie2 = Helper::reinterpretNodeCast<Movie2 *>( _ud );

        ConstString c_name = Helper::stringizeString( _callbackData->name );

        Movie2::Camera * old_camera;
        if( movie2->getCamera_( c_name, &old_camera ) == true )
        {
            *_cd = old_camera;

            return AE_TRUE;
        }

        RenderCameraProjectionPtr renderCameraProjection = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "RenderCameraProjection" ), MENGINE_DOCUMENT_FUNCTION );

        renderCameraProjection->setName( c_name );

        mt::vec3f cameraTarget;
        cameraTarget.from_f3( _callbackData->target );
        mt::vec3f cameraPosition;
        cameraPosition.from_f3( _callbackData->position );

        mt::vec3f cameraDirection;
        mt::norm_v3_v3( cameraDirection, cameraTarget - cameraPosition );

        float aspect = _callbackData->width / _callbackData->height;

        renderCameraProjection->setCameraPosition( cameraPosition );
        renderCameraProjection->setCameraDirection( cameraDirection );
        renderCameraProjection->setCameraFOV( _callbackData->fov );
        renderCameraProjection->setCameraAspect( aspect );

        RenderViewportPtr renderViewport = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "RenderViewport" ), MENGINE_DOCUMENT_FUNCTION );

        renderViewport->setName( c_name );

        Viewport vp;
        vp.begin.x = 0.f;
        vp.begin.y = 0.f;

        vp.end.x = _callbackData->width;
        vp.end.y = _callbackData->height;

        renderViewport->setViewport( vp );

        Movie2::Camera * new_camera = movie2->addCamera_( c_name, renderCameraProjection, renderViewport );

        *_cd = new_camera;

        return AE_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    static void __movie_composition_camera_deleter( const aeMovieCameraDeleterCallbackData * _callbackData, ae_voidptr_t _data )
    {
        Movie2 * movie2 = Helper::reinterpretNodeCast<Movie2 *>( _data );

        ConstString c_name = Helper::stringizeString( _callbackData->name );

        movie2->removeCamera_( c_name );
    }
    //////////////////////////////////////////////////////////////////////////
    static void __movie_composition_camera_update( const aeMovieCameraUpdateCallbackData * _callbackData, ae_voidptr_t _data )
    {
        AE_UNUSED( _data );

        Movie2::Camera * camera = reinterpret_cast<Movie2::Camera *>(_callbackData->camera_userdata);

        //camera
        mt::vec3f cameraTarget;
        cameraTarget.from_f3( _callbackData->target );
        mt::vec3f cameraPosition;
        cameraPosition.from_f3( _callbackData->position );

        mt::vec3f cameraDirection;
        mt::norm_v3_v3( cameraDirection, cameraTarget - cameraPosition );

        camera->projection->setCameraPosition( cameraPosition );
        camera->projection->setCameraDirection( cameraDirection );
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static EMaterialBlendMode getMovieBlendMode( ae_blend_mode_t _ae_blend_mode )
        {
            EMaterialBlendMode blend_mode = EMB_NORMAL;

            switch( _ae_blend_mode )
            {
            case AE_MOVIE_BLEND_ADD:
                blend_mode = EMB_ADD;
                break;
            case AE_MOVIE_BLEND_SCREEN:
                blend_mode = EMB_SCREEN;
                break;
            case AE_MOVIE_BLEND_MULTIPLY:
                blend_mode = EMB_MULTIPLY;
                break;
            default:
                break;
            };

            return blend_mode;
        }
        //////////////////////////////////////////////////////////////////////////
        static EMaterialBlendMode getMovieLayerBlendMode( const aeMovieLayerData * _layer )
        {
            ae_blend_mode_t layer_blend_mode = ae_get_movie_layer_data_blend_mode( _layer );

            EMaterialBlendMode blend_mode = getMovieBlendMode( layer_blend_mode );

            return blend_mode;
        }
        //////////////////////////////////////////////////////////////////////////
        static void updateMatrixProxy( const Node * _node, ae_bool_t _immutable_matrix, const float * _matrix, ae_bool_t _immutable_color, const ae_color_t & _color, ae_color_channel_t _opacity )
        {
            Node * nodeParent = _node->getParent();

            MatrixProxy * matrixProxy = Helper::staticNodeCast<MatrixProxy *>( nodeParent );

            if( _immutable_matrix == AE_FALSE )
            {
                mt::mat4f mp;
                mp.from_f12( _matrix );
                matrixProxy->setProxyMatrix( mp );
            }

            if( _immutable_color == AE_FALSE )
            {
                matrixProxy->setLocalColorRGBA( _color.r, _color.g, _color.b, _opacity );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ae_bool_t Movie2::__movie_composition_node_provider( const aeMovieNodeProviderCallbackData * _callbackData, ae_voidptrptr_t _nd, ae_voidptr_t _ud )
    {
        Movie2 * movie2 = static_cast<Movie2 *>( _ud );

        const aeMovieLayerData * layer = _callbackData->layer;

        ae_uint32_t node_index = _callbackData->index;
        const ae_char_t * layer_name = ae_get_movie_layer_data_name( layer );

        ae_bool_t is_track_matte = ae_is_movie_layer_data_track_mate( layer );

        if( is_track_matte == AE_TRUE )
        {
            *_nd = AE_NULLPTR;

            return AE_TRUE;
        }

        aeMovieLayerTypeEnum type = ae_get_movie_layer_data_type( layer );

        switch( type )
        {
        case AE_MOVIE_LAYER_TYPE_SPRITE:
            {
                const ShapeQuadFixedPtr & node = movie2->getSprite_( node_index );

                MENGINE_ASSERTION_MEMORY_PANIC( node, AE_FALSE, "name '%s' resource '%s' composition '%s' layer '%s' invalid create 'Sprite'"
                    , movie2->getName().c_str()
                    , movie2->getResourceMovie2()->getName().c_str()
                    , movie2->getCompositionName().c_str()
                    , layer_name
                );

                const SurfacePtr & surface = node->getSurface();

                EMaterialBlendMode blend_mode = Detail::getMovieLayerBlendMode( layer );

                surface->setBlendMode( blend_mode );

                *_nd = node.get();

                return AE_TRUE;
            }break;
        case AE_MOVIE_LAYER_TYPE_TEXT:
            {
                const TextFieldPtr & node = movie2->getText_( node_index );

                MENGINE_ASSERTION_MEMORY_PANIC( node, AE_FALSE );

                Detail::updateMatrixProxy( node.get(), AE_FALSE, _callbackData->matrix, AE_FALSE, _callbackData->color, _callbackData->opacity );

                if( ae_has_movie_layer_data_option( layer, AE_OPTION( '\0', '\0', 'h', 'r' ) ) == AE_TRUE )
                {
                    node->setHorizontalRightAlign();
                }

                if( ae_has_movie_layer_data_option( layer, AE_OPTION( '\0', '\0', 'h', 'c' ) ) == AE_TRUE )
                {
                    node->setHorizontalCenterAlign();
                }

                if( ae_has_movie_layer_data_option( layer, AE_OPTION( '\0', '\0', 'h', 'l' ) ) == AE_TRUE )
                {
                    node->setHorizontalLeftAlign();
                }

                if( ae_has_movie_layer_data_option( layer, AE_OPTION( '\0', '\0', 'v', 't' ) ) == AE_TRUE )
                {
                    node->setVerticalTopAlign();
                }

                if( ae_has_movie_layer_data_option( layer, AE_OPTION( '\0', '\0', 'v', 'c' ) ) == AE_TRUE )
                {
                    node->setVerticalCenterAlign();
                }

                if( ae_has_movie_layer_data_option( layer, AE_OPTION( '\0', '\0', 'v', 'b' ) ) == AE_TRUE )
                {
                    node->setVerticalBottomAlign();
                }

                if( ae_has_movie_layer_data_option( layer, AE_OPTION( '\0', '\0', 'a', 's' ) ) == AE_TRUE )
                {
                    node->setWrap( false );
                    node->setAutoScale( true );
                }

                ae_aabb_t aabb;
                if( ae_get_movie_layer_data_dimension( layer, &aabb ) == AE_TRUE )
                {
                    float maxLength = aabb.maximal_x - aabb.minimal_x;

                    node->setMaxLength( maxLength );
                }

                *_nd = node.get();

                return AE_TRUE;
            }break;
        case AE_MOVIE_LAYER_TYPE_SLOT:
            {
                const Movie2SlotPtr & node = movie2->getSlot_( node_index );

                MENGINE_ASSERTION_MEMORY_PANIC( node, AE_FALSE );

                *_nd = node.get();

                return AE_TRUE;
            }break;
        case AE_MOVIE_LAYER_TYPE_SOCKET:
            {
                const HotSpotPolygonPtr & node = movie2->getSocket_( node_index );

                MENGINE_ASSERTION_MEMORY_PANIC( node, AE_FALSE );

                const ae_polygon_t * polygon;
                if( ae_get_movie_layer_data_socket_polygon( _callbackData->layer, 0, &polygon ) == AE_FALSE )
                {
                    return false;
                }

                Polygon p;
                for( ae_uint32_t index = 0; index != polygon->point_count; ++index )
                {
                    const ae_vector2_t * v = polygon->points + index;

                    mt::vec2f v2( (*v)[0], (*v)[1] );

                    p.append( v2 );
                }

                node->setPolygon( p );

                *_nd = node.get();

                return AE_TRUE;
            }break;
        case AE_MOVIE_LAYER_TYPE_PARTICLE:
            {
                const NodePtr & node = movie2->getParticle_( node_index );

                MENGINE_ASSERTION_MEMORY_PANIC( node, AE_FALSE );

                UnknownParticleEmitterInterface * unknownParticleEmitter2 = node->getUnknown();

                if( ae_has_movie_layer_data_option( layer, AE_OPTION( '\0', '\0', '\0', 't' ) ) == AE_TRUE )
                {
                    unknownParticleEmitter2->setEmitterPositionRelative( true );
                    unknownParticleEmitter2->setEmitterCameraRelative( false );
                    unknownParticleEmitter2->setEmitterTranslateWithParticle( false );
                }
                else
                {
                    unknownParticleEmitter2->setEmitterPositionRelative( false );
                    unknownParticleEmitter2->setEmitterCameraRelative( false );
                    //unknownParticleEmitter2->setEmitterTranslateWithParticle( true );
                }

                Resource * resourceParticle = Helper::reinterpretNodeCast<Resource *>( ae_get_movie_layer_data_resource_userdata( _callbackData->layer ) );

                unknownParticleEmitter2->setResourceParticle( ResourcePtr( resourceParticle ) );

                unknownParticleEmitter2->setEmitterPositionProviderOriginOffset( -mt::vec3f( 1024.f, 1024.f, 0.f ) );

                //EMaterialBlendMode blend_mode = getMovieBlendMode( layer );

                AnimationInterface * animation = node->getAnimation();

                ae_float_t layer_stretch = ae_get_movie_layer_data_stretch( _callbackData->layer );
                animation->setStretch( layer_stretch );
                animation->setLoop( _callbackData->incessantly );

                *_nd = node.get();

                return AE_TRUE;
            }break;
        default:
            break;
        };

        if( _callbackData->track_matte_layer != AE_NULLPTR )
        {
            switch( type )
            {
            case AE_MOVIE_LAYER_TYPE_IMAGE:
                {
                    SurfaceTrackMattePtr surfaceTrackMatte = PROTOTYPE_SERVICE()
                        ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceTrackMatte" ), MENGINE_DOCUMENT_FUNCTION );

                    MENGINE_ASSERTION_MEMORY_PANIC( surfaceTrackMatte, AE_FALSE );

                    ConstString c_name = Helper::stringizeString( layer_name );
                    surfaceTrackMatte->setName( c_name );

                    Movie2DataImageDesc * imageDesc = reinterpret_cast<Movie2DataImageDesc *>(ae_get_movie_layer_data_resource_userdata( _callbackData->layer ));
                    const ResourceImagePtr & resourceImage = imageDesc->resourceImage;

                    surfaceTrackMatte->setResourceImage( resourceImage );

                    Movie2DataImageDesc * imageTrackMatteDesc = reinterpret_cast<Movie2DataImageDesc *>(ae_get_movie_layer_data_resource_userdata( _callbackData->track_matte_layer ));
                    const ResourceImagePtr & resourceTrackMatteImage = imageTrackMatteDesc->resourceImage;

                    surfaceTrackMatte->setResourceTrackMatteImage( resourceTrackMatteImage );

                    ae_track_matte_mode_t track_matte_mode = ae_get_movie_layer_data_track_matte_mode( _callbackData->layer );

                    switch( track_matte_mode )
                    {
                    case AE_MOVIE_TRACK_MATTE_ALPHA:
                        {
                            surfaceTrackMatte->setTrackMatteMode( ESTM_MODE_ALPHA );
                        }break;
                    case AE_MOVIE_TRACK_MATTE_ALPHA_INVERTED:
                        {
                            surfaceTrackMatte->setTrackMatteMode( ESTM_MODE_ALPHA_INVERTED );
                        }break;
                    default:
                        break;
                    }

                    EMaterialBlendMode blend_mode = Detail::getMovieLayerBlendMode( layer );

                    surfaceTrackMatte->setBlendMode( blend_mode );

                    movie2->addSurface_( surfaceTrackMatte, true );

                    *_nd = surfaceTrackMatte.get();

                    return AE_TRUE;
                }break;
            case AE_MOVIE_LAYER_TYPE_SEQUENCE:
                {
                    SurfaceTrackMattePtr surfaceTrackMatte = PROTOTYPE_SERVICE()
                        ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceTrackMatte" ), MENGINE_DOCUMENT_FUNCTION );

                    MENGINE_ASSERTION_MEMORY_PANIC( surfaceTrackMatte, AE_FALSE );

                    ConstString c_name = Helper::stringizeString( layer_name );
                    surfaceTrackMatte->setName( c_name );

                    //Movie2DataImageDesc * imageDesc = reinterpret_cast<Movie2DataImageDesc *>(ae_get_movie_layer_data_resource_userdata( _callbackData->layer ));
                    //const ResourceImagePtr & resourceImage = imageDesc->resourceImage;

                    //surfaceTrackMatte->setResourceImage( resourceImage );

                    Movie2DataImageDesc * imageTrackMatteDesc = reinterpret_cast<Movie2DataImageDesc *>(ae_get_movie_layer_data_resource_userdata( _callbackData->track_matte_layer ));
                    const ResourceImagePtr & resourceTrackMatteImage = imageTrackMatteDesc->resourceImage;

                    surfaceTrackMatte->setResourceTrackMatteImage( resourceTrackMatteImage );

                    ae_track_matte_mode_t track_matte_mode = ae_get_movie_layer_data_track_matte_mode( _callbackData->layer );

                    switch( track_matte_mode )
                    {
                    case AE_MOVIE_TRACK_MATTE_ALPHA:
                        {
                            surfaceTrackMatte->setTrackMatteMode( ESTM_MODE_ALPHA );
                        }break;
                    case AE_MOVIE_TRACK_MATTE_ALPHA_INVERTED:
                        {
                            surfaceTrackMatte->setTrackMatteMode( ESTM_MODE_ALPHA_INVERTED );
                        }break;
                    default:
                        break;
                    }

                    EMaterialBlendMode blend_mode = Detail::getMovieLayerBlendMode( layer );

                    surfaceTrackMatte->setBlendMode( blend_mode );

                    movie2->addSurface_( surfaceTrackMatte, false );

                    *_nd = surfaceTrackMatte.get();

                    return AE_TRUE;
                }break;
            default:
                {
                }break;
            }
        }
        else
        {
            switch( type )
            {
            case AE_MOVIE_LAYER_TYPE_VIDEO:
                {
                    SurfacePtr surface = PROTOTYPE_SERVICE()
                        ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceVideo" ), MENGINE_DOCUMENT_FUNCTION );

                    MENGINE_ASSERTION_MEMORY_PANIC( surface, AE_FALSE );

                    ConstString c_name = Helper::stringizeString( layer_name );
                    surface->setName( c_name );

                    Resource * resourceVideo = Helper::reinterpretNodeCast<Resource *>( ae_get_movie_layer_data_resource_userdata( _callbackData->layer ) );

                    UnknownVideoSurfaceInterface * unknownVideoSurface = surface->getUnknown();

                    unknownVideoSurface->setResourceVideo( ResourcePtr( resourceVideo ) );

                    EMaterialBlendMode blend_mode = Detail::getMovieLayerBlendMode( layer );

                    AnimationInterface * surface_animation = surface->getAnimation();

                    surface_animation->setLoop( _callbackData->incessantly );

                    surface->setBlendMode( blend_mode );
                    surface->setPremultiplyAlpha( true );

                    movie2->addSurface_( surface, true );

                    *_nd = surface.get();

                    return AE_TRUE;
                }break;
            case AE_MOVIE_LAYER_TYPE_SOUND:
                {
                    SurfaceSoundPtr surfaceSound = PROTOTYPE_SERVICE()
                        ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceSound" ), MENGINE_DOCUMENT_FUNCTION );

                    MENGINE_ASSERTION_MEMORY_PANIC( surfaceSound, AE_FALSE );

                    ConstString c_name = Helper::stringizeString( layer_name );
                    surfaceSound->setName( c_name );

                    surfaceSound->setLoop( _callbackData->incessantly );
                    surfaceSound->setInterpolateVolume( false );

                    if( ae_has_movie_layer_data_option( layer, AE_OPTION( '\0', '\0', 'v', 'o' ) ) == AE_TRUE )
                    {
                        surfaceSound->setSoundCategory( ES_SOURCE_CATEGORY_VOICE );
                    }

                    if( ae_has_movie_layer_data_option( layer, AE_OPTION( '\0', '\0', 'm', 'u' ) ) == AE_TRUE )
                    {
                        surfaceSound->setSoundCategory( ES_SOURCE_CATEGORY_MUSIC );
                    }

                    ResourceSound * resourceSound = Helper::reinterpretNodeCast<ResourceSound *>( ae_get_movie_layer_data_resource_userdata( _callbackData->layer ) );

                    surfaceSound->setResourceSound( Helper::makeIntrusivePtr( resourceSound ) );

                    movie2->addSurface_( surfaceSound, true );

                    *_nd = surfaceSound.get();

                    return AE_TRUE;
                }break;
            default:
                break;
            }
        }

        *_nd = AE_NULLPTR;

        return AE_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_composition_node_deleter( const aeMovieNodeDeleterCallbackData * _callbackData, ae_voidptr_t _ud )
    {
        AE_UNUSED( _ud );

        ae_bool_t is_track_matte = ae_is_movie_layer_data_track_mate( _callbackData->layer );

        if( is_track_matte == AE_TRUE )
        {
            return;
        }

        aeMovieLayerTypeEnum type = ae_get_movie_layer_data_type( _callbackData->layer );

        if( _callbackData->track_matte_layer != AE_NULLPTR )
        {
            switch( type )
            {
            case AE_MOVIE_LAYER_TYPE_IMAGE:
                {
                    //SurfaceTrackMatte * surfaceTrackMatte = (SurfaceTrackMatte *)_callbackData->element;

                    //movie2->removeSurface( surfaceTrackMatte );
                }break;
            default:
                {
                }break;
            }
        }
        else
        {
            switch( type )
            {
            case AE_MOVIE_LAYER_TYPE_PARTICLE:
                {
                    //ParticleEmitter2 * particleEmitter = (ParticleEmitter2 *)_callbackData->element;

                    //movie2->removeParticle( particleEmitter );
                }break;
            case AE_MOVIE_LAYER_TYPE_VIDEO:
                {
                    //SurfaceVideo * surfaceVideo = (SurfaceVideo *)_callbackData->element;

                    //movie2->removeSurface( surfaceVideo );
                }break;
            case AE_MOVIE_LAYER_TYPE_SOUND:
                {
                    //SurfaceSound * surfaceSound = (SurfaceSound *)_callbackData->element;

                    //movie2->removeSurface( surfaceSound );
                }break;
            default:
                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    static void __movie_composition_node_update_animation( const aeMovieNodeUpdateCallbackData * _callbackData, AnimationInterface * _animation )
    {
        switch( _callbackData->state )
        {
        case AE_MOVIE_STATE_UPDATE_BEGIN:
            {
                float time = TIMELINE_SERVICE()
                    ->getTime();

                if( _callbackData->loop == AE_TRUE && _animation->isLoop() == true )
                {
                    if( _animation->isPlay() == false )
                    {
                        if( _animation->play( time ) == 0 )
                        {
                            return;
                        }
                    }
                }
                else
                {
                    _animation->setTime( AE_TIME_MILLISECOND( _callbackData->offset ) );

                    if( _animation->play( time ) == 0 )
                    {
                        return;
                    }

                    if( _callbackData->interrupt == AE_TRUE )
                    {
                        if( _animation->interrupt() == false )
                        {
                            return;
                        }
                    }
                }
            }break;
        case AE_MOVIE_STATE_UPDATE_PROCESS:
            {
            }break;
        case AE_MOVIE_STATE_UPDATE_STOP:
            {
                _animation->stop();
            }break;
        case AE_MOVIE_STATE_UPDATE_END:
            {
                if( _callbackData->loop == AE_FALSE && _callbackData->interrupt == AE_FALSE )
                {
                    _animation->stop();
                }
            }break;
        case AE_MOVIE_STATE_UPDATE_PAUSE:
            {
                _animation->pause();
            }break;
        case AE_MOVIE_STATE_UPDATE_RESUME:
            {
                float time = TIMELINE_SERVICE()
                    ->getTime();

                _animation->resume( time );
            }break;
        case AE_MOVIE_STATE_UPDATE_SEEK:
            {
                _animation->setTime( AE_TIME_MILLISECOND( _callbackData->offset ) );
            }break;
        default:
            {
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_composition_node_update( const aeMovieNodeUpdateCallbackData * _callbackData, ae_voidptr_t _ud )
    {
        MENGINE_UNUSED( _ud );

        aeMovieLayerTypeEnum layer_type = ae_get_movie_layer_data_type( _callbackData->layer );

        switch( layer_type )
        {
        case AE_MOVIE_LAYER_TYPE_PARTICLE:
            {
                Node * particle = Helper::reinterpretNodeCast<Node *>( _callbackData->element_userdata );

                Detail::updateMatrixProxy( particle, _callbackData->immutable_matrix, _callbackData->matrix, _callbackData->immutable_color, _callbackData->color, _callbackData->opacity );

                AnimationInterface * particle_animation = particle->getAnimation();

                __movie_composition_node_update_animation( _callbackData, particle_animation );
            }break;
        case AE_MOVIE_LAYER_TYPE_VIDEO:
            {
                Surface * surface = Helper::reinterpretNodeCast<Surface *>( _callbackData->element_userdata );

                AnimationInterface * surface_animation = surface->getAnimation();

                __movie_composition_node_update_animation( _callbackData, surface_animation );
            }break;
        case AE_MOVIE_LAYER_TYPE_SOUND:
            {
                SurfaceSound * surface = Helper::reinterpretNodeCast<SurfaceSound *>( _callbackData->element_userdata );

                surface->setVolume( _callbackData->volume );

                AnimationInterface * surface_animation = surface->getAnimation();

                __movie_composition_node_update_animation( _callbackData, surface_animation );
            }break;
        case AE_MOVIE_LAYER_TYPE_SLOT:
            {
                Movie2Slot * node = Helper::reinterpretNodeCast<Movie2Slot *>( _callbackData->element_userdata );

                Detail::updateMatrixProxy( node, _callbackData->immutable_matrix, _callbackData->matrix, _callbackData->immutable_color, _callbackData->color, _callbackData->opacity );
            }break;
        case AE_MOVIE_LAYER_TYPE_SPRITE:
            {
                ShapeQuadFixed * node = Helper::reinterpretNodeCast<ShapeQuadFixed *>( _callbackData->element_userdata );

                Detail::updateMatrixProxy( node, _callbackData->immutable_matrix, _callbackData->matrix, _callbackData->immutable_color, _callbackData->color, _callbackData->opacity );
            }break;
        case AE_MOVIE_LAYER_TYPE_TEXT:
            {
                TextField * node = Helper::reinterpretNodeCast<TextField *>( _callbackData->element_userdata );

                Detail::updateMatrixProxy( node, _callbackData->immutable_matrix, _callbackData->matrix, _callbackData->immutable_color, _callbackData->color, _callbackData->opacity );
            }break;
        case AE_MOVIE_LAYER_TYPE_SOCKET:
            {
                HotSpotPolygon * node = Helper::reinterpretNodeCast<HotSpotPolygon *>( _callbackData->element_userdata );

                Detail::updateMatrixProxy( node, _callbackData->immutable_matrix, _callbackData->matrix, _callbackData->immutable_color, _callbackData->color, _callbackData->opacity );
            }break;
        default:
            {
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    struct TrackMatteDesc
    {
        mt::mat4f matrix;
        aeMovieRenderMesh mesh;
        ae_track_matte_mode_t mode;
    };
    //////////////////////////////////////////////////////////////////////////
    static ae_bool_t __movie_composition_track_matte_provider( const aeMovieTrackMatteProviderCallbackData * _callbackData, ae_voidptrptr_t _tmp, ae_voidptr_t _ud )
    {
        AE_UNUSED( _ud );

        TrackMatteDesc * desc = Helper::allocateT<TrackMatteDesc>();

        desc->matrix.from_f12( _callbackData->matrix );
        desc->mesh = *_callbackData->mesh;
        desc->mode = _callbackData->track_matte_mode;

        *_tmp = desc;

        return AE_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_composition_track_matte_update( const aeMovieTrackMatteUpdateCallbackData * _callbackData, ae_voidptr_t _ud )
    {
        AE_UNUSED( _ud );

        switch( _callbackData->state )
        {
        case AE_MOVIE_STATE_UPDATE_BEGIN:
            {
                TrackMatteDesc * desc = reinterpret_cast<TrackMatteDesc *>(_callbackData->track_matte_userdata);

                desc->matrix.from_f12( _callbackData->matrix );
                desc->mesh = *_callbackData->mesh;
            }break;
        case AE_MOVIE_STATE_UPDATE_PROCESS:
            {
                TrackMatteDesc * desc = reinterpret_cast<TrackMatteDesc *>(_callbackData->track_matte_userdata);

                desc->matrix.from_f12( _callbackData->matrix );
                desc->mesh = *_callbackData->mesh;
            }break;
        default:
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_composition_track_matte_deleter( const aeMovieTrackMatteDeleterCallbackData * _callbackData, ae_voidptr_t _ud )
    {
        AE_UNUSED( _ud );

        TrackMatteDesc * desc = reinterpret_cast<TrackMatteDesc *>(_callbackData->element_userdata);

        Helper::freeT( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    struct ShaderParameterDesc
    {
        Char uniform[32];
        ae_uint8_t type;
    };
    //////////////////////////////////////////////////////////////////////////
    struct Movie2ShaderDesc
    {
        ConstString materialNameBlend;
        ConstString materialNameBlendExternalAlpha;
        RenderProgramVariableInterfacePtr programVariable;
        ae_uint32_t indexOffset;
    };
    //////////////////////////////////////////////////////////////////////////
    static ae_bool_t __movie_composition_shader_provider( const aeMovieShaderProviderCallbackData * _callbackData, ae_voidptrptr_t _sd, ae_voidptr_t _ud )
    {
        AE_UNUSED( _ud );

        Movie2ShaderDesc * desc = Helper::allocateT<Movie2ShaderDesc>();

        ArrayString<64> materialNameBlend;
        materialNameBlend.append( _callbackData->description );
        materialNameBlend.append( "_Blend" );

        desc->materialNameBlend = Helper::stringizeString( materialNameBlend.c_str() );

        ArrayString<64> materialNameBlendExternalAlpha;
        materialNameBlendExternalAlpha.append( _callbackData->description );
        materialNameBlendExternalAlpha.append( "_Blend_ExternalAlpha" );

        desc->materialNameBlendExternalAlpha = Helper::stringizeString( materialNameBlendExternalAlpha.c_str() );

        desc->indexOffset = 1;

        RenderProgramVariableInterfacePtr programVariable = RENDER_SYSTEM()
            ->createProgramVariable( 0, desc->indexOffset + _callbackData->parameter_count, MENGINE_DOCUMENT_FUNCTION );

        float shader_uvsl[4] = { 0.f, 0.f, 1.f, 1.f };
        programVariable->setPixelVariableFloats( "uvsl", 0, shader_uvsl, 4, 1 );

        for( ae_uint32_t index = 0; index != _callbackData->parameter_count; ++index )
        {
            ShaderParameterDesc parameter;

            strcpy( parameter.uniform, _callbackData->parameter_uniforms[index] );
            parameter.type = _callbackData->parameter_types[index];

            float shader_values[4] = { 0.f };
            switch( parameter.type )
            {
            case AE_MOVIE_EXTENSION_SHADER_PARAMETER_SLIDER:
                {
                    ae_float_t value = _callbackData->parameter_values[index];

                    shader_values[0] = value;

                    programVariable->setPixelVariableFloats( parameter.uniform, desc->indexOffset + index, shader_values, 1, 1 );
                }break;
            case AE_MOVIE_EXTENSION_SHADER_PARAMETER_ANGLE:
                {
                    ae_float_t value = _callbackData->parameter_values[index];

                    shader_values[0] = value;

                    programVariable->setPixelVariableFloats( parameter.uniform, desc->indexOffset + index, shader_values, 1, 1 );
                }break;
            case AE_MOVIE_EXTENSION_SHADER_PARAMETER_COLOR:
                {
                    ae_color_t color_value = _callbackData->parameter_colors[index];

                    shader_values[0] = color_value.r;
                    shader_values[1] = color_value.g;
                    shader_values[2] = color_value.b;
                    shader_values[3] = 1.f;

                    programVariable->setPixelVariableFloats( parameter.uniform, desc->indexOffset + index, shader_values, 4, 1 );
                }break;
            }
        }

        desc->programVariable = programVariable;

        *_sd = desc;

        return AE_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_composition_shader_deleter( const aeMovieShaderDeleterCallbackData * _callbackData, ae_userdata_t _ud )
    {
        AE_UNUSED( _callbackData );
        AE_UNUSED( _ud );

        Movie2ShaderDesc * desc = reinterpret_cast<Movie2ShaderDesc *>(_callbackData->element_userdata);

        desc->programVariable = nullptr;

        Helper::freeT( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_composition_shader_property_update( const aeMovieShaderPropertyUpdateCallbackData * _callbackData, ae_voidptr_t _ud )
    {
        AE_UNUSED( _callbackData );
        AE_UNUSED( _ud );

        Movie2ShaderDesc * desc = reinterpret_cast<Movie2ShaderDesc *>(_callbackData->element_userdata);

        const RenderProgramVariableInterfacePtr & programVariable = desc->programVariable;

        float shader_values[4] = { 0.f };

        switch( _callbackData->type )
        {
        case AE_MOVIE_EXTENSION_SHADER_PARAMETER_SLIDER:
            {
                shader_values[0] = _callbackData->value;

                programVariable->updatePixelVariableFloats( desc->indexOffset + _callbackData->index, shader_values, 1, 1 );
            }break;
        case AE_MOVIE_EXTENSION_SHADER_PARAMETER_ANGLE:
            {
                shader_values[0] = _callbackData->value;

                programVariable->updatePixelVariableFloats( desc->indexOffset + _callbackData->index, shader_values, 1, 1 );
            }break;
        case AE_MOVIE_EXTENSION_SHADER_PARAMETER_COLOR:
            {
                shader_values[0] = _callbackData->color.r;
                shader_values[1] = _callbackData->color.g;
                shader_values[2] = _callbackData->color.b;
                shader_values[3] = 1.f;

                programVariable->updatePixelVariableFloats( desc->indexOffset + _callbackData->index, shader_values, 4, 1 );
            }break;
        case AE_MOVIE_EXTENSION_SHADER_PARAMETER_TIME:
            {
                shader_values[0] = _callbackData->value;

                programVariable->updatePixelVariableFloats( desc->indexOffset + _callbackData->index, shader_values, 1, 1 );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_composition_event( const aeMovieCompositionEventCallbackData * _callbackData, ae_voidptr_t _ud )
    {
        AE_UNUSED( _callbackData );
        AE_UNUSED( _ud );
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_composition_state( const aeMovieCompositionStateCallbackData * _callbackData, ae_voidptr_t _ud )
    {
        Movie2 * m2 = Helper::reinterpretNodeCast<Movie2 *>( _ud );

        switch( _callbackData->state )
        {
        case AE_MOVIE_COMPOSITION_PLAY:
        case AE_MOVIE_COMPOSITION_STOP:
        case AE_MOVIE_COMPOSITION_PAUSE:
        case AE_MOVIE_COMPOSITION_RESUME:
        case AE_MOVIE_COMPOSITION_LOOP_END:
            {
                //Empty
            }break;
        case AE_MOVIE_COMPOSITION_INTERRUPT:
            {
                if( m2->interruptElements_() == false )
                {
                    LOGGER_ERROR( "name '%s' invalid interrupt elements"
                        , m2->getName().c_str()
                    );
                }
            }break;
        case AE_MOVIE_COMPOSITION_LOOP_CONTINUOUS:
            {
            }break;
        case AE_MOVIE_COMPOSITION_END:
            {
                m2->end();
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_bool_t __movie_composition_extra_interrupt( const aeMovieCompositionExtraInterruptCallbackData * _callbackData, ae_voidptr_t _ud )
    {
        AE_UNUSED( _callbackData );

        Movie2 * m2 = Helper::reinterpretNodeCast<Movie2 *>( _ud );

        if( m2->checkInterruptElement_() == false )
        {
            return AE_FALSE;
        }

        return AE_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_bool_t __movie_scene_effect_provider( const aeMovieCompositionSceneEffectProviderCallbackData * _callbackData, ae_voidptrptr_t _sed, ae_voidptr_t _ud )
    {
        AE_UNUSED( _sed );

        Movie2 * m2 = Helper::reinterpretNodeCast<Movie2 *>( _ud );

        Layer * parent_layer = Helper::findParentNodeT<Layer *>( m2 );

        mt::vec3f anchor_point;
        anchor_point.from_f2( _callbackData->anchor_point, 0.f );
        parent_layer->setLocalOrigin( anchor_point );

        mt::vec3f position;
        position.from_f2( _callbackData->position, 0.f );
        parent_layer->setLocalPosition( position );

        mt::vec3f scale;
        scale.from_f2( _callbackData->scale, 1.f );
        parent_layer->setLocalScale( scale );

        mt::quatf q;
        q.x = 0.f;
        q.y = 0.f;
        q.z = _callbackData->quaternion[0];
        q.w = _callbackData->quaternion[1];
        float angle = quatzw_to_angle( q );

        parent_layer->setLocalOrientationX( angle );

        Color color( _callbackData->color.r, _callbackData->color.g, _callbackData->color.b, _callbackData->opacity );

        RenderInterface * parent_layer_render = parent_layer->getRender();
        parent_layer_render->setLocalColor( color );

        *_sed = parent_layer;

        return AE_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_scene_effect_update( const aeMovieCompositionSceneEffectUpdateCallbackData * _callbackData, ae_voidptr_t _ud )
    {
        AE_UNUSED( _ud );

        Layer * parent_layer = Helper::reinterpretNodeCast<Layer *>( _callbackData->scene_effect_userdata );

        mt::vec3f anchor_point;
        anchor_point.from_f2( _callbackData->anchor_point, 0.f );
        parent_layer->setLocalOrigin( anchor_point );

        mt::vec3f position;
        position.from_f2( _callbackData->position, 0.f );
        parent_layer->setLocalPosition( position );

        mt::vec3f scale;
        scale.from_f2( _callbackData->scale, 1.f );
        parent_layer->setLocalScale( scale );

        mt::quatf q;
        q.x = 0.f;
        q.y = 0.f;
        q.z = _callbackData->quaternion[0];
        q.w = _callbackData->quaternion[1];
        float angle = quatzw_to_angle( q );

        parent_layer->setLocalOrientationX( angle );

        Color color( _callbackData->color.r, _callbackData->color.g, _callbackData->color.b, _callbackData->opacity );

        RenderInterface * parent_layer_render = parent_layer->getRender();
        parent_layer_render->setLocalColor( color );
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_bool_t __movie_subcomposition_provider( const aeMovieSubCompositionProviderCallbackData * _callbackData, ae_voidptrptr_t _scd, ae_voidptr_t _ud )
    {
        Movie2 * m2 = Helper::reinterpretNodeCast<Movie2 *>( _ud );

        const aeMovieLayerData * layer = _callbackData->layer;

        const ae_char_t * layer_name = ae_get_movie_layer_data_name( layer );

        ConstString c_name = Helper::stringizeString( layer_name );

        const Movie2SubCompositionPtr & subcomposition = m2->getSubComposition( c_name );

        MENGINE_ASSERTION_MEMORY_PANIC( subcomposition, AE_FALSE );

        *_scd = subcomposition.get();

        return AE_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_subcomposition_deleter( const aeMovieSubCompositionDeleterCallbackData * _callbackData, ae_voidptr_t _ud )
    {
        AE_UNUSED( _callbackData );
        AE_UNUSED( _ud );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_subcomposition_state( const aeMovieSubCompositionStateCallbackData * _callbackData, ae_voidptr_t _ud )
    {
        AE_UNUSED( _ud );

        Movie2SubComposition * m2sc = Helper::reinterpretNodeCast<Movie2SubComposition *>( _callbackData->subcomposition_userdata );

        if( _callbackData->state == AE_MOVIE_COMPOSITION_END )
        {
            m2sc->end();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2::Camera * Movie2::addCamera_( const ConstString & _name, const RenderCameraProjectionPtr & _projection, const RenderViewportPtr & _viewport )
    {
        this->addChild( _projection );
        this->addChild( _viewport );

        Camera c;
        c.projection = _projection;
        c.viewport = _viewport;

        MapCameras::iterator it_found = m_cameras.emplace( _name, c ).first;

        Camera * new_camera = &it_found->second;

        return new_camera;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::removeCamera_( const ConstString & _name )
    {
        MapCameras::iterator it_found = m_cameras.find( _name );

        if( it_found == m_cameras.end() )
        {
            return false;
        }

        Camera & c = it_found->second;

        c.projection->removeFromParent();
        c.projection = nullptr;
        c.viewport->removeFromParent();
        c.viewport = nullptr;

        m_cameras.erase( it_found );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasCamera_( const ConstString & _name ) const
    {
        MapCameras::const_iterator it_found = m_cameras.find( _name );

        if( it_found == m_cameras.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::getCamera_( const ConstString & _name, Camera ** _camera )
    {
        MapCameras::iterator it_found = m_cameras.find( _name );

        if( it_found == m_cameras.end() )
        {
            return false;
        }

        Camera & camera = it_found->second;

        *_camera = &camera;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceMovie2, false, "name '%s' can't setup resource"
            , this->getName().c_str()
        );

        if( m_resourceMovie2->compile() == false )
        {
            LOGGER_ERROR( "name '%s' resource '%s' not compile"
                , this->getName().c_str()
                , m_resourceMovie2->getName().c_str()
            );

            return false;
        }

        const aeMovieCompositionData * compositionData = m_resourceMovie2->getCompositionData( m_compositionName );

        MENGINE_ASSERTION_MEMORY_PANIC( compositionData, false, "name '%s' resource '%s' not found composition '%s'"
            , this->getName().c_str()
            , m_resourceMovie2->getName().c_str()
            , m_compositionName.c_str()
        );

        const Movie2DataInterfacePtr & data = m_resourceMovie2->getData();

        const aeMovieData * movieData = data->getMovieData();

        aeMovieCompositionProviders providers;
        ae_initialize_movie_composition_providers( &providers );

        providers.camera_provider = &__movie_composition_camera_provider;
        providers.camera_deleter = &__movie_composition_camera_deleter;
        providers.camera_update = &__movie_composition_camera_update;

        providers.node_provider = &Movie2::__movie_composition_node_provider;
        providers.node_deleter = &__movie_composition_node_deleter;
        providers.node_update = &__movie_composition_node_update;

        providers.track_matte_provider = &__movie_composition_track_matte_provider;
        providers.track_matte_deleter = &__movie_composition_track_matte_deleter;
        providers.track_matte_update = &__movie_composition_track_matte_update;

        providers.shader_provider = &__movie_composition_shader_provider;
        providers.shader_deleter = &__movie_composition_shader_deleter;
        providers.shader_property_update = &__movie_composition_shader_property_update;

        providers.composition_event = &__movie_composition_event;
        providers.composition_state = &__movie_composition_state;
        providers.composition_extra_interrupt = &__movie_composition_extra_interrupt;

        providers.scene_effect_provider = &__movie_scene_effect_provider;
        providers.scene_effect_update = &__movie_scene_effect_update;

        providers.subcomposition_provider = &__movie_subcomposition_provider;
        providers.subcomposition_deleter = &__movie_subcomposition_deleter;
        providers.subcomposition_state = &__movie_subcomposition_state;

        const aeMovieComposition * composition = ae_create_movie_composition( movieData, compositionData, AE_TRUE, &providers, this );

        MENGINE_ASSERTION_MEMORY_PANIC( composition, false, "name '%s' resource '%s' invalid create composition '%s'"
            , this->getName().c_str()
            , m_resourceMovie2->getName().c_str()
            , m_compositionName.c_str()
        );

        m_composition = composition;

        aeMovieCompositionRenderInfo info;
        ae_calculate_movie_composition_render_info( m_composition, &info );

        if( info.max_vertex_count != 0 )
        {
            m_vertices.resize( info.max_vertex_count );
        }

        if( info.max_index_count != 0 )
        {
            m_indices.resize( info.max_index_count );
        }

        bool loop = this->isLoop();

        ae_set_movie_composition_loop( m_composition, loop ? AE_TRUE : AE_FALSE );

        float animationSpeedFactor = this->getAnimationSpeedFactor();
        this->updateAnimationSpeedFactor_( animationSpeedFactor );

        for( const HashtableSubCompositions::value_type & value : m_subCompositions )
        {
            const Movie2SubCompositionPtr & subComposition = value.element;

            if( subComposition->initialize( m_composition ) == false )
            {
                return false;
            }
        }

        bool play = this->isPlay();

        if( play == true )
        {
            float timing = this->getTime();

            ae_play_movie_composition( m_composition, timing * 0.001f );
        }
        else
        {
            float timing = this->getTime();

            ae_set_movie_composition_time( m_composition, timing * 0.001f );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_release()
    {
        ae_delete_movie_composition( m_composition );
        m_composition = nullptr;

        m_cameras.clear();

        for( const HashtableSubCompositions::value_type & value : m_subCompositions )
        {
            const Movie2SubCompositionPtr & subComposition = value.element;

            subComposition->finalize();
        }

        for( const SurfacePtr & surface : m_surfaces )
        {
            surface->release();
        }

        m_surfaces.clear();

        m_resourceMovie2->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::_activate()
    {
        if( Node::_activate() == false )
        {
            return false;
        }

        ae_vector3_t anchorPoint;
        if( ae_get_movie_composition_anchor_point( m_composition, anchorPoint ) == AE_TRUE )
        {
            mt::vec3f origin;

            origin.from_f3( anchorPoint );

            this->setLocalOrigin( origin );
        }

        for( const SurfacePtr & surface : m_surfaces )
        {
            surface->activate();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_deactivate()
    {
        Node::_deactivate();

        for( const SurfacePtr & surface : m_surfaces )
        {
            surface->deactivate();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_changeParent( Node * _oldParent, Node * _newParent )
    {
        MENGINE_UNUSED( _oldParent );
        MENGINE_UNUSED( _newParent );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_addChild( const NodePtr & _node )
    {
        MENGINE_UNUSED( _node );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_removeChild( const NodePtr & _node )
    {
        MENGINE_UNUSED( _node );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_afterActivate()
    {
        Node::_afterActivate();

        bool autoPlay = this->isAutoPlay();

        if( autoPlay == true )
        {
            float time = TIMELINE_SERVICE()
                ->getTime();

            if( this->play( time ) == 0 )
            {
                LOGGER_ERROR( "name '%s' resource '%s' auto play return 0"
                    , this->getName().c_str()
                    , this->m_resourceMovie2->getName().c_str()
                );

                return;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_setLoop( bool _value )
    {
        if( this->isCompile() == false )
        {
            return;
        }

        ae_set_movie_composition_loop( m_composition, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_setTime( float _time )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "name '%s' invalid compile"
                , this->getName().c_str()
            );

            return;
        }

        ae_set_movie_composition_time( m_composition, _time * 0.001f );
    }
    //////////////////////////////////////////////////////////////////////////
    float Movie2::_getTime() const
    {
        if( this->isCompile() == false )
        {
            return 0.f;
        }

        float timing = ae_get_movie_composition_time( m_composition );

        return AE_TIME_MILLISECOND( timing );
    }
    //////////////////////////////////////////////////////////////////////////
    float Movie2::_getDuration() const
    {
        return m_duration;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_setFirstFrame()
    {
        this->setTime( 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_setLastFrame()
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "name '%s' not activate"
                , this->getName().c_str()
            );

            return;
        }

        const aeMovieCompositionData * compositionData = ae_get_movie_composition_composition_data( m_composition );

        float duration = ae_get_movie_composition_data_duration( compositionData );
        float frameDuration = ae_get_movie_composition_data_frame_duration( compositionData );

        this->setTime( AE_TIME_MILLISECOND( (duration - frameDuration) ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_setAnimationSpeedFactor( float _factor )
    {
        if( this->isCompile() == false )
        {
            return;
        }

        this->updateAnimationSpeedFactor_( _factor );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::updateAnimationSpeedFactor_( float _factor )
    {
        for( const SurfacePtr & surface : m_surfaces )
        {
            AnimationInterface * animation = surface->getAnimation();

            if( animation == nullptr )
            {
                continue;
            }

            animation->setAnimationSpeedFactor( _factor );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::update( const UpdateContext * _context )
    {
        if( m_composition == nullptr )
        {
            return;
        }

        float totalTime = this->calcTotalTime( _context );

        ae_update_movie_composition( m_composition, totalTime * 0.001f );

        for( const SurfacePtr & surface : m_surfaces )
        {
            surface->update( _context );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::render( const RenderContext * _context ) const
    {
        uint32_t vertex_iterator = 0;
        uint32_t index_iterator = 0;

        RenderVertex2D * vertices_buffer = m_vertices.empty() == false ? m_vertices.data() : nullptr;
        RenderIndex * indices_buffer = m_indices.empty() == false ? m_indices.data() : nullptr;

        const mt::mat4f & wm = this->getWorldMatrix();

        Color total_color;
        this->calcTotalColor( total_color );

        float total_color_r = total_color.getR();
        float total_color_g = total_color.getG();
        float total_color_b = total_color.getB();
        float total_color_a = total_color.getA();

        ae_voidptr_t composition_camera_userdata = ae_get_movie_composition_camera_userdata( m_composition );
        AE_UNUSED( composition_camera_userdata );

        ae_uint32_t compute_movie_mesh_iterator = 0;

        aeMovieRenderMesh mesh;
        while( ae_compute_movie_mesh( m_composition, &compute_movie_mesh_iterator, &mesh ) == AE_TRUE )
        {
            RenderContext context;

            if( mesh.camera_userdata != nullptr )
            {
                Movie2::Camera * camera = reinterpret_cast<Movie2::Camera *>(mesh.camera_userdata);

                context.camera = camera->projection;
                context.viewport = camera->viewport;

                context.transformation = _context->transformation;
                context.scissor = _context->scissor;
                context.target = _context->target;
            }
            else
            {
                context = *_context;
            }

            if( mesh.viewport != nullptr )
            {
                Movie2ScissorPtr scissor = Helper::makeFactorableUnique<Movie2Scissor>();

                scissor->setViewport( wm, mesh.viewport );

                context.scissor = scissor;
            }
            else
            {
                context.scissor = _context->scissor;
            }

            if( mesh.track_matte_userdata == nullptr )
            {
                switch( mesh.layer_type )
                {
                case AE_MOVIE_LAYER_TYPE_SLOT:
                    {
                        Movie2Slot * node = Helper::reinterpretNodeCast<Movie2Slot *>( mesh.element_userdata );

                        Helper::nodeRenderChildren( node, &context, true );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SOCKET:
                    {
                        //HotSpotPolygon * node = reinterpret_node_cast<HotSpotPolygon *>(mesh.element_data);

                        //RenderInterfacePtr render = node->getRender();

                        //render->render( &state );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SPRITE:
                    {
                        ShapeQuadFixed * node = Helper::reinterpretNodeCast<ShapeQuadFixed *>( mesh.element_userdata );

                        Helper::nodeRenderChildren( node, &context, true );
                    }break;
                case AE_MOVIE_LAYER_TYPE_TEXT:
                    {
                        TextField * node = Helper::reinterpretNodeCast<TextField *>( mesh.element_userdata );

                        Helper::nodeRenderChildren( node, &context, true );
                    }break;
                case AE_MOVIE_LAYER_TYPE_PARTICLE:
                    {
                        Node * node = Helper::reinterpretNodeCast<Node *>( mesh.element_userdata );

                        Helper::nodeRenderChildren( node, &context, true );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SHAPE:
                    {
                        if( mesh.vertexCount == 0 )
                        {
                            continue;
                        }

                        ColorValue_ARGB total_mesh_color = Helper::makeARGB( total_color_r * mesh.color.r, total_color_g * mesh.color.g, total_color_b * mesh.color.b, total_color_a * mesh.opacity );

                        if( (total_mesh_color & 0xFF000000) == 0 )
                        {
                            continue;
                        }

                        RenderVertex2D * vertices = vertices_buffer + vertex_iterator;
                        vertex_iterator += mesh.vertexCount;

                        for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            RenderVertex2D & v = vertices[index];

                            mt::vec3f vp;
                            vp.from_f3( &mesh.position[index][0] );

                            mt::mul_v3_v3_m4( v.position, vp, wm );

                            v.color = total_mesh_color;
                        }

                        RenderIndex * indices = indices_buffer + index_iterator;
                        index_iterator += mesh.indexCount;

                        stdex::memorycopy_pod( indices, 0, mesh.indices, mesh.indexCount );

                        EMaterialBlendMode blend_mode = Detail::getMovieBlendMode( mesh.blend_mode );

                        const RenderMaterialInterfacePtr & material = RENDERMATERIAL_SERVICE()
                            ->getSolidMaterial( blend_mode );

                        this->addRenderObject( &context, material, nullptr, vertices, mesh.vertexCount, indices, mesh.indexCount, nullptr, false, MENGINE_DOCUMENT_FUNCTION );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SOLID:
                    {
                        if( mesh.vertexCount == 0 )
                        {
                            continue;
                        }

                        ColorValue_ARGB total_mesh_color = Helper::makeARGB( total_color_r * mesh.color.r, total_color_g * mesh.color.g, total_color_b * mesh.color.b, total_color_a * mesh.opacity );

                        if( (total_mesh_color & 0xFF000000) == 0 )
                        {
                            continue;
                        }

                        RenderVertex2D * vertices = vertices_buffer + vertex_iterator;
                        vertex_iterator += mesh.vertexCount;

                        for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            RenderVertex2D & v = vertices[index];

                            mt::vec3f vp;
                            vp.from_f3( &mesh.position[index][0] );

                            mt::mul_v3_v3_m4( v.position, vp, wm );

                            v.color = total_mesh_color;
                        }

                        RenderIndex * indices = indices_buffer + index_iterator;
                        index_iterator += mesh.indexCount;

                        stdex::memorycopy_pod( indices, 0, mesh.indices, mesh.indexCount );

                        EMaterialBlendMode blend_mode = Detail::getMovieBlendMode( mesh.blend_mode );

                        const RenderMaterialInterfacePtr & material = RENDERMATERIAL_SERVICE()
                            ->getSolidMaterial( blend_mode );

                        this->addRenderObject( &context, material, nullptr, vertices, mesh.vertexCount, indices, mesh.indexCount, nullptr, false, MENGINE_DOCUMENT_FUNCTION );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SEQUENCE:
                case AE_MOVIE_LAYER_TYPE_IMAGE:
                    {
                        if( mesh.vertexCount == 0 )
                        {
                            continue;
                        }

                        const aeMovieResourceImage * resource_image = reinterpret_cast<const aeMovieResourceImage *>(mesh.resource);

                        ae_userdata_t resource_userdata = ae_get_movie_resource_userdata( mesh.resource );

                        Movie2DataImageDesc * image_desc = reinterpret_cast<Movie2DataImageDesc *>(resource_userdata);

                        const ResourceImagePtr & resourceImage = image_desc->resourceImage;

                        const Color & imageColor = resourceImage->getColor();

                        ColorValue_ARGB total_mesh_color = Helper::makeARGB( total_color_r * mesh.color.r * imageColor.getR(), total_color_g * mesh.color.g * imageColor.getG(), total_color_b * mesh.color.b * imageColor.getB(), total_color_a * mesh.opacity * imageColor.getA() );

                        if( (total_mesh_color & 0xFF000000) == 0 )
                        {
                            continue;
                        }

                        RenderVertex2D * vertices = vertices_buffer + vertex_iterator;
                        vertex_iterator += mesh.vertexCount;

                        for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            RenderVertex2D & v = vertices[index];

                            mt::vec3f vp;
                            const float * vp3 = mesh.position[index];
                            vp.from_f3( vp3 );

                            mt::mul_v3_v3_m4( v.position, vp, wm );

                            v.color = total_mesh_color;
                        }

                        if( mesh.uv_cache_userdata == AE_NULLPTR )
                        {
                            for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                            {
                                RenderVertex2D & v = vertices[index];

                                mt::vec2f uv;
                                const float * uv2 = mesh.uv[index];
                                uv.from_f2( uv2 );

                                resourceImage->correctUVImage( uv, v.uv + 0 );
                                resourceImage->correctUVAlpha( uv, v.uv + 1 );
                            }
                        }
                        else
                        {
                            const mt::vec2f * uvs = reinterpret_cast<const mt::vec2f *>(mesh.uv_cache_userdata);

                            for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                            {
                                RenderVertex2D & v = vertices[index];

                                const mt::vec2f & uv = uvs[index];

                                v.uv[0] = uv;
                                v.uv[1] = uv;
                            }
                        }

                        if( mesh.shader_userdata != AE_NULLPTR )
                        {
                            Movie2ShaderDesc * shader_desc = reinterpret_cast<Movie2ShaderDesc *>(mesh.shader_userdata);

                            const RenderProgramVariableInterfacePtr & programVariable = shader_desc->programVariable;

                            mt::vec2f uv_zero;
                            mt::vec2f uv_one;
                            resourceImage->correctUVImage( mt::vec2f( 0.f, 0.f ), &uv_zero );
                            resourceImage->correctUVImage( mt::vec2f( 1.f, 1.f ), &uv_one );

                            const RenderTextureInterfacePtr & texture = resourceImage->getTexture();

                            const RenderImageInterfacePtr & image = texture->getImage();

                            float textureWidthInv = image->getHWWidthInv();
                            float textureHeightInv = image->getHWHeightInv();

                            float u = uv_one.x - uv_zero.x;

                            float ou = uv_zero.x - resource_image->offset_x * textureWidthInv;
                            float du = resource_image->base_width / resource_image->trim_width * u;

                            float v = uv_one.y - uv_zero.y;

                            float ov = uv_zero.y - resource_image->offset_y * textureHeightInv;
                            float dv = resource_image->base_height / resource_image->trim_height * v;

                            float uvsl[4] = { -ou, -ov, 1.f / du, 1.f / dv };
                            programVariable->setPixelVariableFloats( "uvsl", 0, uvsl, 4, 1 );
                        }

                        RenderIndex * indices = indices_buffer + index_iterator;
                        index_iterator += mesh.indexCount;

                        stdex::memorycopy_pod( indices, 0, mesh.indices, mesh.indexCount );

                        EMaterialBlendMode blend_mode = Detail::getMovieBlendMode( mesh.blend_mode );

                        if( mesh.shader_userdata == AE_NULLPTR )
                        {
                            RenderMaterialInterfacePtr material = image_desc->materials[blend_mode];

                            this->addRenderObject( &context, material, nullptr, vertices, mesh.vertexCount, indices, mesh.indexCount, nullptr, false, MENGINE_DOCUMENT_FUNCTION );
                        }
                        else
                        {
                            Movie2ShaderDesc * shader_desc = reinterpret_cast<Movie2ShaderDesc *>(mesh.shader_userdata);

                            ConstString materialName;

                            if( resourceImage->getTextureAlpha() == nullptr )
                            {
                                switch( blend_mode )
                                {
                                case EMB_NORMAL:
                                    {
                                        materialName = shader_desc->materialNameBlend;
                                    }break;
                                default:
                                    {
                                        LOGGER_ERROR( "invalid support shader material blend mode '%d' (Normal)"
                                            , blend_mode
                                        );

                                        continue;
                                    }break;
                                }
                            }
                            else
                            {
                                switch( blend_mode )
                                {
                                case EMB_NORMAL:
                                    {
                                        materialName = shader_desc->materialNameBlendExternalAlpha;
                                    }break;
                                default:
                                    {
                                        LOGGER_ERROR( "invalid support shader material blend mode '%d' (ExternalAlpha)"
                                            , blend_mode
                                        );

                                        continue;
                                    }break;
                                }
                            }

                            RenderMaterialInterfacePtr material = Helper::makeImageMaterial( resourceImage, materialName, blend_mode, false, false, MENGINE_DOCUMENT_FUNCTION );

                            const RenderProgramVariableInterfacePtr & programVariable = shader_desc->programVariable;

                            this->addRenderObject( &context, material, programVariable, vertices, mesh.vertexCount, indices, mesh.indexCount, nullptr, false, MENGINE_DOCUMENT_FUNCTION );
                        }
                    }break;
                case AE_MOVIE_LAYER_TYPE_VIDEO:
                    {
                        if( mesh.vertexCount == 0 )
                        {
                            continue;
                        }

                        Surface * surface = Helper::reinterpretNodeCast<Surface *>( mesh.element_userdata );

                        const Color & surfaceColor = surface->getColor();

                        ColorValue_ARGB total_mesh_color = Helper::makeARGB( total_color_r * mesh.color.r * surfaceColor.getR(), total_color_g * mesh.color.g * surfaceColor.getG(), total_color_b * mesh.color.b * surfaceColor.getB(), total_color_a * mesh.opacity * surfaceColor.getA() );

                        if( (total_mesh_color & 0xFF000000) == 0 )
                        {
                            continue;
                        }

                        RenderVertex2D * vertices = vertices_buffer + vertex_iterator;
                        vertex_iterator += mesh.vertexCount;

                        for( ae_uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            RenderVertex2D & v = vertices[index];

                            mt::vec3f vp;
                            vp.from_f3( &mesh.position[index][0] );

                            mt::mul_v3_v3_m4( v.position, vp, wm );

                            mt::vec2f uv;
                            uv.from_f2( &mesh.uv[index][0] );

                            surface->correctUV( 0, uv, v.uv + 0 );
                            surface->correctUV( 1, uv, v.uv + 1 );

                            v.color = total_mesh_color;
                        }

                        RenderIndex * indices = indices_buffer + index_iterator;
                        index_iterator += mesh.indexCount;

                        stdex::memorycopy_pod( indices, 0, mesh.indices, mesh.indexCount );

                        const RenderMaterialInterfacePtr & material = surface->getMaterial();

                        this->addRenderObject( &context, material, nullptr, vertices, mesh.vertexCount, indices, mesh.indexCount, nullptr, false, MENGINE_DOCUMENT_FUNCTION );
                    }break;
                default:
                    break;
                }
            }
            else
            {
                switch( mesh.layer_type )
                {
                case AE_MOVIE_LAYER_TYPE_IMAGE:
                    {
                        if( mesh.vertexCount == 0 )
                        {
                            continue;
                        }

                        const SurfaceTrackMatte * surfaceTrackMatte = Helper::reinterpretNodeCast<const SurfaceTrackMatte *>( mesh.element_userdata );

                        RenderVertex2D * vertices = vertices_buffer + vertex_iterator;
                        vertex_iterator += mesh.vertexCount;

                        const ResourceImagePtr & resourceImage = surfaceTrackMatte->getResourceImage();
                        const ResourceImagePtr & resourceTrackMatteImage = surfaceTrackMatte->getResourceTrackMatteImage();

                        const TrackMatteDesc * track_matte_desc = reinterpret_cast<const TrackMatteDesc *>(mesh.track_matte_userdata);

                        const aeMovieRenderMesh * track_matte_mesh = &track_matte_desc->mesh;

                        const Color & imageColor = resourceImage->getColor();

                        ColorValue_ARGB total_mesh_color = Helper::makeARGB( total_color_r * mesh.color.r * imageColor.getR(), total_color_g * mesh.color.g * imageColor.getG(), total_color_b * mesh.color.b * imageColor.getB(), total_color_a * mesh.opacity * imageColor.getA() );

                        if( (total_mesh_color & 0xFF000000) == 0 )
                        {
                            continue;
                        }

                        for( uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            RenderVertex2D & v = vertices[index];

                            mt::vec3f vp;
                            vp.from_f3( mesh.position[index] );

                            mt::mul_v3_v3_m4( v.position, vp, wm );

                            mt::vec2f uv;
                            uv.from_f2( &mesh.uv[index][0] );

                            resourceImage->correctUVImage( uv, v.uv + 0 );

                            mt::vec2f uv_track_matte = mt::calc_point_uv(
                                mt::vec2f( track_matte_mesh->position[0] ), mt::vec2f( track_matte_mesh->position[1] ), mt::vec2f( track_matte_mesh->position[2] ),
                                mt::vec2f( track_matte_mesh->uv[0] ), mt::vec2f( track_matte_mesh->uv[1] ), mt::vec2f( track_matte_mesh->uv[2] ),
                                vp.to_vec2f()
                            );

                            resourceTrackMatteImage->correctUVAlpha( uv_track_matte, v.uv + 1 );

                            v.color = total_mesh_color;
                        }

                        RenderIndex * indices = indices_buffer + index_iterator;
                        index_iterator += mesh.indexCount;

                        stdex::memorycopy_pod( indices, 0, mesh.indices, mesh.indexCount );

                        const RenderProgramVariableInterfacePtr & programVariable = surfaceTrackMatte->getProgramVariable();

                        float uvbb[4];
                        uvbb[0] = std::numeric_limits<float>::max();
                        uvbb[1] = std::numeric_limits<float>::max();
                        uvbb[2] = -std::numeric_limits<float>::max();
                        uvbb[3] = -std::numeric_limits<float>::max();

                        for( uint32_t index = 0; index != track_matte_mesh->vertexCount; ++index )
                        {
                            mt::vec2f uv;
                            uv.from_f2( &track_matte_mesh->uv[index][0] );

                            mt::vec2f uv_correct;
                            resourceTrackMatteImage->correctUVAlpha( uv, &uv_correct );

                            if( uvbb[0] > uv_correct.x )
                            {
                                uvbb[0] = uv_correct.x;
                            }

                            if( uvbb[1] > uv_correct.y )
                            {
                                uvbb[1] = uv_correct.y;
                            }

                            if( uvbb[2] < uv_correct.x )
                            {
                                uvbb[2] = uv_correct.x;
                            }

                            if( uvbb[3] < uv_correct.y )
                            {
                                uvbb[3] = uv_correct.y;
                            }
                        }

                        programVariable->setPixelVariableFloats( "uvbb", 0, uvbb, 4, 1 );

                        const RenderMaterialInterfacePtr & material = surfaceTrackMatte->getMaterial();

                        this->addRenderObject( &context, material, programVariable, vertices, mesh.vertexCount, indices, mesh.indexCount, nullptr, false, MENGINE_DOCUMENT_FUNCTION );
                    }break;
                case AE_MOVIE_LAYER_TYPE_SEQUENCE:
                    {
                        if( mesh.vertexCount == 0 )
                        {
                            continue;
                        }

                        SurfaceTrackMatte * surfaceTrackMatte = Helper::reinterpretNodeCast<SurfaceTrackMatte *>( mesh.element_userdata );

                        RenderVertex2D * vertices = vertices_buffer + vertex_iterator;
                        vertex_iterator += mesh.vertexCount;

                        Movie2DataImageDesc * imageDesc = reinterpret_cast<Movie2DataImageDesc *>(ae_get_movie_resource_userdata( mesh.resource ));
                        surfaceTrackMatte->setResourceImage( imageDesc->resourceImage );

                        surfaceTrackMatte->compile();

                        const ResourceImagePtr & resourceImage = surfaceTrackMatte->getResourceImage();
                        const ResourceImagePtr & resourceTrackMatteImage = surfaceTrackMatte->getResourceTrackMatteImage();

                        const TrackMatteDesc * track_matte_desc = reinterpret_cast<const TrackMatteDesc *>(mesh.track_matte_userdata);

                        const aeMovieRenderMesh * track_matte_mesh = &track_matte_desc->mesh;

                        const Color & imageColor = resourceImage->getColor();

                        ColorValue_ARGB total_mesh_color = Helper::makeARGB( total_color_r * mesh.color.r * imageColor.getR(), total_color_g * mesh.color.g * imageColor.getG(), total_color_b * mesh.color.b * imageColor.getB(), total_color_a * mesh.opacity * imageColor.getA() );

                        if( (total_mesh_color & 0xFF000000) == 0 )
                        {
                            continue;
                        }

                        for( uint32_t index = 0; index != mesh.vertexCount; ++index )
                        {
                            RenderVertex2D & v = vertices[index];

                            mt::vec3f vp;
                            vp.from_f3( mesh.position[index] );

                            mt::mul_v3_v3_m4( v.position, vp, wm );

                            mt::vec2f uv;
                            uv.from_f2( &mesh.uv[index][0] );

                            resourceImage->correctUVImage( uv, v.uv + 0 );

                            mt::vec2f uv_track_matte = mt::calc_point_uv(
                                mt::vec2f( track_matte_mesh->position[0] ), mt::vec2f( track_matte_mesh->position[1] ), mt::vec2f( track_matte_mesh->position[2] ),
                                mt::vec2f( track_matte_mesh->uv[0] ), mt::vec2f( track_matte_mesh->uv[1] ), mt::vec2f( track_matte_mesh->uv[2] ),
                                vp.to_vec2f()
                            );

                            resourceTrackMatteImage->correctUVAlpha( uv_track_matte, v.uv + 1 );

                            v.color = total_mesh_color;
                        }

                        RenderIndex * indices = indices_buffer + index_iterator;
                        index_iterator += mesh.indexCount;

                        stdex::memorycopy_pod( indices, 0, mesh.indices, mesh.indexCount );

                        const RenderProgramVariableInterfacePtr & programVariable = surfaceTrackMatte->getProgramVariable();

                        float uvbb[4];
                        uvbb[0] = std::numeric_limits<float>::max();
                        uvbb[1] = std::numeric_limits<float>::max();
                        uvbb[2] = -std::numeric_limits<float>::max();
                        uvbb[3] = -std::numeric_limits<float>::max();

                        for( uint32_t index = 0; index != track_matte_mesh->vertexCount; ++index )
                        {
                            mt::vec2f uv;
                            uv.from_f2( &track_matte_mesh->uv[index][0] );

                            mt::vec2f uv_correct;
                            resourceTrackMatteImage->correctUVAlpha( uv, &uv_correct );

                            if( uvbb[0] > uv_correct.x )
                            {
                                uvbb[0] = uv_correct.x;
                            }

                            if( uvbb[1] > uv_correct.y )
                            {
                                uvbb[1] = uv_correct.y;
                            }

                            if( uvbb[2] < uv_correct.x )
                            {
                                uvbb[2] = uv_correct.x;
                            }

                            if( uvbb[3] < uv_correct.y )
                            {
                                uvbb[3] = uv_correct.y;
                            }
                        }

                        programVariable->setPixelVariableFloats( "uvbb", 0, uvbb, 4, 1 );

                        const RenderMaterialInterfacePtr & material = surfaceTrackMatte->getMaterial();

                        this->addRenderObject( &context, material, programVariable, vertices, mesh.vertexCount, indices, mesh.indexCount, nullptr, false, MENGINE_DOCUMENT_FUNCTION );
                    }break;
                default:
                    break;
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addSurface_( const SurfacePtr & _surface, bool _compile )
    {
        if( _compile == true )
        {
            _surface->compile();
        }

        m_surfaces.emplace_back( _surface );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::removeSurface_( const SurfacePtr & _surface )
    {
        _surface->release();

        VectorSurfaces::iterator it_found = std::find( m_surfaces.begin(), m_surfaces.end(), _surface );

        m_surfaces.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addSprite_( uint32_t _index, const ShapeQuadFixedPtr & _sprite )
    {
        m_sprites.emplace( _index, _sprite );
    }
    //////////////////////////////////////////////////////////////////////////
    const ShapeQuadFixedPtr & Movie2::getSprite_( uint32_t _index ) const
    {
        const ShapeQuadFixedPtr & sprite = m_sprites.find( _index );

        return sprite;
    }
    //////////////////////////////////////////////////////////////////////////
    const ShapeQuadFixedPtr & Movie2::findSprite( const ConstString & _name ) const
    {
        for( const HashtableSprites::value_type & value : m_sprites )
        {
            const ShapeQuadFixedPtr & sprite = value.element;

            if( sprite->getName() != _name )
            {
                continue;
            }

            return sprite;
        }

        return ShapeQuadFixedPtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasSprite( const ConstString & _name ) const
    {
        for( const HashtableSprites::value_type & value : m_sprites )
        {
            const ShapeQuadFixedPtr & sprite = value.element;

            if( sprite->getName() != _name )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addParticle_( uint32_t _index, const NodePtr & _particleEmitter )
    {
        m_particleEmitters.emplace( _index, _particleEmitter );
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & Movie2::getParticle_( uint32_t _index ) const
    {
        const NodePtr & particleEmitter = m_particleEmitters.find( _index );

        return particleEmitter;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & Movie2::findParticle( const ConstString & _name ) const
    {
        for( const HashtableParticleEmitter2s::value_type & value : m_particleEmitters )
        {
            const NodePtr & particle = value.element;

            if( particle->getName() != _name )
            {
                continue;
            }

            return particle;
        }

        return NodePtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasParticle( const ConstString & _name ) const
    {
        for( const HashtableParticleEmitter2s::value_type & value : m_particleEmitters )
        {
            const NodePtr & particle = value.element;

            if( particle->getName() != _name )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addSlot_( uint32_t _index, const Movie2SlotPtr & _slot )
    {
        m_slots.emplace( _index, _slot );
    }
    //////////////////////////////////////////////////////////////////////////
    const Movie2SlotPtr & Movie2::getSlot_( uint32_t _index ) const
    {
        const Movie2SlotPtr & slot = m_slots.find( _index );

        return slot;
    }
    //////////////////////////////////////////////////////////////////////////
    const Movie2SlotPtr & Movie2::findSlot( const ConstString & _name ) const
    {
        for( const HashtableSlots::value_type & value : m_slots )
        {
            const Movie2SlotPtr & slot = value.element;

            if( slot->getName() != _name )
            {
                continue;
            }

            return slot;
        }

        return Movie2SlotPtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasSlot( const ConstString & _name ) const
    {
        for( const HashtableSlots::value_type & value : m_slots )
        {
            const Movie2SlotPtr & slot = value.element;

            if( slot->getName() != _name )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::visitSlots( const VisitorMovie2LayerInterfacePtr & _visitor )
    {
        for( const HashtableSlots::value_type & value : m_slots )
        {
            uint32_t index = value.key;
            const Movie2SlotPtr & node = value.element;

            _visitor->visitMovieLayer( this, index, node );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addSocket_( uint32_t _index, const HotSpotPolygonPtr & _hotspot )
    {
        m_sockets.emplace( _index, _hotspot );
    }
    //////////////////////////////////////////////////////////////////////////
    const HotSpotPolygonPtr & Movie2::getSocket_( uint32_t _index ) const
    {
        const HotSpotPolygonPtr & hotspot = m_sockets.find( _index );

        return hotspot;
    }
    //////////////////////////////////////////////////////////////////////////
    const HotSpotPolygonPtr & Movie2::findSocket( const ConstString & _name ) const
    {
        for( const HashtableSockets::value_type & value : m_sockets )
        {
            const HotSpotPolygonPtr & socket = value.element;

            if( socket->getName() != _name )
            {
                continue;
            }

            return socket;
        }

        return HotSpotPolygonPtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasSocket( const ConstString & _name ) const
    {
        for( const HashtableSockets::value_type & value : m_sockets )
        {
            const HotSpotPolygonPtr & socket = value.element;

            if( socket->getName() != _name )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::visitSockets( const VisitorMovie2LayerInterfacePtr & _visitor )
    {
        for( const HashtableSockets::value_type & value : m_sockets )
        {
            uint32_t key = value.key;
            const HotSpotPolygonPtr & hotspot = value.element;

            _visitor->visitMovieLayer( this, key, hotspot );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addText_( uint32_t _index, const TextFieldPtr & _text )
    {
        m_texts.emplace( _index, _text );
    }
    //////////////////////////////////////////////////////////////////////////
    const TextFieldPtr & Movie2::getText_( uint32_t _index ) const
    {
        const TextFieldPtr & text = m_texts.find( _index );

        return text;
    }
    //////////////////////////////////////////////////////////////////////////
    const TextFieldPtr & Movie2::findText( const ConstString & _name ) const
    {
        for( const HashtableTexts::value_type & value : m_texts )
        {
            const TextFieldPtr & text = value.element;

            if( text->getName() != _name )
            {
                continue;
            }

            return text;
        }

        return TextFieldPtr::none();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2::hasText( const ConstString & _name ) const
    {
        for( const HashtableTexts::value_type & value : m_texts )
        {
            const TextFieldPtr & text = value.element;

            if( text->getName() != _name )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::visitTexts( const VisitorMovie2LayerInterfacePtr & _visitor )
    {
        for( const HashtableTexts::value_type & value : m_texts )
        {
            uint32_t index = value.key;
            const TextFieldPtr & node = value.element;

            _visitor->visitMovieLayer( this, index, node );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addSubMovieComposition_( const ConstString & _name, const Movie2SubCompositionPtr & _subComposition )
    {
        m_subCompositions.emplace( _name, _subComposition );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::addMatrixProxy_( const MatrixProxyPtr & _matrixProxy )
    {
        m_matrixProxies.emplace_back( _matrixProxy );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2::_destroy()
    {
        this->destroyCompositionLayers_();

        Node::_destroy();
    }
}

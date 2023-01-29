#include "Movie.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/TimelineServiceInterface.h"

#include "Plugins/AstralaxPlugin/AstralaxInterface.h"
#include "Plugins/VideoPlugin/VideoInterface.h"

#include "ResourceMovie.h"

#include "Engine/ResourceHIT.h"
#include "Engine/ResourceShape.h"
#include "Engine/TextField.h"
#include "Engine/HotSpotImage.h"
#include "Engine/HotSpotResourceShape.h"
#include "Engine/HotSpotSurface.h"
#include "Engine/SoundEmitter.h"

#include "Kernel/ShapeQuadFixed.h"
#include "Kernel/SurfaceImage.h"
#include "Kernel/SurfaceImageSequence.h"
#include "Kernel/SurfaceSound.h"
#include "Kernel/ResourceImageSequence.h"
#include "Kernel/ResourceImageSolid.h"
#include "Kernel/ResourceSound.h"
#include "Kernel/Layer.h"
#include "Kernel/Scene.h"
#include "Kernel/NodeHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/RenderScissor.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/EventableHelper.h"

#include "MovieSceneEffect.h"
#include "MovieSlot.h"
#include "MovieNodeExtra.h"
#include "MovieMesh2D.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "MovieInternalObject.h"
#   include "MovieEvent.h"
#endif

#include "math/angle.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Movie::Movie()
        : m_frameTime( 0.f )
        , m_currentFrame( 0 )
        , m_interruptEnd( false )
        , m_parentMovie( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie::~Movie()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::setResourceMovie( const ResourceMoviePtr & _resourceMovie )
    {
        if( m_resourceMovie == _resourceMovie )
        {
            return;
        }

        this->destroyLayers_();

        this->recompile( [this, &_resourceMovie]()
        {
            m_resourceMovie = _resourceMovie;

            if( m_resourceMovie == nullptr )
            {
                return false;
            }

            if( this->createLayers_() == false )
            {
                LOGGER_ERROR( "'%s' resource '%s' can't create layers"
                    , this->getName().c_str()
                    , m_resourceMovie->getName().c_str()
                );

                m_resourceMovie = nullptr;

                return false;
            }

            return true;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceMoviePtr & Movie::getResourceMovie() const
    {
        return m_resourceMovie;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Movie::getResourceMovieName() const
    {
        if( m_resourceMovie == nullptr )
        {
            return ConstString::none();
        }

        const ConstString & name = m_resourceMovie->getName();

        return name;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::setParentMovie( bool _value )
    {
        m_parentMovie = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::isParentMovie() const
    {
        return m_parentMovie;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::getFrameTime_( float _time, uint32_t * const _frame, float * const _delthaTime ) const
    {
        float duration = m_resourceMovie->getDuration();

        float crop_timing = mt::crop( _time, 0.f, duration );

        bool loop = this->isLoop();

        float frameDuration = m_resourceMovie->getFrameDuration();

        uint32_t frame = (uint32_t)(crop_timing / frameDuration + 0.5f);

        uint32_t frameCount = m_resourceMovie->getFrameCount();

        if( frameCount == 0 )
        {
            *_frame = 0;
            *_delthaTime = 0.f;
        }
        else if( loop == true )
        {
            *_frame = frame % frameCount;
            *_delthaTime = crop_timing - *_frame * frameDuration;
        }
        else
        {
            if( frame >= frameCount )
            {
                *_frame = frameCount - 1;
                *_delthaTime = 0.f;
            }
            else
            {
                *_frame = frame;
                *_delthaTime = crop_timing - *_frame * frameDuration;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::_setTime( float _time )
    {
        if( m_resourceMovie == nullptr )
        {
            LOGGER_ERROR( "'%s' not activate"
                , this->getName().c_str()
            );

            return;
        }

        uint32_t frame;
        float frameTime;
        this->getFrameTime_( _time, &frame, &frameTime );

        m_currentFrame = frame;
        m_frameTime = frameTime;

        if( this->isActivate() == true )
        {
            this->updateTime_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float Movie::_getTime() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "movie '%s' not compile"
                , this->getName().c_str()
            );

            return 0.f;
        }

        float frameDuration = m_resourceMovie->getFrameDuration();

        float timing = m_currentFrame * frameDuration + m_frameTime;

        return timing;
    }
    //////////////////////////////////////////////////////////////////////////
    float Movie::_getDuration() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "movie '%s' not compile"
                , this->getName().c_str()
            );

            return 0.f;
        }

        float duration = m_resourceMovie->getDuration();

        return duration;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::_play( uint32_t _playId, float _time )
    {
        MENGINE_UNUSED( _playId );
        MENGINE_UNUSED( _time );

        if( this->isActivate() == false )
        {
            if( this->hasParent() == false )
            {
                LOGGER_ERROR( "'%s' is homeless"
                    , this->getName().c_str()
                );
            }
            else
            {
                LOGGER_ERROR( "'%s' play not activate"
                    , this->getName().c_str()
                );
            }

            return false;
        }

        this->updateAnimatablePlay_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::_restart( uint32_t _playId, float _time )
    {
        MENGINE_UNUSED( _time );
        MENGINE_UNUSED( _playId );

        if( this->isActivate() == false )
        {
            return false;
        }

        this->updateAnimatablePlay_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::_pause( uint32_t _playId )
    {
        MENGINE_UNUSED( _playId );

        this->pauseAnimation_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::_resume( uint32_t _playId, float _time )
    {
        MENGINE_UNUSED( _time );
        MENGINE_UNUSED( _playId );

        this->resumeAnimation_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::_stop( uint32_t _playId )
    {
        this->stopAnimation_();

        EVENTABLE_METHOD( EVENT_ANIMATION_STOP )
            ->onAnimationStop( _playId );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::_end( uint32_t _playId )
    {
        this->stopAnimation_();

        EVENTABLE_METHOD( EVENT_ANIMATION_END )
            ->onAnimationEnd( _playId );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::updateFrameNode_( const MovieLayer & _layer, const NodePtr & _node, uint32_t _frameId, bool _interpolate, bool _first )
    {
        const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

        if( _layer.isMesh2D() == true )
        {
#ifdef MENGINE_DEBUG
            if( MovieMesh2DPtr::dynamic_from( _node ) == nullptr )
            {
                LOGGER_ERROR( "'%s' resource '%s' layer '%s' is Mesh2D but node is not Mesh2D name '%s' type '%s'"
                    , this->getName().c_str()
                    , this->getResourceMovieName().c_str()
                    , _layer.name.c_str()
                    , _node->getName().c_str()
                    , _node->getType().c_str()
                );

                return false;
            }
#endif

            MovieMesh2DPtr mesh2D = MovieMesh2DPtr::from( _node );

            const MovieFrameShape * shape;
            if( framePack->getLayerShape( _layer.index, _frameId, &shape ) == false )
            {
                return false;
            }

            mesh2D->setFrameShape( shape );
        }

        const MovieLayerFrame & layerFrame = framePack->getLayer( _layer.index );

        bool immutable = layerFrame.immutable;

        if( immutable == true && _first == false )
        {
            return true;
        }

        MovieFrameSource frame;
        if( _interpolate == true && immutable == false )
        {
            float frameDuration = m_resourceMovie->getFrameDuration();

            float t = m_frameTime / frameDuration;

            if( framePack->getLayerFrameInterpolate( _layer.index, _frameId, t, &frame ) == false )
            {
                return false;
            }
        }
        else
        {
            if( framePack->getLayerFrame( _layer.index, _frameId, &frame ) == false )
            {
                return false;
            }
        }

        mt::vec2f skew( 0.f, 0.f );

        TransformationInterface * transformation = _node->getTransformation();

        transformation->setTransformationData(
            TRANSFORMATION_INVALIDATE_UNKNOWN,
            frame.position,
            frame.anchorPoint,
            frame.scale,
            skew,
            frame.rotation
        );

        const SoundablePtr & soundable = this->getLayerSoundable_( _layer );

        if( soundable == nullptr )
        {
            RenderInterface * render = _node->getRender();

            if( render != nullptr )
            {
                if( _layer.isMovie() == false )
                {
                    render->setPersonalAlpha( frame.opacity );
                }
                else
                {
                    render->setLocalAlpha( frame.opacity );
                }
            }
        }
        else
        {
            soundable->setVolume( frame.volume );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::_setFirstFrame()
    {
        if( m_resourceMovie == nullptr )
        {
            LOGGER_ERROR( "'%s' not activate"
                , this->getName().c_str()
            );

            return;
        }

        this->setTime( 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::_setLastFrame()
    {
        if( m_resourceMovie == nullptr )
        {
            LOGGER_ERROR( "'%s' not activate"
                , this->getName().c_str()
            );

            return;
        }

        float duration = m_resourceMovie->getDuration();
        float frameDuration = m_resourceMovie->getFrameDuration();

        this->setTime( duration - frameDuration );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::addMovieNode_( const MovieLayer & _layer, const NodePtr & _node, const AnimatablePtr & _animatable, const SoundablePtr & _soundable, const MoviePtr & _movie )
    {
        MENGINE_UNUSED( _animatable );

        if( _layer.hasViewport == true )
        {
            RenderScissorPtr scissor = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "RenderScissor" ), MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( scissor );

            scissor->setName( _layer.name );
            scissor->setViewport( _layer.viewport );

            RenderInterface * render = _node->getRender();
            render->setRenderScissor( scissor );

            m_scissors.emplace_back( scissor );

            this->addChild( scissor );
        }

        Nodies nd;
        nd.node = _node;
        nd.render = _node->getRender();

        if( _layer.in <= 0.0001f )
        {
            nd.visible = true;

            if( nd.render != nullptr )
            {
                nd.render->setLocalHide( false );
            }
        }
        else
        {
            nd.visible = false;

            if( nd.render != nullptr )
            {
                nd.render->setLocalHide( true );
            }
        }

        nd.animation = _animatable != nullptr ? _animatable->getAnimation() : nullptr;

        if( _layer.isAnimation() == true && nd.animation == nullptr )
        {
            return false;
        }

        nd.soundable = _soundable;
        nd.movie = _movie;

        nd.child = (_layer.parent != movie_layer_parent_none);

        _node->setName( _layer.name );

        mt::vec2f skew( 0.f, 0.f );

        TransformationInterface * transformation = _node->getTransformation();

        transformation->setTransformationData(
            TRANSFORMATION_INVALIDATE_UNKNOWN,
            _layer.position,
            _layer.anchorPoint,
            _layer.scale,
            skew,
            _layer.rotation
        );

        m_nodies[_layer.index - 1] = nd;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::foreachMovieLayer( const ConstString & _type, const LambdaMovieNodes & _lambda )
    {
        if( m_resourceMovie == nullptr )
        {
            LOGGER_ERROR( "'%s' invalid not compile"
                , this->getName().c_str()
            );

            return false;
        }

        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            if( _type.empty() == false && layer.type != _type )
            {
                continue;
            }

            const NodePtr & node = this->getLayerNode_( layer );

            _lambda( this, node );
        }

        for( const MovieLayer & layer : layers )
        {
            if( layer.isMovie() == false || layer.isSubMovie() == true )
            {
                continue;
            }

            const MoviePtr & movie = this->getLayerMovie_( layer );

            if( movie->foreachMovieLayer( _type, _lambda ) == false )
            {
                continue;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::getMovieNode( const ConstString & _name, const ConstString & _type, NodePtr * const _node, MoviePtr * const _movie ) const
    {
        if( m_resourceMovie == nullptr )
        {
            LOGGER_ERROR( "'%s' resource '%s' invalid not compile '%s' type '%s'"
                , this->getName().c_str()
                , this->getResourceMovieName().c_str()
                , _name.c_str()
                , _type.c_str()
            );

            return false;
        }

        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            if( layer.name != _name || (layer.type != _type && _type.empty() == false) )
            {
                continue;
            }

            const NodePtr & node = this->getLayerNode_( layer );

            *_node = node;
            *_movie = this;

            return true;
        }

        for( const MovieLayer & layer : layers )
        {
            if( layer.isMovie() == false || layer.isSubMovie() == true )
            {
                continue;
            }

            const MoviePtr & movie = this->getLayerMovie_( layer );

            if( movie->getMovieNode( _name, _type, _node, _movie ) == true )
            {
                return true;
            }
        }

        LOGGER_ERROR( "'%s' resource '%s' not found node '%s' type '%s'"
            , this->getName().c_str()
            , this->getResourceMovieName().c_str()
            , _name.c_str()
            , _type.c_str()
        );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::hasMovieNode( const ConstString & _name, const ConstString & _type, NodePtr * const _node, MoviePtr * const _movie ) const
    {
        if( m_resourceMovie == nullptr )
        {
            LOGGER_ERROR( "'%s' resource '%s' invalid not compile name '%s' type '%s'"
                , this->getName().c_str()
                , this->getResourceMovieName().c_str()
                , _name.c_str()
                , _type.c_str()
            );

            return false;
        }

        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            if( layer.name != _name || (layer.type != _type && _type.empty() == false) )
            {
                continue;
            }

            const NodePtr & node = this->getLayerNode_( layer );

            *_node = node;
            *_movie = this;

            return true;
        }

        for( const MovieLayer & layer : layers )
        {
            if( layer.isMovie() == false || layer.isSubMovie() == true )
            {
                continue;
            }

            const MoviePtr & movie = this->getLayerMovie_( layer );

            if( movie->hasMovieNode( _name, _type, _node, _movie ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::hasMovieLayer( const ConstString & _name ) const
    {
        if( m_resourceMovie == nullptr )
        {
            LOGGER_ERROR( "'%s' resource '%s' invalid get layer '%s' not compile"
                , this->getName().c_str()
                , this->getResourceMovieName().c_str()
                , _name.c_str()
            );

            return false;
        }

        const MovieLayer * exist_layer;
        if( m_resourceMovie->hasMovieLayer( _name, &exist_layer ) == true )
        {
            return true;
        }

        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            if( layer.isMovie() == false || layer.isSubMovie() == true )
            {
                continue;
            }

            const MoviePtr & movie = this->getLayerMovie_( layer );

            if( movie->hasMovieLayer( _name ) == false )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::getMovieScissor( const ConstString & _name, RenderScissorPtr * const _scissor ) const
    {
        for( const RenderScissorPtr & scissor : m_scissors )
        {
            const ConstString & name = scissor->getName();

            if( name != _name )
            {
                continue;
            }

            *_scissor = scissor;

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::hasMovieScissor( const ConstString & _name )
    {
        for( const RenderScissorPtr & scissor : m_scissors )
        {
            const ConstString & name = scissor->getName();

            if( name != _name )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::setEnableMovieLayers( const ConstString & _name, bool _enable )
    {
        if( m_resourceMovie == nullptr )
        {
            LOGGER_ERROR( "'%s' resource '%s' invalid get layer '%s' not compile"
                , this->getName().c_str()
                , this->getResourceMovieName().c_str()
                , _name.c_str()
            );

            return;
        }

        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            if( layer.name != _name )
            {
                continue;
            }

            this->setEnableLayer_( layer, _enable );
        }

        for( const MovieLayer & layer : layers )
        {
            if( layer.isMovie() == false || layer.isSubMovie() == true )
            {
                continue;
            }

            const MoviePtr & movie = this->getLayerMovie_( layer );

            movie->setEnableMovieLayers( _name, _enable );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::setEnableMovieLayer( const ConstString & _name, bool _enable )
    {
        if( m_resourceMovie == nullptr )
        {
            LOGGER_ERROR( "'%s' resource '%s' invalid get layer '%s' not compile"
                , this->getName().c_str()
                , this->getResourceMovieName().c_str()
                , _name.c_str()
            );

            return false;
        }

        const MovieLayer * exist_layer;
        if( m_resourceMovie->hasMovieLayer( _name, &exist_layer ) == true )
        {
            this->setEnableLayer_( *exist_layer, _enable );

            return true;
        }

        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            if( layer.isMovie() == false || layer.isSubMovie() == true )
            {
                continue;
            }

            const MoviePtr & movie = this->getLayerMovie_( layer );

            if( movie->setEnableMovieLayer( _name, _enable ) == false )
            {
                continue;
            }

            return true;
        }

        LOGGER_ERROR( "'%s' resource '%s' not found layer '%s'"
            , this->getName().c_str()
            , this->getResourceMovieName().c_str()
            , _name.c_str()
        );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::getEnableMovieLayer( const ConstString & _name, bool * const _enable ) const
    {
        if( m_resourceMovie == nullptr )
        {
            LOGGER_ERROR( "'%s' resource '%s' invalid get layer '%s' not compile"
                , this->getName().c_str()
                , this->getResourceMovieName().c_str()
                , _name.c_str()
            );

            return false;
        }

        const MovieLayer * exist_layer;
        if( m_resourceMovie->hasMovieLayer( _name, &exist_layer ) == true )
        {
            *_enable = this->getEnableLayer_( *exist_layer );

            return true;
        }

        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            if( layer.isMovie() == false || layer.isSubMovie() == true )
            {
                continue;
            }

            const MoviePtr & movie = this->getLayerMovie_( layer );

            if( movie->getEnableMovieLayer( _name, _enable ) == false )
            {
                continue;
            }

            return true;
        }

        LOGGER_ERROR( "'%s' resource '%s' not found layer '%s'"
            , this->getName().c_str()
            , this->getResourceMovieName().c_str()
            , _name.c_str()
        );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::getMovieLayer( const ConstString & _name, const MovieLayer ** const _layer, MoviePtr * const _movie ) const
    {
        if( m_resourceMovie == nullptr )
        {
            LOGGER_ERROR( "'%s' resource '%s' invalid get layer '%s' not compile"
                , this->getName().c_str()
                , this->getResourceMovieName().c_str()
                , _name.c_str()
            );

            return false;
        }

        const MovieLayer * exist_layer;
        if( m_resourceMovie->hasMovieLayer( _name, &exist_layer ) == true )
        {
            *_layer = exist_layer;
            *_movie = this;

            return true;
        }

        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            if( layer.isMovie() == false || layer.isSubMovie() == true )
            {
                continue;
            }

            const MoviePtr & movie = this->getLayerMovie_( layer );

            if( movie->getMovieLayer( _name, _layer, _movie ) == false )
            {
                continue;
            }

            return true;
        }

        LOGGER_ERROR( "'%s' resource '%s' not found layer '%s'"
            , this->getName().c_str()
            , this->getResourceMovieName().c_str()
            , _name.c_str()
        );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createLayers_()
    {
        uint32_t maxLayerIndex = m_resourceMovie->getMaxLayerIndex();

        m_nodies.resize( maxLayerIndex );

        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            if( layer.type == STRINGIZE_STRING_LOCAL( "MovieSlot" ) )
            {
                if( this->createMovieSlot_( layer ) == false )
                {
                    return false;
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieSceneEffect" ) )
            {
                if( this->createMovieSceneEffect_( layer ) == false )
                {
                    return false;
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieText" ) )
            {
                if( this->createMovieText_( layer ) == false )
                {
                    return false;
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieTextCenter" ) )
            {
                if( this->createMovieTextCenter_( layer ) == false )
                {
                    return false;
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieSprite" ) )
            {
                if( this->createMovieExtraSprite_( layer ) == false )
                {
                    return false;
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieNullObject" ) )
            {
                if( this->createMovieNullObject_( layer ) == false )
                {
                    return false;
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "Image" ) )
            {
                if( layer.shape == false )
                {
                    if( this->createMovieImage_( layer ) == false )
                    {
                        return false;
                    }
                }
                else
                {
                    if( this->createMovieMesh2D_( layer ) == false )
                    {
                        return false;
                    }
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "SolidSprite" ) )
            {
                if( layer.shape == false )
                {
                    if( this->createMovieImageSolid_( layer ) == false )
                    {
                        return false;
                    }
                }
                else
                {
                    if( this->createMovieMesh2DSolid_( layer ) == false )
                    {
                        return false;
                    }
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieSocketImage" ) )
            {
                if( this->createMovieSocketImage_( layer ) == false )
                {
                    return false;
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieSocketShape" ) )
            {
                if( this->createMovieSocketShape_( layer ) == false )
                {
                    return false;
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "Animation" ) )
            {
                if( this->createMovieAnimation_( layer ) == false )
                {
                    return false;
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "Movie" ) )
            {
                if( this->createMovieMovie_( layer ) == false )
                {
                    return false;
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "SubMovie" ) )
            {
                if( this->createMovieSubMovie_( layer ) == false )
                {
                    return false;
                }
            }
#ifdef MENGINE_USE_SCRIPT_SERVICE
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieInternalObject" ) )
            {
                if( this->createMovieInternalObject_( layer ) == false )
                {
                    return false;
                }
            }
#endif
            else if( layer.type == STRINGIZE_STRING_LOCAL( "Video" ) )
            {
                if( this->createMovieVideo_( layer ) == false )
                {
                    return false;
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "Sound" ) )
            {
                if( this->createMovieSound_( layer ) == false )
                {
                    return false;
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "SoundId" ) )
            {
                if( this->createMovieSoundId_( layer ) == false )
                {
                    return false;
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ) )
            {
                if( this->createMovieParticleEmitter2_( layer ) == false )
                {
                    return false;
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieParticle" ) )
            {
                //Empty
            }
#ifdef MENGINE_USE_SCRIPT_SERVICE
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieEvent" ) )
            {
                if( this->createMovieEvent_( layer ) == false )
                {
                    return false;
                }
            }
#endif
            else
            {
                LOGGER_ERROR( "'%s' resource '%s' can't create layer_node '%s' type '%s'"
                    , this->getName().c_str()
                    , this->getResourceMovieName().c_str()
                    , layer.source.c_str()
                    , layer.type.c_str()
                );

                return false;
            }
        }

        for( VectorNodies::reverse_iterator
            it = m_nodies.rbegin(),
            it_end = m_nodies.rend();
            it != it_end;
            ++it )
        {
            const Nodies & ns = *it;

            if( ns.node == nullptr )
            {
                continue;
            }

            if( ns.child == false )
            {
                continue;
            }

            this->addChild( ns.node );
        }

        this->createCamera3D_();

        if( m_resourceMovie->hasOffsetPoint() == false )
        {
            if( m_resourceMovie->hasAnchorPoint() == true )
            {
                const mt::vec3f & anchorPoint = m_resourceMovie->getAnchorPoint();

                this->setLocalOrigin( anchorPoint );
            }
        }
        else
        {
            if( m_resourceMovie->hasAnchorPoint() == true )
            {
                const mt::vec3f & offsetPoint = m_resourceMovie->getOffsetPoint();

                const mt::vec3f & anchorPoint = m_resourceMovie->getAnchorPoint();

                this->setLocalOrigin( offsetPoint );

                mt::vec3f offset = offsetPoint - anchorPoint;

                this->setLocalPosition( offset );
            }
            else
            {
                const mt::vec3f & offsetPoint = m_resourceMovie->getOffsetPoint();

                this->setLocalPosition( offsetPoint );
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::compileLayers_()
    {
        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            if( layer.type == STRINGIZE_STRING_LOCAL( "MovieText" ) )
            {
                if( this->compileMovieText_( layer ) == false )
                {
                    return false;
                }
            }
            else if( layer.type == STRINGIZE_STRING_LOCAL( "MovieTextCenter" ) )
            {
                if( this->compileMovieText_( layer ) == false )
                {
                    return false;
                }
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::destroyLayers_()
    {
        m_nodies.clear();

        this->destroyCamera3D_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieSlot_( const MovieLayer & _layer )
    {
        MovieSlotPtr layer_slot = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieSlot" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_slot );

        layer_slot->setMovieName( m_name );

        if( this->addMovieNode_( _layer, layer_slot, nullptr, nullptr, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieSceneEffect_( const MovieLayer & _layer )
    {
        MovieSceneEffectPtr sceneeffect_slot = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieSceneEffect" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( sceneeffect_slot );

        if( this->addMovieNode_( _layer, sceneeffect_slot, nullptr, nullptr, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieNullObject_( const MovieLayer & _layer )
    {
        NodePtr layer_slot = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Interender" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_slot );

        if( this->addMovieNode_( _layer, layer_slot, nullptr, nullptr, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieImage_( const MovieLayer & _layer )
    {
        const ConstString & groupName = m_resourceMovie->getGroupName();

        ResourceImagePtr resourceImage = RESOURCE_SERVICE()
            ->getResourceReference( groupName, _layer.source );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceImage );

        SurfaceImagePtr surface = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceImage" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( surface );

        surface->setResourceImage( resourceImage );

        if( this->setupBlendingMode_( _layer, surface ) == false )
        {
            return false;
        }

        ShapeQuadFixedPtr layer_sprite = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_sprite );

        layer_sprite->setSurface( surface );

        if( this->addMovieNode_( _layer, layer_sprite, nullptr, nullptr, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieMesh2D_( const MovieLayer & _layer )
    {
        const ConstString & groupName = m_resourceMovie->getGroupName();

        ResourceImagePtr resourceImage = RESOURCE_SERVICE()
            ->getResourceReference( groupName, _layer.source );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceImage );

        MovieMesh2DPtr layer_mesh = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieMesh2D" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_mesh );

        layer_mesh->setResourceImage( resourceImage );

        if( this->setupBlendingMode_( _layer, layer_mesh ) == false )
        {
            return false;
        }

        if( this->addMovieNode_( _layer, layer_mesh, nullptr, nullptr, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieImageSolid_( const MovieLayer & _layer )
    {
        const ConstString & groupName = m_resourceMovie->getGroupName();

        ResourceImageSolidPtr resource = RESOURCE_SERVICE()
            ->getResourceReference( groupName, _layer.source );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, "'%s' resource '%s' can't compile sprite '%s' imageSolid resource = NULL"
            , this->getName().c_str()
            , this->getResourceMovieName().c_str()
            , _layer.name.c_str()
        );

        SurfaceImagePtr surface = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceImage" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( surface );

        surface->setResourceImage( resource );

        if( this->setupBlendingMode_( _layer, surface ) == false )
        {
            return false;
        }

        ShapeQuadFixedPtr layer_sprite = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_sprite );

        layer_sprite->setSurface( surface );

        if( this->addMovieNode_( _layer, layer_sprite, nullptr, nullptr, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieMesh2DSolid_( const MovieLayer & _layer )
    {
        const ConstString & groupName = m_resourceMovie->getGroupName();

        ResourceImageSolidPtr resourceImage = RESOURCE_SERVICE()
            ->getResourceReference( groupName, _layer.source );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceImage );

        MovieMesh2DPtr layer_mesh = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieMesh2D" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_mesh );

        layer_mesh->setResourceImage( resourceImage );

        if( this->setupBlendingMode_( _layer, layer_mesh ) == false )
        {
            return false;
        }

        if( this->addMovieNode_( _layer, layer_mesh, nullptr, nullptr, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieSocketImage_( const MovieLayer & _layer )
    {
        const ConstString & groupName = m_resourceMovie->getGroupName();

        ResourceHITPtr resourceHIT = RESOURCE_SERVICE()
            ->getResourceReference( groupName, _layer.source );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceHIT );

        HotSpotImagePtr layer_hotspotimage = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "HotSpotImage" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_hotspotimage );

        layer_hotspotimage->setResourceTestPick( resourceHIT );

        if( this->addMovieNode_( _layer, layer_hotspotimage, nullptr, nullptr, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieSocketShape_( const MovieLayer & _layer )
    {
        HotSpotResourceShapePtr layer_hotspotresourceshape = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "HotSpotResourceShape" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_hotspotresourceshape );

        const ConstString & groupName = m_resourceMovie->getGroupName();

        ResourceShapePtr resourceShape = RESOURCE_SERVICE()
            ->getResourceReference( groupName, _layer.source );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceShape );

        layer_hotspotresourceshape->setResourceShape( resourceShape );

        if( this->addMovieNode_( _layer, layer_hotspotresourceshape, nullptr, nullptr, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieAnimation_( const MovieLayer & _layer )
    {
        const ConstString & groupName = m_resourceMovie->getGroupName();

        ResourceImageSequencePtr resourceImageSequence = RESOURCE_SERVICE()
            ->getResourceReference( groupName, _layer.source );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceImageSequence );

        SurfaceImageSequencePtr surface = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceImageSequence" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( surface );

        surface->setName( _layer.name );
        surface->setResourceImageSequence( resourceImageSequence );

        AnimationInterface * animation = surface->getAnimation();

        animation->setIntervalStart( _layer.startInterval );
        animation->setPlayCount( _layer.playCount );
        animation->setStretch( _layer.stretch );
        animation->setLoop( _layer.loop );

        if( this->setupBlendingMode_( _layer, surface ) == false )
        {
            return false;
        }

        ShapeQuadFixedPtr layer_animation = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_animation );

        layer_animation->setName( _layer.name );
        layer_animation->setSurface( surface );

        if( this->addMovieNode_( _layer, layer_animation, surface, nullptr, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieMovie_( const MovieLayer & _layer )
    {
        MoviePtr layer_movie = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_movie );

        const ConstString & groupName = m_resourceMovie->getGroupName();

        ResourceMoviePtr resourceMovie = RESOURCE_SERVICE()
            ->getResourceReference( groupName, _layer.source );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceMovie );

        layer_movie->setResourceMovie( resourceMovie );

        layer_movie->setIntervalStart( _layer.startInterval );

        layer_movie->setPlayCount( _layer.playCount );
        layer_movie->setStretch( _layer.stretch );
        layer_movie->setLoop( _layer.loop );

        layer_movie->setParentMovie( true );

        if( this->addMovieNode_( _layer, layer_movie, layer_movie, nullptr, layer_movie ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieSubMovie_( const MovieLayer & _layer )
    {
        MoviePtr layer_movie = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_movie );

        const ConstString & groupName = m_resourceMovie->getGroupName();

        ResourceMoviePtr resourceMovie = RESOURCE_SERVICE()
            ->getResourceReference( groupName, _layer.source );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceMovie );

        layer_movie->setResourceMovie( resourceMovie );

        layer_movie->setIntervalStart( _layer.startInterval );

        layer_movie->setPlayCount( _layer.playCount );
        layer_movie->setStretch( _layer.stretch );
        layer_movie->setLoop( _layer.loop );

        layer_movie->setParentMovie( true );

        if( this->addMovieNode_( _layer, layer_movie, layer_movie, nullptr, layer_movie ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_USE_SCRIPT_SERVICE
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieInternalObject_( const MovieLayer & _layer )
    {
        MovieInternalObjectPtr movie_internal = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieInternalObject" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( movie_internal );

        const ConstString & groupName = m_resourceMovie->getGroupName();

        ResourceInternalObjectPtr resourceInternalObject = RESOURCE_SERVICE()
            ->getResourceReference( groupName, _layer.source );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceInternalObject );

        movie_internal->setMovie( this );
        movie_internal->setResourceInternalObject( resourceInternalObject );

        if( this->addMovieNode_( _layer, movie_internal, nullptr, nullptr, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieVideo_( const MovieLayer & _layer )
    {
        const ConstString & groupName = m_resourceMovie->getGroupName();

        const ResourcePtr & resourceVideo = RESOURCE_SERVICE()
            ->getResourceReference( groupName, _layer.source );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceVideo );

        SurfacePtr surface = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceVideo" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( surface );

        UnknownVideoSurfaceInterface * unknownVideoSurface = surface->getUnknown();

        unknownVideoSurface->setResourceVideo( resourceVideo );

        AnimationInterface * surface_animation = surface->getAnimation();

        surface_animation->setIntervalStart( _layer.startInterval );
        surface_animation->setPlayCount( _layer.playCount );
        surface_animation->setStretch( _layer.stretch );
        surface_animation->setLoop( _layer.loop );

        if( this->setupBlendingMode_( _layer, surface ) == false )
        {
            return false;
        }

        ShapeQuadFixedPtr layer_video = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_video );

        layer_video->setSurface( surface );

        if( this->addMovieNode_( _layer, layer_video, surface, nullptr, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieSound_( const MovieLayer & _layer )
    {
        SoundEmitterPtr layer_sound = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "SoundEmitter" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_sound );

        const ConstString & groupName = m_resourceMovie->getGroupName();

        ResourceSoundPtr resourceSound = RESOURCE_SERVICE()
            ->getResourceReference( groupName, _layer.source );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceSound );

        SurfaceSoundPtr surfaceSound = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceSound" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( surfaceSound );

        surfaceSound->setResourceSound( resourceSound );

        AnimationInterface * animation = surfaceSound->getAnimation();

        animation->setIntervalStart( _layer.startInterval );
        animation->setPlayCount( _layer.playCount );
        animation->setStretch( _layer.stretch );
        animation->setLoop( _layer.loop );

        layer_sound->setSurfaceSound( surfaceSound );

        if( this->addMovieNode_( _layer, layer_sound, surfaceSound, surfaceSound, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieSoundId_( const MovieLayer & _layer )
    {
        SoundEmitterPtr layer_sound = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "SoundEmitter" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_sound );

        const ConstString & groupName = m_resourceMovie->getGroupName();

        ResourceSoundPtr resourceSound = RESOURCE_SERVICE()
            ->getResourceReference( groupName, _layer.name );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceSound );

        SurfaceSoundPtr surfaceSound = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceSound" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( surfaceSound );

        surfaceSound->setResourceSound( resourceSound );

        AnimationInterface * animation = surfaceSound->getAnimation();

        animation->setIntervalStart( _layer.startInterval );
        animation->setPlayCount( 1 );
        animation->setStretch( _layer.stretch );
        animation->setLoop( _layer.loop );

        layer_sound->setSurfaceSound( surfaceSound );

        if( this->addMovieNode_( _layer, layer_sound, surfaceSound, surfaceSound, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieText_( const MovieLayer & _layer )
    {
        TextFieldPtr layer_text = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "TextField" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_text );

        layer_text->setTextId( _layer.name );

        if( _layer.hasParam( MOVIE_LAYER_PARAM_TEXT_HORIZONTAL_CENTER ) == true )
        {
            layer_text->setHorizontalCenterAlign();
        }

        if( _layer.hasParam( MOVIE_LAYER_PARAM_TEXT_VERTICAL_CENTER ) == true )
        {
            layer_text->setVerticalCenterAlign();
        }

        if( _layer.hasParam( MOVIE_LAYER_PARAM_PARTICLE_TRANSLATE ) == true )
        {
            layer_text->setPixelsnap( false );
        }

        if( this->addMovieNode_( _layer, layer_text, nullptr, nullptr, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieTextCenter_( const MovieLayer & _layer )
    {
        TextFieldPtr layer_text = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "TextField" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_text );

        layer_text->setTextId( _layer.name ); //Name = TextID
        layer_text->setHorizontalCenterAlign();

        if( _layer.hasParam( MOVIE_LAYER_PARAM_PARTICLE_TRANSLATE ) == true )
        {
            layer_text->setPixelsnap( false );
        }

        if( this->addMovieNode_( _layer, layer_text, nullptr, nullptr, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieExtraSprite_( const MovieLayer & _layer )
    {
        const ConstString & groupName = m_resourceMovie->getGroupName();

        ResourceImagePtr resourceImage = RESOURCE_SERVICE()
            ->getResourceReference( groupName, _layer.name );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, "movie '%s' resource '%s' layer '%s' invalid get resource for image '%s'"
            , this->getName().c_str()
            , this->getResourceMovieName().c_str()
            , _layer.name.c_str()
            , _layer.name.c_str()
        );

        SurfaceImagePtr surface = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceImage" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( surface );

        surface->setResourceImage( resourceImage );

        if( this->setupBlendingMode_( _layer, surface ) == false )
        {
            return false;
        }

        ShapeQuadFixedPtr layer_sprite = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_sprite, "movie '%s' resource '%s' layer '%s' invalid create 'Sprite'"
            , this->getName().c_str()
            , this->getResourceMovieName().c_str()
            , _layer.name.c_str()
        );

        layer_sprite->setSurface( surface );

        if( this->addMovieNode_( _layer, layer_sprite, nullptr, nullptr, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::compileMovieText_( const MovieLayer & _layer )
    {
        if( _layer.polygon == false )
        {
            return true;
        }

        const NodePtr & node = this->getLayerNode_( _layer );

        if( node == nullptr )
        {
            return false;
        }

#ifdef MENGINE_DEBUG
        if( TextFieldPtr::dynamic_from( node ) == nullptr )
        {
            LOGGER_ERROR( "movie '%s' resource '%s' layer '%s' must be 'TextField' but node is '%s' type '%s'"
                , this->getName().c_str()
                , this->getResourceMovieName().c_str()
                , _layer.name.c_str()
                , node->getName().c_str()
                , node->getType().c_str()
            );

            return false;
        }
#endif

        TextFieldPtr layer_text = TextFieldPtr::from( node );

        const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

        uint8_t vertexCount;
        const mt::vec2f * polygon;
        if( framePack->getLayerPolygon( _layer.index, &polygon, &vertexCount ) == false )
        {
            return false;
        }

        if( vertexCount != 2 )
        {
            return false;
        }

        const mt::vec2f & begin = polygon[0];
        const mt::vec2f & end = polygon[1];

        float length = mt::length_v2_v2( begin, end );

        layer_text->setMaxLength( length );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_USE_SCRIPT_SERVICE
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieEvent_( const MovieLayer & _layer )
    {
        MovieEventPtr layer_event = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieEvent" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_event );

        layer_event->setResourceMovie( m_resourceMovie );

        if( this->addMovieNode_( _layer, layer_event, nullptr, nullptr, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    bool Movie::createMovieParticleEmitter2_( const MovieLayer & _layer )
    {
        NodePtr layer_particles = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( layer_particles );

        UnknownAstralaxEmitterInterface * unknownParticleEmitter2 = layer_particles->getUnknown();

        AnimationInterface * animationParticleEmitter2 = layer_particles->getAnimation();

        const ConstString & groupName = m_resourceMovie->getGroupName();

        const ResourcePtr & resourceParticle = RESOURCE_SERVICE()
            ->getResourceReference( groupName, _layer.source );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceParticle );

        unknownParticleEmitter2->setResourceAstralax( resourceParticle );

        animationParticleEmitter2->setIntervalStart( _layer.startInterval );
        animationParticleEmitter2->setPlayCount( _layer.playCount );
        animationParticleEmitter2->setStretch( _layer.stretch );
        animationParticleEmitter2->setLoop( _layer.loop );

        unknownParticleEmitter2->setEmitterPositionProviderOriginOffset( -mt::vec3f( 1024.f, 1024.f, 0.f ) );

        if( _layer.hasParam( MOVIE_LAYER_PARAM_PARTICLE_TRANSLATE ) == true )
        {
            unknownParticleEmitter2->setEmitterPositionRelative( true );
            unknownParticleEmitter2->setEmitterCameraRelative( false );
            unknownParticleEmitter2->setEmitterTranslateWithParticle( false );
        }
        else
        {
            unknownParticleEmitter2->setEmitterPositionRelative( false );
            unknownParticleEmitter2->setEmitterCameraRelative( false );
            unknownParticleEmitter2->setEmitterTranslateWithParticle( true );
        }

        if( this->addMovieNode_( _layer, layer_particles, layer_particles, nullptr, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceMovie, "movie '%s' can't setup resource"
            , this->getName().c_str()
        );

        if( m_resourceMovie->compile() == false )
        {
            LOGGER_ERROR( "movie '%s' resource '%s' not compile"
                , this->getName().c_str()
                , m_resourceMovie->getName().c_str()
            );

            return false;
        }

        if( this->setupSceneEffect_() == false )
        {
            LOGGER_ERROR( "movie '%s' resource '%s' can't setup scene effect"
                , this->getName().c_str()
                , m_resourceMovie->getName().c_str()
            );

            return false;
        }

        if( this->setupLayersParent_() == false )
        {
            LOGGER_ERROR( "movie '%s' resource '%s' can't setup layer parents"
                , this->getName().c_str()
                , m_resourceMovie->getName().c_str()
            );

            return false;
        }

        if( this->compileLayers_() == false )
        {
            LOGGER_ERROR( "movie '%s' resource '%s' can't compile layers"
                , this->getName().c_str()
                , m_resourceMovie->getName().c_str()
            );

            return false;
        }

        bool loop = this->isLoop();
        this->updateLoop_( loop );

        float animationSpeedFactor = this->getAnimationSpeedFactor();
        this->updateAnimationSpeedFactor_( animationSpeedFactor );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::_release()
    {
        this->removeLayersParent_();

        m_resourceMovie->release();
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::updateCamera_()
    {
        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            const NodePtr & node = this->getLayerNode_( layer );

            if( node == nullptr )
            {
                continue;
            }

            if( layer.isThreeD() == true )
            {
                RenderInterface * render = node->getRender();
                render->setRenderCamera( m_renderCameraProjection );
                render->setRenderViewport( m_renderViewport );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::setupSceneEffect_()
    {
        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        Node * parent = Helper::findParentNodeT<Layer *>( this );

        for( const MovieLayer & l : layers )
        {
            if( l.isSceneEffect() == false )
            {
                continue;
            }

            const NodePtr & node = this->getLayerNode_( l );

#ifdef MENGINE_DEBUG
            if( MovieSceneEffectPtr::dynamic_from( node ) == nullptr )
            {
                LOGGER_ERROR( "movie '%s' resource '%s' layer '%s' must be 'MovieSceneEffect' but node is '%s' type '%s'"
                    , this->getName().c_str()
                    , this->getResourceMovieName().c_str()
                    , l.name.c_str()
                    , node->getName().c_str()
                    , node->getType().c_str()
                );

                return false;
            }
#endif

            MovieSceneEffectPtr sceneEffect = MovieSceneEffectPtr::from( node );

            if( sceneEffect->setPropagateNode( parent ) == false )
            {
                LOGGER_ERROR( "movie '%s' resource '%s' layer '%s' invalid set propagate node '%s' type '%s'!"
                    , this->getName().c_str()
                    , this->getResourceMovieName().c_str()
                    , l.name.c_str()
                    , node->getName().c_str()
                    , node->getType().c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::enableSceneEffect_( bool _value )
    {
        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & l : layers )
        {
            if( l.isSceneEffect() == false )
            {
                continue;
            }

            const NodePtr & node = this->getLayerNode_( l );

            MovieSceneEffectPtr sceneEffect = MovieSceneEffectPtr::from( node );

            sceneEffect->enablePropagate( _value );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::setupLayersParent_()
    {
        //float frameDuration = m_resourceMovie->getFrameDuration();

        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            const NodePtr & node = this->getLayerNode_( layer );

            if( node == nullptr )
            {
                continue;
            }

            if( this->updateFrameNode_( layer, node, 0, false, true ) == false )
            {
                return false;
            }

            if( layer.parent != 0 && layer.parent != movie_layer_parent_none )
            {
                const NodePtr & node_parent = this->getLayerParent_( layer );

                MENGINE_ASSERTION_MEMORY_PANIC( node_parent, "movie '%s' resource '%s' invalid parent node [%u]"
                    , this->getName().c_str()
                    , this->getResourceMovieName().c_str()
                    , layer.parent
                );

                TransformationInterface * transformation = node->getTransformation();

                transformation->setRelationTransformation( node_parent->getTransformation() );
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::removeLayersParent_()
    {
        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            const NodePtr & node = this->getLayerNode_( layer );

            if( node == nullptr )
            {
                continue;
            }

            if( layer.parent == 0 || layer.parent == movie_layer_parent_none )
            {
                continue;
            }

            Node * parent = node->getParent();

            TransformationInterface * transformation = node->getTransformation();

            transformation->setRelationTransformation( parent->getTransformation() );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::stopAnimation_()
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "movie '%s' is not compile"
                , this->getName().c_str()
            );

            return;
        }

        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            const NodePtr & node = this->getLayerNode_( layer );

            if( node == nullptr )
            {
                continue;
            }

            if( layer.switcher == true )
            {
                this->setVisibleLayer_( layer, false );
            }

            if( layer.isAnimation() == false )
            {
                continue;
            }

            if( layer.isSubMovie() == true )
            {
                continue;
            }

            const AnimationInterfacePtr & animation = this->getLayerAnimation_( layer );

            if( layer.timeRemap == false )
            {
                if( animation->isPlay() == true )
                {
                    animation->stop();
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::pauseAnimation_()
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' is not compile"
                , this->getName().c_str()
            );

            return;
        }

        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            const NodePtr & node = this->getLayerNode_( layer );

            if( node == nullptr )
            {
                continue;
            }

            if( layer.isAnimation() == false )
            {
                continue;
            }

            if( layer.isSubMovie() == true )
            {
                continue;
            }

            if( layer.timeRemap == false )
            {
                const AnimationInterfacePtr & animation = this->getLayerAnimation_( layer );

                animation->pause();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::resumeAnimation_()
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' is not compile"
                , this->getName().c_str()
            );

            return;
        }

        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            const NodePtr & node = this->getLayerNode_( layer );

            if( node == nullptr )
            {
                continue;
            }

            if( layer.isAnimation() == false )
            {
                continue;
            }

            if( layer.isSubMovie() == true )
            {
                continue;
            }

            if( layer.timeRemap == false )
            {
                const AnimationInterfacePtr & animation = this->getLayerAnimation_( layer );

                float time = TIMELINE_SERVICE()
                    ->getTotalTime();

                animation->resume( time );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::updateStartInterval_()
    {
        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            if( layer.isAnimation() == false )
            {
                continue;
            }

            if( layer.isSubMovie() == true )
            {
                continue;
            }

            const NodePtr & node = this->getLayerNode_( layer );

            if( node == nullptr )
            {
                continue;
            }

            const AnimationInterfacePtr & animation = this->getLayerAnimation_( layer );

            animation->setTime( 0.f );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::_activate()
    {
        if( Node::_activate() == false )
        {
            return false;
        }

        this->updateCamera_();
        this->enableSceneEffect_( true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::_deactivate()
    {
        this->stopAnimation_();
        this->enableSceneEffect_( false );

        Node::_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::_afterActivate()
    {
        if( Node::_afterActivate() == false )
        {
            return false;
        }

        bool autoPlay = this->isAutoPlay();

        if( autoPlay == true )
        {
            float time = TIMELINE_SERVICE()
                ->getTotalTime();

            if( this->play( time ) == 0 )
            {
                LOGGER_ERROR( "'%s' resource '%s' auto play return 0"
                    , this->getName().c_str()
                    , this->m_resourceMovie->getName().c_str()
                );

                return false;
            }
        }
        else
        {
            this->updateTime_();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::_destroy()
    {
        for( Nodies & ns : m_nodies )
        {
            if( ns.node == nullptr )
            {
                continue;
            }

            if( ns.child == true )
            {
                continue;
            }

            ns.node = nullptr;
            ns.animation = nullptr;
            ns.soundable = nullptr;
            ns.movie = nullptr;
        }

        Node::_destroy();
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::update( const UpdateContext * _context )
    {
        if( this->isPlay() == false )
        {
            return;
        }

        float totalTime = this->calcTotalTime( _context );

        m_frameTime += totalTime;

        this->updateForward_( _context->current );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        MENGINE_UNUSED( _renderPipeline );
        MENGINE_UNUSED( _context );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::updateForward_( float _time )
    {
        if( m_interruptEnd == true )
        {
            for( const Nodies & node : m_nodies )
            {
                if( node.animation == nullptr )
                {
                    continue;
                }

                if( node.animation->isInterrupt() == true )
                {
                    return;
                }
            }

            m_interruptEnd = false;

            this->end();

            return;
        }

        float frameDuration = m_resourceMovie->getFrameDuration();
        uint32_t frameCount = m_resourceMovie->getFrameCount();

        const mt::vec2f & loopSegment = m_resourceMovie->getLoopSegment();

        float intervalStart = this->getIntervalStart();
        float intervalBegin = this->getIntervalBegin();
        float intervalEnd = this->getIntervalEnd();

        if( intervalEnd > 0.f )
        {
            frameCount = (uint32_t)((intervalEnd - intervalBegin - intervalStart) / frameDuration + 0.5f);
        }
        else
        {
            if( loopSegment.y > 0.f )
            {
                frameCount = (uint32_t)(loopSegment.y / frameDuration + 0.5f);
            }
        }

        uint32_t lastFrame = m_currentFrame;

        while( m_frameTime >= frameDuration && frameDuration != 0.f )
        {
            m_frameTime -= frameDuration;

            ++m_currentFrame;

            if( m_currentFrame == frameCount - 1 )
            {
                this->updateForwardFrame_( _time, lastFrame, frameCount - 1 );

                bool loop = this->isLoop();
                bool interrupt = this->isInterrupt();

                if( (loop == false && --m_playIterator == 0) || interrupt == true )
                {
                    m_currentFrame = frameCount - 1;

                    if( interrupt == false )
                    {
                        this->end();
                    }
                    else
                    {
                        m_interruptEnd = true;
                    }

                    return;
                }
                else
                {
                    float beginTiming = m_frameTime + loopSegment.x;

                    this->setTime( beginTiming );

                    lastFrame = m_currentFrame;

                    this->updateAnimatablePlay_();
                }
            }
        }

        this->updateForwardFrame_( _time, lastFrame, m_currentFrame );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::updateForwardFrame_( float _time, uint32_t _beginFrame, uint32_t _endFrame )
    {
        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            const NodePtr & node = this->getLayerNode_( layer );

            if( node == nullptr )
            {
                continue;
            }

            if( layer.isNode() == true )
            {
                this->updateForwardFrameNode_( _time, _beginFrame, _endFrame, layer, node );
            }
            else if( layer.isExtra() == true )
            {
#ifdef MENGINE_DEBUG
                if( MovieNodeExtraPtr::dynamic_from( node ) == nullptr )
                {
                    LOGGER_ERROR( "'%s' layer '%s' must be 'MovieNodeExtra' but node is '%s' type '%s'"
                        , this->getName().c_str()
                        , layer.name.c_str()
                        , node->getName().c_str()
                        , node->getType().c_str()
                    );

                    continue;
                }
#endif

                MovieNodeExtraPtr extra = MovieNodeExtraPtr::from( node );

                extra->movieForwardUpdate( _time, _beginFrame, _endFrame, layer );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::updateForwardFrameNode_( float _time, uint32_t _beginFrame, uint32_t _endFrame, const MovieLayer & _layer, const NodePtr & _node )
    {
        MENGINE_UNUSED( _time );

        float layerIn = _layer.in;
        float layerOut = _layer.out;

        float frameDuration = m_resourceMovie->getFrameDuration();

        uint32_t indexIn = (uint32_t)((layerIn / frameDuration) + 0.5f);
        uint32_t indexOut = (uint32_t)((layerOut / frameDuration) + 0.5f);

        if( indexIn > _endFrame || indexOut < _beginFrame )
        {
            return;
        }

        if( this->isLastCicle() == false && _layer.loop == true )
        {
            uint32_t frameId = _endFrame - indexIn;

            this->updateFrameNode_( _layer, _node, frameId, true, false );

            if( _layer.isAnimation() == true && _layer.isSubMovie() == false )
            {
                if( _layer.timeRemap == false )
                {
                }
                else
                {
                    const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

                    float timing;
                    if( framePack->getLayerTimeRemap( _layer.index, _endFrame - indexIn, &timing ) == false )
                    {
                        return;
                    }

                    const AnimationInterfacePtr & animation = this->getLayerAnimation_( _layer );

                    animation->setTime( timing );
                }
            }
        }
        else
        {
            if( _beginFrame < indexIn && _endFrame >= indexIn && _endFrame < indexOut )
            {
                uint32_t frameId = _endFrame - indexIn;

                this->updateFrameNode_( _layer, _node, frameId, (_endFrame + 1) < indexOut, true );

                this->setVisibleLayer_( _layer, true );

                if( _layer.isAnimation() == true && _layer.isSubMovie() == false )
                {
                    const AnimationInterfacePtr & animation = this->getLayerAnimation_( _layer );

                    if( _layer.timeRemap == false )
                    {
                        if( animation->isPlay() == true )
                        {
                            animation->stop();
                        }

                        uint32_t frame = _endFrame - indexIn;
                        float timing = float( frame ) * frameDuration;

                        animation->setTime( timing );


                        float movieTiming = this->getTime();
                        animation->play( movieTiming );
                    }
                    else
                    {
                        const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

                        float timing;
                        if( framePack->getLayerTimeRemap( _layer.index, _endFrame - indexIn, &timing ) == false )
                        {
                            return;
                        }

                        animation->setTime( timing );
                    }
                }
            }
            else if( _endFrame >= indexOut && _beginFrame >= indexIn && _beginFrame < indexOut )
            {
                this->setVisibleLayer_( _layer, false );

                if( _layer.isAnimation() == true && _layer.isSubMovie() == false )
                {
                    const AnimationInterfacePtr & animation = this->getLayerAnimation_( _layer );

                    if( _layer.timeRemap == false )
                    {
                        if( animation->isPlay() == true && _layer.isUnstoppable() == false )
                        {
                            animation->stop();

                            uint32_t frame = indexOut - indexIn;
                            float timing = frame * frameDuration;
                            animation->setTime( timing );
                        }
                    }
                    else
                    {
                        const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

                        float timing;
                        if( framePack->getLayerTimeRemap( _layer.index, indexOut - indexIn, &timing ) == false )
                        {
                            return;
                        }

                        animation->setTime( timing );
                    }
                }
            }
            else if( _beginFrame >= indexIn && _endFrame >= indexIn && _endFrame < indexOut )
            {
                uint32_t frameId = _endFrame - indexIn;

                this->updateFrameNode_( _layer, _node, frameId, (_endFrame + 1) < indexOut, false );

                if( _layer.isAnimation() == true && _layer.isSubMovie() == false )
                {
                    const AnimationInterfacePtr & animation = this->getLayerAnimation_( _layer );

                    if( _layer.timeRemap == false )
                    {
                    }
                    else
                    {
                        const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

                        float timing;
                        if( framePack->getLayerTimeRemap( _layer.index, _endFrame - indexIn, &timing ) == false )
                        {
                            return;
                        }

                        animation->setTime( timing );
                    }
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::_setLocalHide( bool _value )
    {
        this->setHide( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::_setPersonalColor( const Color & _color )
    {
        this->setLocalColor( _color );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::_setPersonalAlpha( float _alpha )
    {
        this->setLocalAlpha( _alpha );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::updateTime_()
    {
        float frameDuration = m_resourceMovie->getFrameDuration();

        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            float layerIn = layer.in;
            float layerOut = layer.out;

            uint32_t indexIn = (uint32_t)((layerIn / frameDuration) + 0.5f);
            uint32_t indexOut = (uint32_t)((layerOut / frameDuration) + 0.5f);

            const NodePtr & node = this->getLayerNode_( layer );

            if( node == nullptr )
            {
                continue;
            }

            if( m_currentFrame >= indexIn && m_currentFrame < indexOut )
            {
                uint32_t frameId = m_currentFrame - indexIn;

                if( layer.switcher == true && m_currentFrame + 1 == indexOut )
                {
                    this->setVisibleLayer_( layer, false );
                }
                else
                {
                    this->updateFrameNode_( layer, node, frameId, (m_currentFrame + 1) < indexOut, true );

                    this->setVisibleLayer_( layer, true );
                }

                if( layer.isAnimation() == true && layer.isSubMovie() == false )
                {
                    const AnimationInterfacePtr & animation = this->getLayerAnimation_( layer );

                    if( layer.timeRemap == false )
                    {
                        float time = (m_currentFrame - indexIn) * frameDuration + m_frameTime;
                        animation->setTime( time );
                    }
                    else
                    {
                        const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

                        float time;
                        if( framePack->getLayerTimeRemap( layer.index, m_currentFrame - indexIn, &time ) == false )
                        {
                            continue;
                        }

                        animation->setTime( time );
                    }
                }
            }
            else
            {
                this->setVisibleLayer_( layer, false );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::updateAnimatablePlay_()
    {
        float frameDuration = m_resourceMovie->getFrameDuration();

        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            float layerIn = layer.in;
            float layerOut = layer.out;

            uint32_t indexIn = (uint32_t)((layerIn / frameDuration) + 0.5f);
            uint32_t indexOut = (uint32_t)((layerOut / frameDuration) + 0.5f);

            if( indexIn > m_currentFrame || indexOut < m_currentFrame )
            {
                continue;
            }

            const NodePtr & node = this->getLayerNode_( layer );

            if( node == nullptr )
            {
                continue;
            }

            if( m_currentFrame >= indexIn && m_currentFrame < indexOut )
            {
                uint32_t frameId = m_currentFrame - indexIn;

                if( layer.switcher == true && m_currentFrame + 1 == indexOut )
                {
                    this->setVisibleLayer_( layer, false );
                }
                else
                {
                    this->updateFrameNode_( layer, node, frameId, (m_currentFrame + 1) < indexOut, true );

                    this->setVisibleLayer_( layer, true );
                }

                if( layer.isAnimation() == true && layer.isSubMovie() == false )
                {
                    const AnimationInterfacePtr & animation = this->getLayerAnimation_( layer );

                    if( layer.timeRemap == false )
                    {
                        if( animation->isPlay() == true && animation->calcTotalLoop() == false )
                        {
                            animation->stop();
                        }

                        float timing = frameId * frameDuration;

                        animation->setTime( timing );

                        if( animation->isPlay() == false )
                        {
                            float playTime = TIMELINE_SERVICE()
                                ->getTotalTime();

                            animation->play( playTime );
                        }
                    }
                    else
                    {
                        const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

                        float timing;
                        if( framePack->getLayerTimeRemap( layer.index, frameId, &timing ) == false )
                        {
                            continue;
                        }

                        animation->setTime( timing );
                    }
                }
            }
            else
            {
                uint32_t frameId = indexOut - indexIn;

                this->setVisibleLayer_( layer, false );

                if( layer.isAnimation() == true && layer.isSubMovie() == false )
                {
                    const AnimationInterfacePtr & animation = this->getLayerAnimation_( layer );

                    if( layer.timeRemap == false )
                    {
                        if( animation->isPlay() == true )
                        {
                            animation->stop();

                            ///Test TestTestTestTest
                            float timing = frameId * frameDuration;
                            animation->setTime( timing );
                        }
                    }
                    else
                    {
                        const MovieFramePackInterfacePtr & framePack = m_resourceMovie->getFramePack();

                        float timing;
                        if( framePack->getLayerTimeRemap( layer.index, frameId, &timing ) == false )
                        {
                            continue;
                        }

                        animation->setTime( timing );
                    }
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::updateClipLastFrame_()
    {
        float frameDuration = m_resourceMovie->getFrameDuration();

        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            float layerIn = layer.in;
            float layerOut = layer.out;

            uint32_t indexIn = (uint32_t)((layerIn / frameDuration) + 0.5f);
            uint32_t indexOut = (uint32_t)((layerOut / frameDuration) + 0.5f);

            if( indexOut < m_currentFrame )
            {
                continue;
            }

            const NodePtr & node = this->getLayerNode_( layer );

            if( node == nullptr )
            {
                continue;
            }

            if( m_currentFrame > indexIn )
            {
                this->setVisibleLayer_( layer, false );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::createCamera3D_()
    {
        if( m_resourceMovie->hasCamera3D() == false )
        {
            return;
        }

        m_renderCameraProjection = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "RenderCameraProjection" ), MENGINE_DOCUMENT_FACTORABLE );

        const ConstString & name = this->getName();
        m_renderCameraProjection->setName( name );

        const MovieLayerCamera3D & camera3D = m_resourceMovie->getCamera3D();

        mt::vec3f cameraDirection = camera3D.cameraInterest - camera3D.cameraPosition;

        m_renderCameraProjection->setCameraPosition( camera3D.cameraPosition );
        m_renderCameraProjection->setCameraDirection( cameraDirection );
        m_renderCameraProjection->setCameraFOV( camera3D.cameraFOV );
        m_renderCameraProjection->setCameraAspect( camera3D.cameraAspect );

        //Viewport rp;
        //rp.begin.x = 0.f;
        //rp.begin.y = 0.f;

        //rp.end.x = camera3D.width;
        //rp.end.y = camera3D.height;

        //m_renderCameraProjection->setRenderport( rp );

        this->addChild( m_renderCameraProjection );

        m_renderViewport = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "RenderViewport" ), MENGINE_DOCUMENT_FACTORABLE );

        m_renderViewport->setName( name );

        Viewport vp;
        vp.begin.x = 0.f;
        vp.begin.y = 0.f;

        vp.end.x = camera3D.width;
        vp.end.y = camera3D.height;

        m_renderViewport->setViewport( vp );

        this->addChild( m_renderViewport );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::destroyCamera3D_()
    {
        m_renderCameraProjection = nullptr;
        m_renderViewport = nullptr;

        m_scissors.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::setEnableLayer_( const MovieLayer & _layer, bool _enable )
    {
        Nodies & nd = m_nodies[_layer.index - 1];

        nd.enable = _enable;

        if( nd.render != nullptr )
        {
            if( nd.visible == true && nd.enable == true )
            {
                nd.render->setLocalHide( false );
            }
            else
            {
                nd.render->setLocalHide( true );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::setVisibleLayer_( const MovieLayer & _layer, bool _visible )
    {
        if( _layer.isSubMovie() == true )
        {
            return;
        }

        Nodies & nd = m_nodies[_layer.index - 1];

        nd.visible = _visible;

        if( nd.render != nullptr )
        {
            if( nd.visible == true && nd.enable == true )
            {
                nd.render->setLocalHide( false );
            }
            else
            {
                nd.render->setLocalHide( true );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::setupBlendingMode_( const MovieLayer & _layer, const MaterialablePtr & _materiable )
    {
        if( _layer.blendingMode.empty() == true )
        {
            //Empty
        }
        else if( _layer.blendingMode == STRINGIZE_STRING_LOCAL( "BlendingModeScreen" ) )
        {
            _materiable->setBlendMode( EMB_SCREEN );
        }
        else if( _layer.blendingMode == STRINGIZE_STRING_LOCAL( "BlendingModeMultiply" ) )
        {
            _materiable->setBlendMode( EMB_MULTIPLY );
        }
        else if( _layer.blendingMode == STRINGIZE_STRING_LOCAL( "BlendingModeNormal" ) )
        {
            _materiable->setBlendMode( EMB_NORMAL );
        }
        else if( _layer.blendingMode == STRINGIZE_STRING_LOCAL( "BlendingModeAdd" ) )
        {
            _materiable->setBlendMode( EMB_ADD );
        }
        else
        {
            LOGGER_ERROR( "'%s' resource '%s' layer '%s' blending mode not supported '%s'"
                , this->getName().c_str()
                , this->getResourceMovieName().c_str()
                , _layer.name.c_str()
                , _layer.blendingMode.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::_setLoop( bool _value )
    {
        if( this->isCompile() == false )
        {
            return;
        }

        this->updateLoop_( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::updateLoop_( bool _value )
    {
        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            if( layer.isSubMovie() == true )
            {
                continue;
            }

            if( layer.isAnimation() == false )
            {
                continue;
            }

            if( layer.loop == false )
            {
                continue;
            }

            const AnimationInterfacePtr & animation = this->getLayerAnimation_( layer );

            animation->setLoop( _value );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::_setAnimationSpeedFactor( float _factor )
    {
        if( this->isCompile() == false )
        {
            return;
        }

        this->updateAnimationSpeedFactor_( _factor );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie::updateAnimationSpeedFactor_( float _factor )
    {
        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            if( layer.isSubMovie() == true )
            {
                continue;
            }

            if( layer.isAnimation() == false )
            {
                continue;
            }

            const AnimationInterfacePtr & animation = this->getLayerAnimation_( layer );

            animation->setAnimationSpeedFactor( _factor );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie::_interrupt( uint32_t _playId )
    {
        MENGINE_UNUSED( _playId );

        const VectorMovieLayers & layers = m_resourceMovie->getLayers();

        for( const MovieLayer & layer : layers )
        {
            if( layer.isSubMovie() == true )
            {
                continue;
            }

            if( layer.isAnimation() == false )
            {
                continue;
            }

            const AnimationInterfacePtr & animation = this->getLayerAnimation_( layer );

            if( animation->interrupt() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}

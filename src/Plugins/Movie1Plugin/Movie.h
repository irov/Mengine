#pragma once

#include "MovieKeyFrameInterface.h"

#include "Kernel/Node.h"
#include "Kernel/Soundable.h"

#include "Kernel/BaseEventation.h"
#include "Kernel/AnimationEventReceiver.h"
#include "Kernel/BaseAnimation.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/BaseRender.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/RenderScissor.h"
#include "Kernel/Materialable.h"

#include "ResourceMovie.h"
#include "MovieNodeExtra.h"
#include "MovieSceneEffect.h"
#include "MovieInternalObject.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Movie> MoviePtr;
    //////////////////////////////////////////////////////////////////////////
    class VisitorMovieNodeInterface
        : public Mixin
    {
    public:
        virtual void visitMovieNode( const MoviePtr & _movie, const NodePtr & _node ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<VisitorMovieNodeInterface> VisitorMovieNodeInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    enum EMovieEventFlag
    {
        EVENT_MOVIE_GET_INTERNAL = __EVENT_ANIMATION_LAST__,
        EVENT_MOVIE_ACTIVATE_INTERNAL,
        EVENT_MOVIE_DEACTIVATE_INTERNAL,
        __EVENT_MOVIE_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class MovieEventReceiverInterface
        : public AnimationEventReceiverInterface
    {
    public:
        virtual pybind::object onMovieGetInternal( const ConstString & _group, const ConstString & _name ) const = 0;
        virtual NodePtr onMovieActivateInternal( const pybind::object & _internal ) = 0;
        virtual void onMovieDeactivateInternal( const pybind::object & _internal ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( MovieEventReceiverInterface, EVENT_MOVIE_GET_INTERNAL );
    EVENTATION_TYPEID( MovieEventReceiverInterface, EVENT_MOVIE_ACTIVATE_INTERNAL );
    EVENTATION_TYPEID( MovieEventReceiverInterface, EVENT_MOVIE_DEACTIVATE_INTERNAL );
    //////////////////////////////////////////////////////////////////////////
    class Movie
        : public Node
        , public BaseEventation
        , public BaseAnimation
        , public BaseUpdation
        , public BaseRender
    {
        DECLARE_VISITABLE( Node );
        DECLARE_ANIMATABLE();
        DECLARE_UPDATABLE();
        DECLARE_EVENTABLE();
        DECLARE_RENDERABLE();

    public:
        Movie();

    public:
        void setResourceMovie( const ResourceMoviePtr & _resourceMovie );
        const ResourceMoviePtr & getResourceMovie() const;

    public:
        const ConstString & getResourceMovieName() const;

    public:
        void setParentMovie( bool _value );
        bool isParentMovie() const;

    public:
        bool visitMovieLayer( const ConstString & _type, const VisitorMovieNodeInterfacePtr & _visitor );

    public:
        bool getMovieNode( const ConstString & _name, const ConstString & _type, NodePtr * _node, MoviePtr * _movie );
        bool hasMovieNode( const ConstString & _name, const ConstString & _type, NodePtr * _node, MoviePtr * _movie );

    public:
        bool getMovieLayer( const ConstString & _name, const MovieLayer ** _layer, MoviePtr * _movie );
        bool hasMovieLayer( const ConstString & _name ) const;

    public:
        bool getMovieScissor( const ConstString & _name, RenderScissorPtr * _scissor );
        bool hasMovieScissor( const ConstString & _name );

    public:
        bool setEnableMovieLayer( const ConstString & _name, bool _enable );
        bool getEnableMovieLayer( const ConstString & _name, bool & _enable );

    public:
        void setEnableMovieLayers( const ConstString & _name, bool _enable );

    protected:
        void _setTime( float _time ) override;
        float _getTime() const override;
        float _getDuration() const override;

        void _setFirstFrame() override;
        void _setLastFrame() override;

        void _setLoop( bool _value ) override;
        void _setAnimationSpeedFactor( float _factor ) override;

    protected:
        void updateLoop_( bool _value );
        void updateAnimationSpeedFactor_( float _factor );

    protected:
        bool _play( uint32_t _playId, float _time ) override;
        bool _restart( uint32_t _playId, float _time ) override;
        void _pause( uint32_t _playId ) override;
        void _resume( uint32_t _playId, float _time ) override;
        bool _stop( uint32_t _playId ) override;
        void _end( uint32_t _playId ) override;
        bool _interrupt( uint32_t _playId ) override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        void _afterActivate() override;

    protected:
        void _destroy() override;

    protected:
        void update( const UpdateContext * _context ) override;
        void render( const RenderContext * _context ) const override;

    protected:
        void _setLocalHide( bool _value ) override;
        void _setPersonalColor( const Color & _color ) override;
        void _setPersonalAlpha( float _alpha ) override;

    protected:
        void updateForward_( float _time );

        void updateForwardFrame_( float _time, uint32_t _beginFrame, uint32_t _endFrame );
        void updateForwardFrameNode_( float _time, uint32_t _beginFrame, uint32_t _endFrame, const MovieLayer &, const NodePtr & _node );

    protected:
        bool updateFrameNode_( const MovieLayer & layer, const NodePtr & _node, uint32_t _frameId, bool _interpolate, bool _start );

    protected:
        bool setupSceneEffect_();
        void enableSceneEffect_( bool _value );

        bool setupLayersParent_();
        void removeLayersParent_();

        void updateCamera_();

    protected:
        void stopAnimation_();
        void pauseAnimation_();
        void resumeAnimation_();

    protected:
        bool createLayers_();
        bool compileLayers_();
        void destroyLayers_();

    protected:
        bool createMovieSlot_( const MovieLayer & _layer );
        bool createMovieSceneEffect_( const MovieLayer & _layer );
        bool createMovieNullObject_( const MovieLayer & _layer );
        bool createMovieImage_( const MovieLayer & _layer );
        bool createMovieMesh2D_( const MovieLayer & _layer );
        bool createMovieMesh2DSolid_( const MovieLayer & _layer );
        bool createMovieImageSolid_( const MovieLayer & _layer );
        bool createMovieSocketImage_( const MovieLayer & _layer );
        bool createMovieSocketShape_( const MovieLayer & _layer );
        bool createMovieAnimation_( const MovieLayer & _layer );
        bool createMovieMovie_( const MovieLayer & _layer );
        bool createMovieSubMovie_( const MovieLayer & _layer );
        bool createMovieInternalObject_( const MovieLayer & _layer );
        bool createMovieVideo_( const MovieLayer & _layer );
        bool createMovieSound_( const MovieLayer & _layer );
        bool createMovieSoundId_( const MovieLayer & _layer );
        bool createMovieText_( const MovieLayer & _layer );
        bool createMovieTextCenter_( const MovieLayer & _layer );
        bool createMovieExtraSprite_( const MovieLayer & _layer );
        bool createMovieEvent_( const MovieLayer & _layer );
        bool createMovieParticleEmitter2_( const MovieLayer & _layer );

    protected:
        bool compileMovieText_( const MovieLayer & _layer );

    protected:
        bool addMovieNode_( const MovieLayer & _layer, const NodePtr & _node, const AnimatablePtr & _animatable, const SoundablePtr & _soundable, const MoviePtr & _movie );

    protected:
        MENGINE_INLINE const NodePtr & getLayerNode_( const MovieLayer & _layer ) const;
        MENGINE_INLINE const AnimationInterfacePtr & getLayerAnimation_( const MovieLayer & _layer ) const;
        MENGINE_INLINE const SoundablePtr & getLayerSoundable_( const MovieLayer & _layer ) const;
        MENGINE_INLINE const MoviePtr & getLayerMovie_( const MovieLayer & _layer ) const;
        MENGINE_INLINE const NodePtr & getLayerParent_( const MovieLayer & _layer ) const;

    protected:
        void getFrameTime_( float _time, uint32_t & _frame, float & _timing ) const;

    protected:
        void updateTime_();
        void updateAnimatablePlay_();
        void updateClipLastFrame_();
        void updateStartInterval_();

        void createCamera3D_();
        void destroyCamera3D_();

    protected:
        void setEnableLayer_( const MovieLayer & _layer, bool _enable );
        MENGINE_INLINE bool getEnableLayer_( const MovieLayer & _layer ) const;

        void setVisibleLayer_( const MovieLayer & _layer, bool _visible );
        MENGINE_INLINE bool getVisibleLayer_( const MovieLayer & _layer ) const;

    protected:
        bool setupBlendingMode_( const MovieLayer & _layer, const MaterialablePtr & _materiable );

    protected:
        ResourceMoviePtr m_resourceMovie;

        RenderCameraProjectionPtr m_renderCameraProjection;
        RenderViewportPtr m_renderViewport;

        typedef Vector<RenderScissorPtr> VectorRenderScissor;
        VectorRenderScissor m_scissors;

        struct Nodies
        {
            Nodies()
                : visible( false )
                , enable( true )
                , child( false )
            {
            }

            NodePtr node;
            RenderInterfacePtr render;
            AnimationInterfacePtr animation;
            SoundablePtr soundable;
            MoviePtr movie;

            bool visible;
            bool enable;
            bool child;
        };

        typedef Vector<Nodies> VectorNodies;
        VectorNodies m_nodies;

        float m_frameTime;
        uint32_t m_currentFrame;

        bool m_interruptEnd;
        bool m_parentMovie;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie> MoviePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const NodePtr & Movie::getLayerNode_( const MovieLayer & _layer ) const
    {
        const Nodies & ns = m_nodies[_layer.index - 1];

        return ns.node;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const AnimationInterfacePtr & Movie::getLayerAnimation_( const MovieLayer & _layer ) const
    {
        const Nodies & ns = m_nodies[_layer.index - 1];

        return ns.animation;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const SoundablePtr & Movie::getLayerSoundable_( const MovieLayer & _layer ) const
    {
        const Nodies & ns = m_nodies[_layer.index - 1];

        return ns.soundable;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const MoviePtr & Movie::getLayerMovie_( const MovieLayer & _layer ) const
    {
        const Nodies & ns = m_nodies[_layer.index - 1];

        return ns.movie;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const NodePtr & Movie::getLayerParent_( const MovieLayer & _layer ) const
    {
        const Nodies & ns = m_nodies[_layer.parent - 1];

        return ns.node;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Movie::getVisibleLayer_( const MovieLayer & _layer ) const
    {
        const Nodies & nd = m_nodies[_layer.index - 1];

        return nd.visible;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Movie::getEnableLayer_( const MovieLayer & _layer ) const
    {
        const Nodies & nd = m_nodies[_layer.index - 1];

        return nd.enable;
    }
}
#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseAnimation.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/Surface.h"
#include "Kernel/ResourceHolder.h"

#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/VectorRenderIndex.h"
#include "Kernel/MatrixProxy.h"

#include "Engine/ShapeQuadFixed.h"
#include "Engine/HotSpotPolygon.h"
#include "Engine/TextField.h"

#include "UnknownMovie2Interface.h"
#include "ResourceMovie2.h"
#include "Movie2Slot.h"
#include "Movie2SubComposition.h"
#include "Movie2Scissor.h"

#include "movie/movie.hpp"

#include "Config/Map.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Movie2> Movie2Ptr;
    //////////////////////////////////////////////////////////////////////////
    class Movie2EventReceiver
        : public AnimationEventReceiver
    {
    public:
    };
    //////////////////////////////////////////////////////////////////////////
    class VisitorMovie2LayerInterface
        : public Mixin
    {
    public:
        virtual void visitMovieLayer( Movie2 * _movie, uint32_t _index, const NodePtr & _node ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<VisitorMovie2LayerInterface> VisitorMovie2LayerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class Movie2
        : public Node
        , public UnknownMovie2Interface
        , public BaseUpdation
        , public BaseRender
        , public BaseEventation
        , public BaseAnimation
    {
        DECLARE_UNKNOWABLE();
        DECLARE_VISITABLE( Node );
        DECLARE_ANIMATABLE();
        DECLARE_UPDATABLE();
        DECLARE_RENDERABLE();
        DECLARE_EVENTABLE( Movie2EventReceiver );

    public:
        Movie2();
        ~Movie2() override;

    public:
        void setResourceMovie2( const ResourcePtr & _resource ) override;
        const ResourcePtr & getResourceMovie2() const override;

    public:
        bool setCompositionName( const ConstString & _compositionName ) override;
        const ConstString & getCompositionName() const override;

    public:
        void setTextAliasEnvironment( const ConstString & _aliasEnvironment ) override;
        const ConstString & getTextAliasEnvironment() const override;

    public:
        bool setWorkAreaFromEvent( const ConstString & _eventName ) override;
        bool removeWorkArea() override;

    public:
        bool hasCompositionBounds() const override;
        const mt::box2f & getCompositionBounds() const override;

    public:
        bool hasSubComposition( const ConstString & _name ) const;
        const Movie2SubCompositionPtr & getSubComposition( const ConstString & _name ) const;

    public:
        bool hasMovieLayers( const ConstString & _name ) const override;
        void setEnableMovieLayers( const ConstString & _name, bool _enable ) override;

    public:
        bool getWorldBoundingBox( mt::box2f * _bb ) const override;

    public:
        typedef Lambda<void( Node * _node, const RenderContext * _context )> LambdaMovieRenderSlot;
        void foreachRenderSlots( const RenderContext * _context, const LambdaMovieRenderSlot & _lambda ) override;

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

        void _afterActivate() override;

    protected:
        void _setLoop( bool _value ) override;
        void _setTime( float _time ) override;
        float _getTime() const override;
        float _getDuration() const override;
        void _setFirstFrame() override;
        void _setLastFrame() override;
        void _setAnimationSpeedFactor( float _factor ) override;

    protected:
        void _update( const UpdateContext * _context ) override;

    protected:
        void render( const RenderContext * _state ) const override;

    protected:
        void _changeParent( Node * _oldParent, Node * _newParent ) override;
        void _addChild( const NodePtr & _node ) override;
        void _removeChild( const NodePtr & _node ) override;

    public:
        void addSurface_( const SurfacePtr & _surface, bool _compile );
        void removeSurface_( const SurfacePtr & _surface );

    public:
        void addSprite_( uint32_t _index, const ShapeQuadFixedPtr & _sprite );
        const ShapeQuadFixedPtr & getSprite_( uint32_t _index ) const;
        const ShapeQuadFixedPtr & findSprite( const ConstString & _name ) const;
        bool hasSprite( const ConstString & _name ) const;

    public:
        void addParticle_( uint32_t _index, const NodePtr & _particleEmitter );
        const NodePtr & getParticle_( uint32_t _index ) const;
        const NodePtr & findParticle( const ConstString & _name ) const;
        bool hasParticle( const ConstString & _name ) const;

    public:
        void addSlot_( uint32_t _index, const Movie2SlotPtr & _slot );
        const Movie2SlotPtr & getSlot_( uint32_t _index ) const;
        const Movie2SlotPtr & findSlot( const ConstString & _name ) const;
        bool hasSlot( const ConstString & _name ) const;
        void visitSlots( const VisitorMovie2LayerInterfacePtr & _visitor );

    public:
        void addSocket_( uint32_t _index, const HotSpotPolygonPtr & _hotspot );
        const HotSpotPolygonPtr & getSocket_( uint32_t _index ) const;
        const HotSpotPolygonPtr & findSocket( const ConstString & _name ) const;
        bool hasSocket( const ConstString & _name ) const;
        void visitSockets( const VisitorMovie2LayerInterfacePtr & _visitor );

    public:
        void addText_( uint32_t _index, const TextFieldPtr & _text );
        const TextFieldPtr & getText_( uint32_t _index ) const;
        const TextFieldPtr & findText( const ConstString & _name ) const;
        bool hasText( const ConstString & _name ) const;
        void visitTexts( const VisitorMovie2LayerInterfacePtr & _visitor );

    public:
        void addSubMovieComposition_( const ConstString & _name, const Movie2SubCompositionPtr & _subComposition );

    public:
        void addMatrixProxy_( const MatrixProxyPtr & _matrixProxy );

    public:
        struct Camera
        {
            RenderCameraProjectionPtr projection;
            RenderViewportPtr viewport;
        };

        Camera * addCamera_( const ConstString & _name, const RenderCameraProjectionPtr & _projection, const RenderViewportPtr & _viewport );
        bool removeCamera_( const ConstString & _name );
        bool hasCamera_( const ConstString & _name ) const;
        bool getCamera_( const ConstString & _name, Camera ** _camera );

    public:
        bool interruptElements_();
        bool checkInterruptElement_() const;

    protected:
        bool createCompositionLayers_();
        void destroyCompositionLayers_();

    protected:
        void _destroy() override;

    protected:
        void updateAnimationSpeedFactor_( float _factor );

    public:
        ResourceHolder<ResourceMovie2> m_resourceMovie2;

        ConstString m_compositionName;

        const aeMovieComposition * m_composition;

        float m_duration;
        float m_frameDuration;

        bool m_hasBounds;
        mt::box2f m_bounds;

        mutable VectorRenderVertex2D m_vertices;
        mutable VectorRenderIndex m_indices;

        typedef Map<ConstString, Camera> MapCameras;
        MapCameras m_cameras;

        typedef Map<ConstString, Movie2SubCompositionPtr> MapSubCompositions;
        MapSubCompositions m_subCompositions;

        typedef Vector<SurfacePtr> VectorSurfaces;
        VectorSurfaces m_surfaces;

        typedef Map<uint32_t, Movie2SlotPtr> MapSlots;
        MapSlots m_slots;

        typedef Map<uint32_t, HotSpotPolygonPtr> MapSockets;
        MapSockets m_sockets;

        ConstString m_aliasEnvironment;

        typedef Map<uint32_t, TextFieldPtr> MapTexts;
        MapTexts m_texts;

        typedef Map<uint32_t, ShapeQuadFixedPtr> MapSprites;
        MapSprites m_sprites;

        typedef Map<uint32_t, NodePtr> MapParticleEmitter2s;
        MapParticleEmitter2s m_particleEmitters;

        typedef Vector<MatrixProxyPtr> VectorMatrixProxies;
        VectorMatrixProxies m_matrixProxies;

    protected:
        static ae_bool_t __movie_composition_node_provider( const aeMovieNodeProviderCallbackData * _callbackData, ae_voidptrptr_t _nd, ae_voidptr_t _ud );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie2> Movie2Ptr;
    //////////////////////////////////////////////////////////////////////////
}
#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseAnimation.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/Surface.h"
#include "Kernel/ResourceHolder.h"

#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/RenderVertex2D.h"
#include "Kernel/RenderIndex.h"
#include "Kernel/MatrixProxy.h"

#include "Engine/ShapeQuadFixed.h"
#include "Engine/HotSpotPolygon.h"
#include "Engine/TextField.h"

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
        virtual void visitMovieLayer( const Movie2Ptr & _movie, uint32_t _index, const NodePtr & _node ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<VisitorMovie2LayerInterface> VisitorMovie2LayerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class Movie2
        : public Node
        , public BaseUpdation
        , public BaseRender
        , public BaseEventation
        , public BaseAnimation
    {
        DECLARE_VISITABLE( Node );
        DECLARE_ANIMATABLE();
        DECLARE_UPDATABLE();
        DECLARE_RENDERABLE();
        DECLARE_EVENTABLE( Movie2EventReceiver );

    public:
        Movie2();
        ~Movie2() override;

    public:
        void setResourceMovie2( const ResourceMovie2Ptr & _resourceMovie );
        const ResourceMovie2Ptr & getResourceMovie2() const;

    public:
        bool setCompositionName( const ConstString & _compositionName );
        const ConstString & getCompositionName() const;

    public:
        void setTextAliasEnvironment( const ConstString & _aliasEnvironment );
        const ConstString & getTextAliasEnvironment() const;

    public:
        float getDuration() const;
        bool setWorkAreaFromEvent( const ConstString & _eventName );
        bool removeWorkArea();

    public:
        bool hasCompositionBounds() const;
        const Viewport & getCompositionBounds() const;

    public:
        bool hasSubComposition( const ConstString & _name ) const;
        const Movie2SubCompositionPtr & getSubComposition( const ConstString & _name ) const;

    public:
        void setEnableMovieLayers( const ConstString & _name, bool _enable );

    public:
        typedef Lambda<void( Node * _node, const RenderContext * _context )> LambdaMovieRenderSlot;
        void foreachRenderSlots( const RenderContext * _context, const LambdaMovieRenderSlot & _lambda );

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
        void _setFirstFrame() override;
        void _setLastFrame() override;
        void _setAnimationSpeedFactor( float _factor ) override;

    protected:
        void _update( const UpdateContext * _context ) override;

    protected:
        void _render( const RenderContext * _state ) override;

    protected:
        void _changeParent( Node * _oldParent, Node * _newParent ) override;
        void _addChild( const NodePtr & _node ) override;
        void _removeChild( const NodePtr & _node ) override;

    public:
        void addSurface( const SurfacePtr & _surface );
        void removeSurface( const SurfacePtr & _surface );

    protected:
        void addSprite_( uint32_t _index, const ShapeQuadFixedPtr & _sprite );
        const ShapeQuadFixedPtr & getSprite_( uint32_t _index ) const;

    public:
        const ShapeQuadFixedPtr & findSprite( const ConstString & _name ) const;
        bool hasSprite( const ConstString & _name ) const;

    protected:
        void addParticle_( uint32_t _index, const NodePtr & _particleEmitter );
        const NodePtr & getParticle_( uint32_t _index ) const;

    public:
        const NodePtr & findParticle( const ConstString & _name ) const;
        bool hasParticle( const ConstString & _name ) const;

    protected:
        void addSlot_( uint32_t _index, const Movie2SlotPtr & _slot );
        const Movie2SlotPtr & getSlot_( uint32_t _index ) const;

    public:
        const Movie2SlotPtr & findSlot( const ConstString & _name ) const;
        bool hasSlot( const ConstString & _name ) const;
        void visitSlots( const VisitorMovie2LayerInterfacePtr & _visitor );

    protected:
        void addSocket_( uint32_t _index, const HotSpotPolygonPtr & _hotspot );
        const HotSpotPolygonPtr & getSocket_( uint32_t _index ) const;

    public:
        const HotSpotPolygonPtr & findSocket( const ConstString & _name ) const;
        bool hasSocket( const ConstString & _name ) const;
        void visitSockets( const VisitorMovie2LayerInterfacePtr & _visitor );

    protected:
        void addText_( uint32_t _index, const TextFieldPtr & _text );
        const TextFieldPtr & getText_( uint32_t _index ) const;

    public:
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

        Camera * addCamera( const ConstString & _name, const RenderCameraProjectionPtr & _projection, const RenderViewportPtr & _viewport );
        bool removeCamera( const ConstString & _name );
        bool hasCamera( const ConstString & _name ) const;

        bool getCamera( const ConstString & _name, Camera ** _camera );

    public:
        bool interruptElements();
        bool checkInterruptElement() const;

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
        Viewport m_bounds;

        VectorRenderVertex2D m_vertices;
        VectorRenderIndices m_indices;

        struct Mesh
        {
            uint32_t begin_vertices;
            uint32_t count_vertices;

            uint32_t begin_indices;
            uint32_t count_indices;

            RenderMaterialInterfacePtr material;
        };

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
#pragma once

#include "Engine/HotSpotPolygon.h"
#include "Engine/TextField.h"

#include "Kernel/ShapeQuadFixed.h"
#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseAnimation.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/Surface.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/VectorRenderIndex.h"
#include "Kernel/MatrixProxy.h"
#include "Kernel/Hashtable.h"
#include "Kernel/Map.h"

#include "Movie2Interface.h"

#include "ResourceMovie2.h"
#include "Movie2Slot.h"
#include "Movie2SubComposition.h"
#include "Movie2Scissor.h"

#include "movie/movie.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Movie2EventReceiverInterface
        : public AnimationEventReceiverInterface
    {
    public:
    };
    //////////////////////////////////////////////////////////////////////////
    class Movie2
        : public Node
        , public UnknownMovie2Interface
        , protected BaseUpdation
        , protected BaseRender
        , protected BaseEventation
        , protected BaseAnimation
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( Movie2 );
        DECLARE_UNKNOWABLE();
        DECLARE_VISITABLE( Node );
        DECLARE_ANIMATABLE();
        DECLARE_UPDATABLE();
        DECLARE_RENDERABLE();
        DECLARE_EVENTABLE();
        DECLARE_TRANSFORMABLE();

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
        bool hasSubComposition( const ConstString & _name ) const override;
        const Movie2SubCompositionPtr & getSubComposition( const ConstString & _name ) const override;
        void foreachSubComposition( const LambdaSubCompositions & _lambda ) const override;

    public:
        bool hasMovieLayers( const ConstString & _name ) const override;
        void setEnableMovieLayers( const ConstString & _name, bool _enable ) override;
        void setExtraOpacityMovieLayers( const ConstString & _name, float _opacity ) override;

    public:
        bool getWorldBoundingBox( mt::box2f * const _bb ) const override;

    public:
        void foreachRenderSlots( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const LambdaMovieRenderSlot & _lambda ) const override;

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

        bool _afterActivate() override;

    protected:
        void _setLoop( bool _value ) override;
        void _setTime( float _time ) override;
        float _getTime() const override;
        float _getDuration() const override;
        void _setFirstFrame() override;
        void _setLastFrame() override;
        void _setAnimationSpeedFactor( float _factor ) override;

    protected:
        void update( const UpdateContext * _context ) override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        void _hierarchyChangeParent( Node * _oldParent, Node * _newParent ) override;
        void _hierarchyAddChild( const NodePtr & _node ) override;
        void _hierarchyRemoveChild( const NodePtr & _node ) override;

    protected:
        void addSurface_( const SurfacePtr & _surface, bool _compile );
        void removeSurface_( const SurfacePtr & _surface );

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
        typedef Lambda<void( Movie2 * _movie, uint32_t _index, const NodePtr & _node )> LambdaNodes;

    public:
        const Movie2SlotPtr & findSlot( const ConstString & _name ) const;
        bool hasSlot( const ConstString & _name ) const;
        void foreachSlots( const LambdaNodes & _lambda );

    protected:
        void addSocket_( uint32_t _index, const HotSpotPolygonPtr & _hotspot );
        const HotSpotPolygonPtr & getSocket_( uint32_t _index ) const;

    public:
        const HotSpotPolygonPtr & findSocket( const ConstString & _name ) const;
        bool hasSocket( const ConstString & _name ) const;
        void foreachSockets( const LambdaNodes & _lambda );


    protected:
        void addText_( uint32_t _index, const TextFieldPtr & _text );
        const TextFieldPtr & getText_( uint32_t _index ) const;

    public:
        const TextFieldPtr & findText( const ConstString & _name ) const;
        bool hasText( const ConstString & _name ) const;
        void foreachTexts( const LambdaNodes & _lambda );

    protected:
        void addSubMovieComposition_( const ConstString & _name, const Movie2SubCompositionPtr & _subComposition );

    protected:
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
        bool getCamera_( const ConstString & _name, Camera ** const _camera );

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
        ResourceMovie2Ptr m_resourceMovie2;

        ConstString m_compositionName;

        const aeMovieComposition * m_composition;

        float m_duration;
        float m_frameDuration;

        float m_preCompileTime;

        bool m_hasBounds;
        mt::box2f m_bounds;

        mutable VectorRenderVertex2D m_vertices;
        mutable VectorRenderIndex m_indices;

        typedef Map<ConstString, Camera> MapCameras;
        MapCameras m_cameras;

        typedef Hashtable<ConstString, Movie2SubCompositionPtr> HashtableSubCompositions;
        HashtableSubCompositions m_subCompositions;

        typedef Vector<SurfacePtr> VectorSurfaces;
        VectorSurfaces m_surfaces;

        typedef Hashtable<uint32_t, Movie2SlotPtr> HashtableSlots;
        HashtableSlots m_slots;

        typedef Hashtable<uint32_t, HotSpotPolygonPtr> HashtableSockets;
        HashtableSockets m_sockets;

        ConstString m_aliasEnvironment;

        typedef Hashtable<uint32_t, TextFieldPtr> HashtableTexts;
        HashtableTexts m_texts;

        typedef Hashtable<uint32_t, ShapeQuadFixedPtr> HashtableSprites;
        HashtableSprites m_sprites;

        typedef Hashtable<uint32_t, NodePtr> HashtableAstralaxEmitters;
        HashtableAstralaxEmitters m_astralaxEmitters;

        typedef Vector<MatrixProxyPtr> VectorMatrixProxies;
        VectorMatrixProxies m_matrixProxies;

    protected:
        static ae_bool_t __movie_composition_node_provider( const aeMovieNodeProviderCallbackData * _callbackData, ae_voidptrptr_t _nd, ae_voidptr_t _ud );
        static ae_void_t __movie_composition_node_deleter( const aeMovieNodeDeleterCallbackData * _callbackData, ae_voidptr_t _ud );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Movie2> Movie2Ptr;
    //////////////////////////////////////////////////////////////////////////
}
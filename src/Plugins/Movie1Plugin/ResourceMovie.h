#pragma once

#include "MovieKeyFrameInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/Viewport.h"
#include "Kernel/Polygon.h"
#include "Kernel/Vector.h"

#include "math/vec2.h"
#include "math/vec3.h"
#include "math/mat4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<class ResourceShape> ResourceShapePtr;
    //////////////////////////////////////////////////////////////////////////
    const uint32_t MOVIE_LAYER_NODE = 0x0001;
    const uint32_t MOVIE_LAYER_ANIMATABLE = 0x0002;
    const uint32_t MOVIE_LAYER_MOVIE = 0x0004;
    const uint32_t MOVIE_LAYER_THREED = 0x0008;
    const uint32_t MOVIE_LAYER_AUDIO = 0x0010;
    const uint32_t MOVIE_LAYER_EVENT = 0x0020;
    const uint32_t MOVIE_LAYER_EXTRA = 0x0040;
    const uint32_t MOVIE_LAYER_SUB_MOVIE = 0x0080;
    const uint32_t MOVIE_LAYER_MESH_2D = 0x0100;
    const uint32_t MOVIE_LAYER_UNSTOPPABLE = 0x0200;
    const uint32_t MOVIE_LAYER_SCENE_EFFECT = 0x0400;
    const uint32_t MOVIE_LAYER_INTERNAL = 0x0800;

    const uint32_t movie_layer_parent_none = (uint32_t)-1;

    const uint32_t MOVIE_LAYER_PARAM_TEXT_HORIZONTAL_CENTER = 0x00000001;
    const uint32_t MOVIE_LAYER_PARAM_TEXT_VERTICAL_CENTER = 0x00000002;
    const uint32_t MOVIE_LAYER_PARAM_PARTICLE_TRANSLATE = 0x00000004;
    //////////////////////////////////////////////////////////////////////////
    struct MovieLayer
    {
        MovieLayer()
            : index( 0 )
            , parent( 0 )
            , playCount( 1 )
            , stretch( 1.f )
            , in( 0.f )
            , out( 0.f )
            , startInterval( 0.f )
            , state( 0 )
            , position( 0.f, 0.f, 0.f )
            , anchorPoint( 0.f, 0.f, 0.f )
            , scale( 1.f, 1.f, 1.f )
            , rotation( 0.f, 0.f, 0.f )
            , viewport( 0.f, 0.f, 0.f, 0.f )
            , params( 0 )
            , timeRemap( false )
            , shape( false )
            , polygon( false )
            , loop( false )
            , switcher( false )
            , hasViewport( false )
        {
        }

        ConstString name;
        ConstString type;

        uint32_t index;
        uint32_t parent;

        uint32_t playCount;
        float stretch;

        float in;
        float out;
        float startInterval;

        uint32_t state;

        ConstString blendingMode;

        ConstString source;

        mt::vec3f position;
        mt::vec3f anchorPoint;
        mt::vec3f scale;
        mt::vec3f rotation;

        Viewport viewport;

        uint32_t params;

        bool timeRemap;
        bool shape;
        bool polygon;
        bool loop;
        bool switcher;

        bool hasViewport;

        bool hasParam( uint32_t _param ) const
        {
            return (params & _param) == _param;
        }

        bool isNode() const
        {
            return (state & MOVIE_LAYER_NODE) == MOVIE_LAYER_NODE;
        }

        bool isAnimation() const
        {
            return (state & MOVIE_LAYER_ANIMATABLE) == MOVIE_LAYER_ANIMATABLE;
        }

        bool isMovie() const
        {
            return (state & MOVIE_LAYER_MOVIE) == MOVIE_LAYER_MOVIE;
        }

        bool isMesh2D() const
        {
            return (state & MOVIE_LAYER_MESH_2D) == MOVIE_LAYER_MESH_2D;
        }

        bool isThreeD() const
        {
            return (state & MOVIE_LAYER_THREED) == MOVIE_LAYER_THREED;
        }

        bool isAudio() const
        {
            return (state & MOVIE_LAYER_AUDIO) == MOVIE_LAYER_AUDIO;
        }

        bool isEvent() const
        {
            return (state & MOVIE_LAYER_EVENT) == MOVIE_LAYER_EVENT;
        }

        bool isExtra() const
        {
            return (state & MOVIE_LAYER_EXTRA) == MOVIE_LAYER_EXTRA;
        }

        bool isSubMovie() const
        {
            return (state & MOVIE_LAYER_SUB_MOVIE) == MOVIE_LAYER_SUB_MOVIE;
        }

        bool isUnstoppable() const
        {
            return (state & MOVIE_LAYER_UNSTOPPABLE) == MOVIE_LAYER_UNSTOPPABLE;
        }

        bool isSceneEffect() const
        {
            return (state & MOVIE_LAYER_SCENE_EFFECT) == MOVIE_LAYER_SCENE_EFFECT;
        }

        bool isInternal() const
        {
            return (state & MOVIE_LAYER_INTERNAL) == MOVIE_LAYER_INTERNAL;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<MovieLayer> VectorMovieLayers;
    //////////////////////////////////////////////////////////////////////////
    struct MovieLayerCamera3D
    {
        float width;
        float height;

        mt::vec3f cameraPosition;
        mt::vec3f cameraInterest;
        float cameraFOV;
        float cameraAspect;
    };
    //////////////////////////////////////////////////////////////////////////
    struct MovieLayer3D
        : public MovieLayer
    {
        MovieLayerCamera3D camera;
    };
    //////////////////////////////////////////////////////////////////////////
    class ResourceMovie
        : public Resource
    {
        DECLARE_FACTORABLE( ResourceMovie );
        DECLARE_VISITABLE( Resource );

    public:
        ResourceMovie();
        ~ResourceMovie() override;

    public:
        void setFrameDuration( float _frameDuration );
        float getFrameDuration() const;

        void setDuration( float _duration );
        float getDuration() const;

        void setFrameCount( uint32_t _frameCount );
        uint32_t getFrameCount() const;

        void setMaxLayerIndex( uint32_t _maxLayerIndex );
        uint32_t getMaxLayerIndex() const;

        void setSize( const mt::vec2f & _size );
        const mt::vec2f & getSize() const;

        void setLoopSegment( const mt::vec2f & _loopSegment );
        const mt::vec2f & getLoopSegment() const;

    public:
        void setLayers( const VectorMovieLayers & _layers );
        const VectorMovieLayers & getLayers() const;

    public:
        bool hasCamera3D() const;
        void setCamera3D( const MovieLayerCamera3D & _camera3d );
        const MovieLayerCamera3D & getCamera3D() const;

    public:
        ResourceShapePtr getSocketResourceShape( const ConstString & _socketName ) const;

    public:
        bool hasAnchorPoint() const;
        void setAnchorPoint( const mt::vec3f & _anchorPoint );
        const mt::vec3f & getAnchorPoint() const;

    public:
        bool hasOffsetPoint() const;
        void setOffsetPoint( const mt::vec3f & _offsetPoint );
        const mt::vec3f & getOffsetPoint() const;

    public:
        bool hasBoundBox() const;
        void setBoundBox( const mt::box2f & _boundBox );
        const mt::box2f & getBoundBox() const;

    public:
        MENGINE_INLINE const MovieFramePackInterfacePtr & getFramePack() const;

    public:
        bool hasMovieLayer( const ConstString & _name, const MovieLayer ** const _layer ) const;
        bool hasMovieLayerType( const ConstString & _name, const ConstString & _type, const MovieLayer ** const _layer ) const;

    public:
        typedef Lambda<void( const MovieFramePackInterfacePtr &, const MovieLayer & )> LambdaResourceMovie;
        void foreachResourceMovie( const LambdaResourceMovie & _lambda );

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        float m_frameDuration;
        float m_duration;

        mt::vec2f m_loopSegment;

        bool m_hasBoundBox;
        mt::box2f m_boundBox;

        bool m_hasAnchorPoint;
        mt::vec3f m_anchorPoint;

        bool m_hasOffsetPoint;
        mt::vec3f m_offsetPoint;

        MovieFramePackInterfacePtr m_keyFramePack;

        uint32_t m_frameCount;

        mt::vec2f m_size;

        VectorMovieLayers m_layers;

        bool m_hasCamera3D;
        MovieLayerCamera3D m_camera3D = {};

        uint32_t m_maxLayerIndex;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceMovie> ResourceMoviePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const MovieFramePackInterfacePtr & ResourceMovie::getFramePack() const
    {
        return m_keyFramePack;
    }
    //////////////////////////////////////////////////////////////////////////
}
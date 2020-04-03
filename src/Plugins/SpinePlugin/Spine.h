#pragma once

#include "Interface/RenderMaterialInterface.h"

#include "SpineInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseAnimation.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/VectorRenderIndex.h"

#include "ResourceSpineSkeleton.h"

#include "spine/spine.h"

namespace Mengine
{
    class Spine
        : public Node
        , public BaseAnimation
        , public BaseUpdation
        , public BaseRender
        , public BaseEventation
        , public UnknownSpineInterface
    {
        DECLARE_VISITABLE( Node );
        DECLARE_UNKNOWABLE();
        DECLARE_UPDATABLE();
        DECLARE_RENDERABLE();
        DECLARE_ANIMATABLE();
        DECLARE_EVENTABLE();

    public:
        Spine();
        ~Spine() override;

    public:
        void setResourceSpineSkeleton( const ResourcePtr & _resourceSpineSkeleton ) override;
        const ResourcePtr & getResourceSpineSkeleton() const override;

    public:
        bool mixAnimation( const ConstString & _first, const ConstString & _second, float _mix ) override;

    public:
        bool setStateAnimation( const ConstString & _state, const ConstString & _name, float _timing, float _speedFactor, bool _loop ) override;
        bool removeStateAnimation( const ConstString & _state ) override;

        bool hasSkeletonAnimationName( const ConstString & _name ) const override;

    public:
        bool setStateAnimationSpeedFactor( const ConstString & _state, float _speedFactor ) override;
        float getStateAnimationSpeedFactor( const ConstString & _state ) const override;

        bool setStateAnimationTiming( const ConstString & _state, float _timing ) override;
        float getStateAnimationTiming( const ConstString & _state ) const override;

        bool setStateAnimationFreeze( const ConstString & _state, bool _freeze ) override;
        bool getStateAnimationFreeze( const ConstString & _state ) const override;

        float getStateAnimationDuration( const ConstString & _state ) const override;

    public:
        bool setStateAnimationLastFrame( const ConstString & _state, const ConstString & _name ) override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void update( const UpdateContext * _context ) override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        bool _afterActivate() override;

    protected:
        bool _play( uint32_t _enumerator, float _time ) override;
        bool _restart( uint32_t _enumerator, float _time ) override;
        void _pause( uint32_t _enumerator ) override;
        void _resume( uint32_t _enumerator, float _resume ) override;
        bool _stop( uint32_t _enumerator ) override;
        void _end( uint32_t _enumerator ) override;
        bool _interrupt( uint32_t _enumerator ) override;

    public:
        void addAnimationEvent_( spAnimationState * _animationState, spEventType _type, spTrackEntry * _entry, spEvent * _event );

    protected:
        void fillVertices_( RenderVertex2D * _vertices2D, const float * _vertices, const float * _uv, ColorValue_ARGB _argb, int _count, const mt::mat4f & _wm ) const;
        void fillIndices_( RenderIndex * _vertices2D, const RenderIndex * _triangles, uint32_t _count ) const;

    protected:
        RenderMaterialInterfacePtr makeMaterial_( spSlot * _slot, const ResourceImage * _resourceImage ) const;

    protected:
        void updateAnimation_();

    protected:
        ResourceSpineSkeletonPtr m_resourceSpineSkeleton;

        spSkeleton * m_skeleton;
        spAnimationStateData * m_animationStateData;

        struct AnimationDesc
        {
            ConstString state;
            ConstString name;
            spAnimationState * animationState;
            spTrackEntry * track;
            float time;
            float duration;
            float speedFactor;
            bool freeze;
            bool complete;
            bool loop;
        };

        typedef Vector<AnimationDesc> VectorAnimations;
        VectorAnimations m_animations;

        struct AttachmentMeshDesc
        {
            uint32_t index;

            const ResourceImage * image;
            RenderMaterialInterfacePtr material;

            VectorRenderVertex2D vertices;
            VectorRenderIndex indices;
        };

        typedef Vector<AttachmentMeshDesc> VectorAttachmentMesh;
        mutable VectorAttachmentMesh m_attachmentMeshes;

        struct AnimationEventDesc
        {
            int trackIndex;
            spEventType type;

            ConstString state;
            ConstString name;

            const char * eventName;
            int eventIntValue;
            float eventFloatValue;
            const char * eventStringValue;

            int loopCount;
        };

        typedef Vector<AnimationEventDesc> VectorAnimationEvent;
        VectorAnimationEvent m_events;
        VectorAnimationEvent m_eventsAux;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Spine> SpinePtr;
    //////////////////////////////////////////////////////////////////////////
}
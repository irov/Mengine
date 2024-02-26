#pragma once

#include "Interface/RenderMaterialInterface.h"

#include "SpineInterface.h"

#include "Kernel/BaseAnimation.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/BaseEventation.h"

#include "ResourceSpineSkeleton.h"

#include "spine/spine.h"

namespace Mengine
{
    class SamplerSpineAnimation
        : public SamplerSpineAnimationInterface
        , protected BaseAnimation
        , protected BaseUpdation
        , protected BaseEventation
    {
        DECLARE_FACTORABLE( SamplerSpineAnimation );
        DECLARE_ANIMATABLE();
        DECLARE_EVENTABLE();
        DECLARE_UPDATABLE();

    public:
        SamplerSpineAnimation();
        ~SamplerSpineAnimation() override;

    public:
        void setResourceSpineSkeleton( const ResourcePtr & _resourceSpineSkeleton ) override;
        const ResourcePtr & getResourceSpineSkeleton() const override;

    public:
        void setAnimationName( const ConstString & _animationName ) override;
        const ConstString & getAnimationName() const override;

    public:
        void setAnimationEnable( bool _animationEnable ) override;
        bool getAnimationEnable() const override;

    public:
        spAnimationState * getAnimationState() const;

    public:
        float _getDuration() const override;
        void _setLoop( bool _loop ) override;
        void _setTime( float _time ) override;

        void _setFirstFrame() override;
        void _setLastFrame() override;

    public:
        void update( const UpdateContext * _context ) override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        bool _play( uint32_t _enumerator, float _time ) override;
        bool _restart( uint32_t _enumerator, float _time ) override;
        void _pause( uint32_t _enumerator ) override;
        void _resume( uint32_t _enumerator, float _resume ) override;
        bool _stop( uint32_t _enumerator ) override;
        void _end( uint32_t _enumerator ) override;
        bool _interrupt( uint32_t _enumerator ) override;

    protected:
        ResourceSpineSkeletonPtr m_resourceSpineSkeleton;

        ConstString m_animationName;

        bool m_animationEnable;

        spAnimationStateData * m_animationStateData;
        spAnimationState * m_animationState;
        spTrackEntry * m_trackEntry;

        float m_time;
        float m_duration;

        struct AnimationEventDesc
        {
            int trackIndex;
            spEventType type;

            const Char * eventName;
            int eventIntValue;
            float eventFloatValue;
            const Char * eventStringValue;

            int loopCount;
        };

        typedef Vector<AnimationEventDesc> VectorAnimationEvent;
        VectorAnimationEvent m_events;
        VectorAnimationEvent m_eventsAux;

    public:
        void addAnimationEvent_( spAnimationState * _animationState, spEventType _type, spTrackEntry * _entry, spEvent * _event );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SamplerSpineAnimation, SamplerSpineAnimationInterface> SamplerSpineAnimationPtr;
    //////////////////////////////////////////////////////////////////////////
}
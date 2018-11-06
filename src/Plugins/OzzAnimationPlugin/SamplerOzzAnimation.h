#pragma once

#include "OzzAnimationInterface.h"

#include "Kernel/BaseAnimation.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/BaseEventation.h"

#include "ResourceOzzAnimation.h"
#include "ResourceOzzSkeleton.h"

#include "ozz/animation/runtime/sampling_job.h"

namespace Mengine
{
    class SamplerOzzAnimation
        : public SamplerOzzAnimationInterface
        , public Factorable
        , public BaseAnimation
        , public BaseUpdation
        , public BaseEventation
    {
        DECLARE_ANIMATABLE();
        DECLARE_EVENTABLE( SamplerOzzAnimationEventReceiver );

    public:
        SamplerOzzAnimation();
        ~SamplerOzzAnimation();

    public:
        void setResourceOzzAnimation( const ResourcePtr & _resource ) override;
        const ResourcePtr & getResourceOzzAnimation() const override;

        void setResourceOzzSkeleton( const ResourcePtr & _resource ) override;
        const ResourcePtr & getResourceOzzSkeleton() const override;

        void setWeight( float _weight ) override;
        float getWeight() const override;

    public:
        const ozz::Range<ozz::math::SoaTransform> & getLocals() const;

    public:
        void _update( const UpdateContext * _context ) override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void updateTiming_( float _timing );
        void updateAnimation_();

    protected:
        bool _play( uint32_t _enumerator, float _time ) override;
        bool _restart( uint32_t _enumerator, float _time ) override;
        void _pause( uint32_t _enumerator ) override;
        void _resume( uint32_t _enumerator, float _time ) override;
        void _stop( uint32_t _enumerator ) override;
        void _end( uint32_t _enumerator ) override;
        bool _interrupt( uint32_t _enumerator ) override;

    public:
        ResourceOzzAnimationPtr m_resourceOzzAnimation;
        ResourceOzzSkeletonPtr m_resourceOzzSkeleton;

        ozz::animation::SamplingCache* m_cache;
        ozz::Range<ozz::math::SoaTransform> m_locals;

        float m_weight;
        float m_time;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SamplerOzzAnimation, SamplerOzzAnimationInterface> SamplerOzzAnimationPtr;
    //////////////////////////////////////////////////////////////////////////
}
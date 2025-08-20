#pragma once

#include "OzzAnimationInterface.h"

#include "OzzDetail.h"
#include "ResourceOzzAnimation.h"
#include "ResourceOzzSkeleton.h"

#include "Kernel/BaseAnimation.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/BaseEventation.h"

#include "ozz/animation/runtime/sampling_job.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SamplerOzzAnimation
        : public SamplerOzzAnimationInterface
        , protected BaseAnimation
        , protected BaseUpdation
        , protected BaseEventation
    {
        DECLARE_FACTORABLE( SamplerOzzAnimation );
        DECLARE_ANIMATABLE();
        DECLARE_UPDATABLE();
        DECLARE_EVENTABLE();

    public:
        SamplerOzzAnimation();
        ~SamplerOzzAnimation() override;

    public:
        void setResourceOzzAnimation( const ResourcePtr & _resource ) override;
        const ResourcePtr & getResourceOzzAnimation() const override;

        void setResourceOzzSkeleton( const ResourcePtr & _resource ) override;
        const ResourcePtr & getResourceOzzSkeleton() const override;

        void setWeight( float _weight ) override;
        float getWeight() const override;

    public:
        const ozz::vector<ozz::math::SoaTransform> & getLocals() const;

    public:
        void update( const UpdateContext * _context ) override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void updateTiming_( float _timing );
        bool updateAnimation_();

    protected:
        bool _play( UniqueId _playId, float _time ) override;
        bool _restart( UniqueId _playId, float _time ) override;
        void _pause( UniqueId _playId ) override;
        void _resume( UniqueId _playId, float _time ) override;
        bool _stop( UniqueId _playId ) override;
        void _end( UniqueId _playId ) override;
        bool _interrupt( UniqueId _playId ) override;

    protected:
        float _getDuration() const override;

    public:
        ResourceOzzAnimationPtr m_resourceOzzAnimation;
        ResourceOzzSkeletonPtr m_resourceOzzSkeleton;

        ozz::animation::SamplingJob::Context m_context;
        ozz::vector<ozz::math::SoaTransform> m_locals;

        float m_weight;
        float m_time;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SamplerOzzAnimation, SamplerOzzAnimationInterface> SamplerOzzAnimationPtr;
    //////////////////////////////////////////////////////////////////////////
}
#include "SamplerOzzAnimation.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/EventableHelper.h"

#include "OzzDetail.h"

#include "ozz/base/memory/allocator.h"
#include "ozz/base/maths/soa_transform.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SamplerOzzAnimation::SamplerOzzAnimation()
        : m_cache( nullptr )
        , m_weight( 1.f )
        , m_time( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SamplerOzzAnimation::~SamplerOzzAnimation()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerOzzAnimation::setResourceOzzAnimation( const ResourcePtr & _resource )
    {
        m_resourceOzzAnimation = _resource;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & SamplerOzzAnimation::getResourceOzzAnimation() const
    {
        return m_resourceOzzAnimation;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerOzzAnimation::setResourceOzzSkeleton( const ResourcePtr & _resource )
    {
        m_resourceOzzSkeleton = _resource;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & SamplerOzzAnimation::getResourceOzzSkeleton() const
    {
        return m_resourceOzzSkeleton;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerOzzAnimation::setWeight( float _weight )
    {
        m_weight = _weight;
    }
    //////////////////////////////////////////////////////////////////////////
    float SamplerOzzAnimation::getWeight() const
    {
        return m_weight;
    }
    //////////////////////////////////////////////////////////////////////////
    const ozz::Range<ozz::math::SoaTransform> & SamplerOzzAnimation::getLocals() const
    {
        return m_locals;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerOzzAnimation::update( const UpdateContext * _context )
    {
        if( this->isCompile() == false )
        {
            return;
        }

        if( this->isPlay() == false )
        {
            return;
        }

        float totalTiming = this->calcTotalTime( _context );

        this->updateTiming_( totalTiming );
        this->updateAnimation_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SamplerOzzAnimation::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceOzzAnimation, false );
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceOzzSkeleton, false );

        if( m_resourceOzzAnimation->compile() == false )
        {
            return false;
        }

        if( m_resourceOzzSkeleton->compile() == false )
        {
            return false;
        }

        const ozz::animation::Skeleton & skeleton = m_resourceOzzSkeleton->getOzzSkeleton();

        int32_t num_soa_joints = skeleton.num_soa_joints();
        int32_t num_joints = skeleton.num_joints();

        ozz::memory::Allocator * allocator = ozz::memory::default_allocator();

        m_locals = Detail::AllocateRange<ozz::math::SoaTransform>( allocator, num_soa_joints );
        m_cache = allocator->New<ozz::animation::SamplingCache>( num_joints );

        this->updateAnimation_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerOzzAnimation::_release()
    {
        ozz::memory::Allocator * allocator = ozz::memory::default_allocator();

        Detail::DeallocateRange( allocator, m_locals );
        allocator->Delete( m_cache );

        m_resourceOzzAnimation->release();
        m_resourceOzzSkeleton->release();
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerOzzAnimation::updateTiming_( float _timing )
    {
        m_time += _timing;

        const ozz::animation::Animation & ozz_animation = m_resourceOzzAnimation->getOzzAnimation();

        float duration = ozz_animation.duration() * 1000.f;

        if( m_time > duration )
        {
            m_time -= duration;

            bool loop = this->isLoop();
            bool interrupt = this->isInterrupt();

            if( (loop == false && --m_playIterator == 0) || interrupt == true )
            {
                m_time = duration;

                this->end();

                return;
            }
            else
            {
                m_time = 0.f;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerOzzAnimation::updateAnimation_()
    {
        // Early out if this sampler weight makes it irrelevant during blending.
        if( m_weight <= 0.f )
        {
            return;
        }

        const ozz::animation::Animation & ozz_animation = m_resourceOzzAnimation->getOzzAnimation();

        // Setup sampling job.
        ozz::animation::SamplingJob sampling_job;
        sampling_job.animation = &ozz_animation;
        sampling_job.cache = m_cache;

        float duration = ozz_animation.duration() * 1000.f;
        float ratio = m_time / duration;
        sampling_job.ratio = ratio;
        sampling_job.output = m_locals;

        // Samples animation.
        if( !sampling_job.Run() )
        {
            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SamplerOzzAnimation::_play( uint32_t _playId, float _time )
    {
        MENGINE_UNUSED( _playId );
        MENGINE_UNUSED( _time );

        m_time = 0.f;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SamplerOzzAnimation::_restart( uint32_t _playId, float _time )
    {
        MENGINE_UNUSED( _playId );
        MENGINE_UNUSED( _time );

        m_time = 0.f;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerOzzAnimation::_pause( uint32_t _playId )
    {
        MENGINE_UNUSED( _playId );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerOzzAnimation::_resume( uint32_t _playId, float _time )
    {
        MENGINE_UNUSED( _playId );
        MENGINE_UNUSED( _time );

        m_time = 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SamplerOzzAnimation::_stop( uint32_t _playId )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_STOP )
            ->onAnimationStop( _playId );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerOzzAnimation::_end( uint32_t _playId )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_END )
            ->onAnimationEnd( _playId );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SamplerOzzAnimation::_interrupt( uint32_t _playId )
    {
        MENGINE_UNUSED( _playId );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float SamplerOzzAnimation::_getDuration() const
    {
        const ozz::animation::Animation & ozz_animation = m_resourceOzzAnimation->getOzzAnimation();

        float duration = ozz_animation.duration() * 1000.f;

        return duration;
    }
}
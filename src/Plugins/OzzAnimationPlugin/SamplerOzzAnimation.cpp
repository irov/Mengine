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
        : m_weight( 1.f )
        , m_time( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SamplerOzzAnimation::~SamplerOzzAnimation()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerOzzAnimation::setResourceOzzAnimation( const ResourcePtr & _resourceOzzAnimation )
    {
        m_resourceOzzAnimation = _resourceOzzAnimation;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & SamplerOzzAnimation::getResourceOzzAnimation() const
    {
        return m_resourceOzzAnimation;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerOzzAnimation::setResourceOzzSkeleton( const ResourcePtr & _resourceOzzSkeleton )
    {
        m_resourceOzzSkeleton = _resourceOzzSkeleton;
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
    const ozz::vector<ozz::math::SoaTransform> & SamplerOzzAnimation::getLocals() const
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

        //if( this->isPlay() == false )
        //{
        //    return;
        //}

        float totalTiming = this->calcTotalTime( _context );

        this->updateTiming_( totalTiming );
        
        if( this->updateAnimation_() == false )
        {
            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SamplerOzzAnimation::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceOzzAnimation, "resource ozz animation is nullptr" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceOzzSkeleton, "resource ozz skeleton is nullptr" );

        if( m_resourceOzzAnimation->compile() == false )
        {
            LOGGER_ERROR( "sampler ozz animation '%s' invalid compile resource ozz animation '%s'"
                , this->getName().c_str()
                , m_resourceOzzAnimation->getName().c_str()
            );

            return false;
        }

        if( m_resourceOzzSkeleton->compile() == false )
        {
            LOGGER_ERROR( "sampler ozz animation '%s' invalid compile resource ozz skeleton '%s'"
                , this->getName().c_str()
                , m_resourceOzzSkeleton->getName().c_str()
            );

            return false;
        }

        const ozz::animation::Skeleton & skeleton = m_resourceOzzSkeleton->getOzzSkeleton();

        int32_t num_soa_joints = skeleton.num_soa_joints();
        int32_t num_joints = skeleton.num_joints();

        m_locals.resize( num_soa_joints );
        m_context.Resize( num_joints );

        if( this->updateAnimation_() == false )
        {
            LOGGER_ERROR( "sampler ozz animation '%s' invalid update animation"
                , this->getName().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerOzzAnimation::_release()
    {
        m_locals.clear();

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
    bool SamplerOzzAnimation::updateAnimation_()
    {
        // Early out if this sampler weight makes it irrelevant during blending.
        if( m_weight <= 0.f )
        {
            return true;
        }

        const ozz::animation::Animation & ozz_animation = m_resourceOzzAnimation->getOzzAnimation();

        float duration = ozz_animation.duration() * 1000.f;
        float ratio = m_time / duration;

        // Setup sampling job.
        ozz::animation::SamplingJob sampling_job;
        sampling_job.animation = &ozz_animation;
        sampling_job.context = &m_context;
        sampling_job.ratio = ratio;
        sampling_job.output = ozz::make_span( m_locals );

        // Samples animation.
        if( sampling_job.Run() == false )
        {
            LOGGER_ERROR( "sampler ozz animation '%s' invalid sampling job"
                , this->getName().c_str()
            );

            return false;
        }

        m_context.Invalidate();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SamplerOzzAnimation::_play( UniqueId _playId, float _time )
    {
        MENGINE_UNUSED( _playId );
        MENGINE_UNUSED( _time );

        m_time = 0.f;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SamplerOzzAnimation::_restart( UniqueId _playId, float _time )
    {
        MENGINE_UNUSED( _playId );
        MENGINE_UNUSED( _time );

        m_time = 0.f;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerOzzAnimation::_pause( UniqueId _playId )
    {
        MENGINE_UNUSED( _playId );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerOzzAnimation::_resume( UniqueId _playId, float _time )
    {
        MENGINE_UNUSED( _playId );
        MENGINE_UNUSED( _time );

        m_time = 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SamplerOzzAnimation::_stop( UniqueId _playId )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_STOP )
            ->onAnimationStop( _playId );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerOzzAnimation::_end( UniqueId _playId )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_END )
            ->onAnimationEnd( _playId );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SamplerOzzAnimation::_interrupt( UniqueId _playId )
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
    //////////////////////////////////////////////////////////////////////////
}
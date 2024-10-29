#include "SamplerSpineAnimation.h"

#include "Interface/TimelineServiceInterface.h"

#include "Kernel/Materialable.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/EventableHelper.h"

#include "Config/Algorithm.h"

#ifndef MENGINE_SPINE_MAX_VERTICES
#define MENGINE_SPINE_MAX_VERTICES 512
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SamplerSpineAnimation::SamplerSpineAnimation()
        : m_animationStateData( nullptr )
        , m_animationState( nullptr )
        , m_animationEnable( true )
        , m_trackEntry( nullptr )
        , m_time( 0.f )
        , m_duration( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SamplerSpineAnimation::~SamplerSpineAnimation()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerSpineAnimation::setResourceSpineSkeleton( const ResourcePtr & _resourceSpineSkeleton )
    {
        if( m_resourceSpineSkeleton == _resourceSpineSkeleton )
        {
            return;
        }

        this->recompile( [this, &_resourceSpineSkeleton]()
        {
            m_resourceSpineSkeleton = _resourceSpineSkeleton;

            if( m_resourceSpineSkeleton == nullptr )
            {
                return false;
            }

            return true;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & SamplerSpineAnimation::getResourceSpineSkeleton() const
    {
        return m_resourceSpineSkeleton;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerSpineAnimation::setAnimationName( const ConstString & _animationName )
    {
        m_animationName = _animationName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & SamplerSpineAnimation::getAnimationName() const
    {
        return m_animationName;
    }
    //////////////////////////////////////////////////////////////////////////
    spAnimationState * SamplerSpineAnimation::getAnimationState() const
    {
        return m_animationState;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerSpineAnimation::setAnimationEnable( bool _animationEnable )
    {
        m_animationEnable = _animationEnable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SamplerSpineAnimation::getAnimationEnable() const
    {
        return m_animationEnable;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void s_spAnimationStateListener( spAnimationState * _state, spEventType _type, spTrackEntry * _entry, spEvent * _event )
        {
            SamplerSpineAnimation * sampler = static_cast<SamplerSpineAnimation *>(_state->rendererObject);

            sampler->addAnimationEvent_( _state, _type, _entry, _event );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SamplerSpineAnimation::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceSpineSkeleton, "'%s' resource is nullptr"
            , this->getName().c_str()
        );

        if( m_resourceSpineSkeleton->compile() == false )
        {
            LOGGER_ERROR( "sampler spine skeleton '%s' resource '%s' is not compile"
                , this->getName().c_str()
                , m_resourceSpineSkeleton->getName().c_str()
            );

            return false;
        }

        spSkeletonData * skeletonData = m_resourceSpineSkeleton->getSkeletonData();

        spAnimationStateData * animationStateData = spAnimationStateData_create( skeletonData );

        MENGINE_ASSERTION_MEMORY_PANIC( animationStateData, "sampler '%s' invalid create animation state data"
            , this->getName().c_str()
        );

        m_animationStateData = animationStateData;

        spAnimation * animation = m_resourceSpineSkeleton->findSkeletonAnimation( m_animationName );

        MENGINE_ASSERTION_MEMORY_PANIC( animation, "sampler '%s' invalid found animation '%s'"
            , this->getName().c_str()
            , m_animationName.c_str()
        );

        spAnimationState * animationState = spAnimationState_create( m_animationStateData );

        animationState->rendererObject = this;
        animationState->listener = &Detail::s_spAnimationStateListener;

        m_animationState = animationState;

        bool loop = this->isLoop();

        spTrackEntry * trackEntry = spAnimationState_setAnimation( m_animationState, 0, animation, loop == true ? 1 : 0 );

        m_trackEntry = trackEntry;

        m_duration = animation->duration * 1000.f;

        if( m_time < 0.f )
        {
            m_time = m_duration;
        }

        float spTime = m_time * 0.001f;
        spAnimationState_update( m_animationState, spTime );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerSpineAnimation::_release()
    {
        spAnimationState_clearTracks( m_animationState );
        spAnimationState_dispose( m_animationState );
        m_animationState = nullptr;

        if( m_animationStateData != nullptr )
        {
            spAnimationStateData_dispose( m_animationStateData );
            m_animationStateData = nullptr;
        }

        m_resourceSpineSkeleton->release();

        m_events.clear();
        m_eventsAux.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    float SamplerSpineAnimation::_getDuration() const
    {
        return m_duration;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerSpineAnimation::_setLoop( bool _loop )
    {
        if( this->isCompile() == false )
        {
            return;
        }

        m_trackEntry->loop = _loop == true ? 1 : 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerSpineAnimation::_setTime( float _time )
    {
        if( mt::equal_f_f( m_time, _time ) == true )
        {
            return;
        }

        m_time = _time;

        if( this->isCompile() == false )
        {
            return;
        }

        m_trackEntry->trackTime = m_time;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerSpineAnimation::_setFirstFrame()
    {
        m_time = 0.f;

        if( this->isCompile() == false )
        {
            return;
        }

        m_trackEntry->trackTime = 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerSpineAnimation::_setLastFrame()
    {
        m_time = -1.f;

        if( this->isCompile() == false )
        {
            return;
        }

        m_trackEntry->trackTime = m_trackEntry->trackEnd;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerSpineAnimation::addAnimationEvent_( spAnimationState * _animationState, spEventType _type, spTrackEntry * _entry, spEvent * _event )
    {
        MENGINE_UNUSED( _animationState );

        bool loop = this->isLoop();

        AnimationEventDesc ev;

        ev.trackIndex = _entry->trackIndex;
        ev.type = _type;
        ev.loopCount = _entry->loop;

        switch( _type )
        {
        case SP_ANIMATION_START:
        case SP_ANIMATION_END:
        case SP_ANIMATION_COMPLETE:
            {
                ev.eventName = nullptr;
                ev.eventIntValue = 0;
                ev.eventFloatValue = 0.f;
                ev.eventStringValue = nullptr;

                if( loop == true )
                {
                    return;
                }

                m_time = m_duration;
            }break;
        case SP_ANIMATION_EVENT:
            {
                ev.eventName = _event->data->name;
                ev.eventIntValue = _event->intValue;
                ev.eventFloatValue = _event->floatValue;
                ev.eventStringValue = _event->stringValue != nullptr ? _event->stringValue : "";
            }break;
        default:
            {
                return;
            }break;
        }

        m_events.push_back( ev );
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerSpineAnimation::update( const UpdateContext * _context )
    {
        if( this->isPlay() == false )
        {
            return;
        }

        float totalTime = this->calcTotalTime( _context );

        float spTime = totalTime * 0.001f;

        if( m_duration != 0.f )
        {
            m_time += spTime;

            while( m_time >= m_duration )
            {
                m_time -= m_duration;
            }
        }
        else
        {
            m_time = 0.f;
        }

        spAnimationState_update( m_animationState, spTime );

        m_eventsAux = std::move( m_events );

        bool loop = this->isLoop();

        for( const AnimationEventDesc & desc : m_eventsAux )
        {
            switch( desc.type )
            {
            case SP_ANIMATION_COMPLETE:
                {
                    const ConstString & name = this->getName();

                    EVENTABLE_METHOD( EVENT_SAMPLER_SPINE_STATE_ANIMATION_END )
                        ->onSamplerSpineStateAnimationEnd( name, m_animationName, true );

                    if( loop == false )
                    {
                        this->end();
                    }
                }break;
            case SP_ANIMATION_EVENT:
                {
                    EVENTABLE_METHOD( EVENT_SAMPLER_SPINE_EVENT )
                        ->onSamplerSpineEvent( desc.eventName, desc.eventIntValue, desc.eventFloatValue, desc.eventStringValue );
                }break;
            default:
                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SamplerSpineAnimation::_play( uint32_t _enumerator, float _time )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_PLAY )
            ->onAnimationPlay( _enumerator, _time );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SamplerSpineAnimation::_restart( uint32_t _enumerator, float _time )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_RESTART )
            ->onAnimationRestart( _enumerator, _time );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerSpineAnimation::_pause( uint32_t _enumerator )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_PAUSE )
            ->onAnimationPause( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerSpineAnimation::_resume( uint32_t _enumerator, float _time )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_RESUME )
            ->onAnimationResume( _enumerator, _time );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SamplerSpineAnimation::_stop( uint32_t _enumerator )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_STOP )
            ->onAnimationStop( _enumerator );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SamplerSpineAnimation::_end( uint32_t _enumerator )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_END )
            ->onAnimationEnd( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SamplerSpineAnimation::_interrupt( uint32_t _enumerator )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_INTERRUPT )
            ->onAnimationInterrupt( _enumerator );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
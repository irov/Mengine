#include "Movie2SubComposition.h"
#include "Movie2.h"

#include "Kernel/Logger.h"
#include "Kernel/EventableHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Movie2SubComposition::Movie2SubComposition()
        : m_movie( nullptr )
        , m_duration( 0.f )
        , m_frameDuration( 0.f )
        , m_composition( nullptr )
        , m_subcomposition( nullptr )
        , m_startTime( -1.f )
        , m_enable( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2SubComposition::~Movie2SubComposition()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::setMovie( const Movie2 * _movie )
    {
        m_movie = _movie;
    }
    //////////////////////////////////////////////////////////////////////////
    const Movie2 * Movie2SubComposition::getMovie() const
    {
        return m_movie;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::setDuration( float _duration )
    {
        m_duration = _duration;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::setFrameDuration( float _frameDuration )
    {
        m_frameDuration = _frameDuration;
    }
    //////////////////////////////////////////////////////////////////////////
    float Movie2SubComposition::getFrameDuration() const
    {
        return m_frameDuration;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::setSubCompositionEnable( bool _enable )
    {
        m_enable = _enable;

        if( m_subcomposition == nullptr )
        {
            return;
        }

        this->updateEnable_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2SubComposition::getSubCompositionEnable() const
    {
        return m_enable;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::updateEnable_()
    {
        ae_set_movie_sub_composition_enable( m_subcomposition, m_enable == true ? AE_TRUE : AE_FALSE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2SubComposition::initialize( const aeMovieComposition * _composition )
    {
        const ConstString & name = this->getName();

        const aeMovieSubComposition * subcomposition = ae_get_movie_sub_composition( _composition, name.c_str() );

        if( subcomposition == AE_NULLPTR )
        {
            return false;
        }

        m_composition = _composition;
        m_subcomposition = subcomposition;

        this->updateEnable_();
        this->updateLoop_();
        this->updateTime_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::finalize()
    {
        m_composition = nullptr;
        m_subcomposition = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2SubComposition::_play( uint32_t _playId, float _time )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_composition, "subcomposition '%s' is not initialized"
            , this->getName().c_str()
        );

        ae_play_movie_sub_composition( m_composition, m_subcomposition, 0.f );

        EVENTABLE_METHOD( EVENT_ANIMATION_PLAY )
            ->onAnimationPlay( _playId, _time );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2SubComposition::_restart( uint32_t _playId, float _time )
    {
        if( m_composition == nullptr )
        {
            LOGGER_ERROR( "movie2 subcomposition '%s' is not initialized"
                , this->getName().c_str()
            );

            return false;
        }

        EVENTABLE_METHOD( EVENT_ANIMATION_RESTART )
            ->onAnimationRestart( _playId, _time );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_pause( uint32_t _playId )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_composition, "subcomposition '%s' is not initialized"
            , this->getName().c_str()
        );

        ae_pause_movie_sub_composition( m_composition, m_subcomposition );

        EVENTABLE_METHOD( EVENT_ANIMATION_PAUSE )
            ->onAnimationPause( _playId );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_resume( uint32_t _playId, float _time )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_composition, "subcomposition '%s' is not initialized"
            , this->getName().c_str()
        );

        ae_resume_movie_sub_composition( m_composition, m_subcomposition );

        EVENTABLE_METHOD( EVENT_ANIMATION_RESUME )
            ->onAnimationResume( _playId, _time );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2SubComposition::_stop( uint32_t _playId )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_composition, "subcomposition '%s' is not initialized"
            , this->getName().c_str()
        );

        ae_stop_movie_sub_composition( m_composition, m_subcomposition );

        EVENTABLE_METHOD( EVENT_ANIMATION_STOP )
            ->onAnimationStop( _playId );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_end( uint32_t _playId )
    {
        EVENTABLE_METHOD( EVENT_ANIMATION_END )
            ->onAnimationEnd( _playId );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2SubComposition::_interrupt( uint32_t _playId )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_composition, "subcomposition '%s' is not initialized"
            , this->getName().c_str()
        );

        ae_interrupt_movie_sub_composition( m_composition, m_subcomposition, AE_FALSE );

        EVENTABLE_METHOD( EVENT_ANIMATION_INTERRUPT )
            ->onAnimationInterrupt( _playId );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_setLoop( bool _value )
    {
        MENGINE_UNUSED( _value );

        if( m_subcomposition == nullptr )
        {
            return;
        }

        this->updateLoop_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_setTime( float _time )
    {
        m_startTime = _time;

        if( m_subcomposition == nullptr )
        {
            return;
        }

        this->updateTime_();
    }
    //////////////////////////////////////////////////////////////////////////
    float Movie2SubComposition::_getTime() const
    {
        if( m_subcomposition == nullptr )
        {
            return m_startTime;
        }

        float timing = ae_get_movie_sub_composition_time( m_subcomposition );

        return AE_TIME_MILLISECOND( timing );
    }
    //////////////////////////////////////////////////////////////////////////
    float Movie2SubComposition::_getDuration() const
    {
        return m_duration;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_setFirstFrame()
    {
        this->setTime( 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_setLastFrame()
    {
        this->setTime( m_duration - m_frameDuration );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::updateLoop_()
    {
        AnimationInterface * animation = this->getAnimation();
        bool loop = animation->calcTotalLoop();

        ae_set_movie_sub_composition_loop( m_subcomposition, loop == true ? AE_TRUE : AE_FALSE );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::updateTime_()
    {
        if( m_startTime < 0.f )
        {
            return;
        }

        ae_set_movie_sub_composition_time( m_composition, m_subcomposition, m_startTime * 0.001f );
    }
    //////////////////////////////////////////////////////////////////////////
}

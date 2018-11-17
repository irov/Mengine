#include "Movie2SubComposition.h"
#include "Movie2.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Movie2SubComposition::Movie2SubComposition()
        : m_composition( nullptr )
        , m_subcomposition( nullptr )
        , m_duration( 0.f )
        , m_frameDuration( 0.f )
        , m_enable( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2SubComposition::~Movie2SubComposition()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::setMovie( const Movie2Ptr & _movie )
    {
        m_movie = _movie;
    }
    //////////////////////////////////////////////////////////////////////////
    const Movie2Ptr & Movie2SubComposition::getMovie() const
    {
        return m_movie;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::setDuration( float _duration )
    {
        m_duration = _duration;
    }
    //////////////////////////////////////////////////////////////////////////
    float Movie2SubComposition::getDuration() const
    {
        return m_duration;
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
    void Movie2SubComposition::setEnable( bool _enable )
    {
        m_enable = _enable;

        if( m_subcomposition == nullptr )
        {
            return;
        }

        this->updateEnable_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2SubComposition::getEnable() const
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

        if( subcomposition == AE_NULL )
        {
            return false;
        }

        m_composition = _composition;
        m_subcomposition = subcomposition;

        this->updateEnable_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2SubComposition::_play( uint32_t _enumerator, float _time )
    {
        ae_play_movie_sub_composition( m_composition, m_subcomposition, 0.f );

        EVENTABLE_METHOD( this, EVENT_ANIMATION_PLAY )
            ->onAnimationPlay( _enumerator, _time );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2SubComposition::_restart( uint32_t _enumerator, float _time )
    {
        EVENTABLE_METHOD( this, EVENT_ANIMATION_RESTART )
            ->onAnimationRestart( _enumerator, _time );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_pause( uint32_t _enumerator )
    {
        ae_pause_movie_sub_composition( m_composition, m_subcomposition );

        EVENTABLE_METHOD( this, EVENT_ANIMATION_PAUSE )
            ->onAnimationPause( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_resume( uint32_t _enumerator, float _time )
    {
        ae_resume_movie_sub_composition( m_composition, m_subcomposition );

        EVENTABLE_METHOD( this, EVENT_ANIMATION_RESUME )
            ->onAnimationResume( _enumerator, _time );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_stop( uint32_t _enumerator )
    {
        ae_stop_movie_sub_composition( m_composition, m_subcomposition );

        EVENTABLE_METHOD( this, EVENT_ANIMATION_STOP )
            ->onAnimationStop( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_end( uint32_t _enumerator )
    {
        EVENTABLE_METHOD( this, EVENT_ANIMATION_END )
            ->onAnimationEnd( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2SubComposition::_interrupt( uint32_t _enumerator )
    {
        ae_interrupt_movie_sub_composition( m_composition, m_subcomposition, AE_FALSE );

        EVENTABLE_METHOD( this, EVENT_ANIMATION_INTERRUPT )
            ->onAnimationInterrupt( _enumerator );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_setLoop( bool _value )
    {
        ae_set_movie_sub_composition_loop( m_subcomposition, _value == true ? AE_TRUE : AE_FALSE );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_setTime( float _time )
    {
        if( m_subcomposition == nullptr )
        {
            LOGGER_ERROR( "submovie '%s' invalid initialized"
                , this->getName().c_str()
            );

            return;
        }

        ae_set_movie_sub_composition_time( m_composition, m_subcomposition, _time * 0.001f );
    }
    //////////////////////////////////////////////////////////////////////////
    float Movie2SubComposition::_getTime() const
    {
        if( m_subcomposition == nullptr )
        {
            LOGGER_ERROR( "submovie '%s' not initialized"
                , this->getName().c_str()
            );

            return 0.f;
        }

        float timing = ae_get_movie_sub_composition_time( m_subcomposition );

        return timing * 1000.f;
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

}

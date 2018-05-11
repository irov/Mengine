#include "Movie2SubComposition.h"
#include "Movie2.h"

#include "Interface/RenderSystemInterface.h"

#include "Logger/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Movie2SubComposition::Movie2SubComposition()
        : m_movie( nullptr )
        , m_composition( nullptr )
        , m_subcomposition( nullptr )
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
    bool Movie2SubComposition::_play( uint32_t _enumerator, float _time )
    {
        ae_play_movie_sub_composition( m_composition, m_subcomposition, 0.f );

        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_PLAY )
            ->onAnimatablePlay( _enumerator, _time );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2SubComposition::_restart( uint32_t _enumerator, float _time )
    {
        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_RESTART )
            ->onAnimatableRestart( _enumerator, _time );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_pause( uint32_t _enumerator )
    {
        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_PAUSE )
            ->onAnimatablePause( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_resume( uint32_t _enumerator, float _time )
    {
        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_RESUME )
            ->onAnimatableResume( _enumerator, _time );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_stop( uint32_t _enumerator )
    {
        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_STOP )
            ->onAnimatableStop( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_end( uint32_t _enumerator )
    {
        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_END )
            ->onAnimatableEnd( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2SubComposition::_interrupt( uint32_t _enumerator )
    {
        ae_interrupt_movie_sub_composition( m_composition, m_subcomposition, AE_FALSE );

        EVENTABLE_METHOD( this, EVENT_ANIMATABLE_INTERRUPT )
            ->onAnimatableInterrupt( _enumerator );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}

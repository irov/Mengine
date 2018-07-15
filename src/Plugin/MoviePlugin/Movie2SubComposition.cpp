#include "Movie2SubComposition.h"
#include "Movie2.h"

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
    void Movie2SubComposition::setSubMovieComposition( const aeMovieComposition * _composition, const ConstString & _name )
    {
        m_composition = _composition;

        const aeMovieSubComposition * subcomposition = ae_get_movie_sub_composition( m_composition, _name.c_str() );

        m_subcomposition = subcomposition;
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
        EVENTABLE_METHOD( this, EVENT_ANIMATION_PAUSE )
            ->onAnimationPause( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_resume( uint32_t _enumerator, float _time )
    {
        EVENTABLE_METHOD( this, EVENT_ANIMATION_RESUME )
            ->onAnimationResume( _enumerator, _time );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::_stop( uint32_t _enumerator )
    {
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
        ae_set_movie_sub_composition_loop( m_subcomposition, _value );
    }
    //////////////////////////////////////////////////////////////////////////
}

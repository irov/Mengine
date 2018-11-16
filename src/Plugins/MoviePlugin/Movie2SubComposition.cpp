#include "Movie2SubComposition.h"
#include "Movie2.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Movie2SubComposition::Movie2SubComposition()
        : m_composition( nullptr )
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
    void Movie2SubComposition::setSubMovieCompositionName( const ConstString & _subcompositionName )
    {
        m_subcompositionName = _subcompositionName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Movie2SubComposition::getSubMovieCompositionName() const
    {
        return m_subcompositionName;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2SubComposition::setEnable( bool _enable )
    {
        ae_set_movie_sub_composition_enable( m_subcomposition, _enable == true ? AE_TRUE : AE_FALSE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2SubComposition::getEnable() const
    {
        ae_bool_t enable = ae_get_movie_sub_composition_enable( m_subcomposition );

        return enable == AE_TRUE ? true : false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2SubComposition::initialize( const aeMovieComposition * _composition )
    {
        const aeMovieSubComposition * subcomposition = ae_get_movie_sub_composition( _composition, m_subcompositionName.c_str() );

        if( subcomposition == AE_NULL )
        {
            return false;
        }

        m_composition = _composition;
        m_subcomposition = subcomposition;

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
}

#pragma once

#include "Factory/FactorablePtr.h"
#include "Kernel/Eventable.h"
#include "Kernel/Animatable.h"

#include "movie/movie.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Movie2;
    //////////////////////////////////////////////////////////////////////////
    class Movie2SubCompositionEventReceiver
        : public AnimatableEventReceiver
    {
    public:
    };
    //////////////////////////////////////////////////////////////////////////
	class Movie2SubComposition
        : public FactorablePtr
		, public Eventable
        , public Animatable
	{
        EVENT_RECEIVER( Movie2SubCompositionEventReceiver );

    public:
        Movie2SubComposition();
        ~Movie2SubComposition() override;

    public:
        void setMovie( Movie2 * _movie );
        Movie2 * getMovie() const;
        
    protected:
        bool _play( uint32_t _enumerator, float _time ) override;
        bool _restart( uint32_t _enumerator, float _time ) override;
        void _pause( uint32_t _enumerator ) override;
        void _resume( uint32_t _enumerator, float _time ) override;
        void _stop( uint32_t _enumerator ) override;
        void _end( uint32_t _enumerator ) override;
        bool _interrupt( uint32_t _enumerator ) override;

    protected:
        Movie2 * m_movie;

        const aeMovieComposition * m_composition;
        const aeMovieSubComposition * m_subcomposition;
	};
}

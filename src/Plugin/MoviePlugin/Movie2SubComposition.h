#pragma once

#include "Kernel/Animatable.h"
#include "Kernel/AnimationEventReceiver.h"
#include "Kernel/BaseAnimation.h"
#include "Kernel/Eventable.h"

#include "Factory/Factorable.h"

#include "movie/movie.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Movie2> Movie2Ptr;
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Movie2SubComposition> Movie2SubCompositionPtr;
    //////////////////////////////////////////////////////////////////////////
    class Movie2SubCompositionEventReceiver
        : public AnimationEventReceiver
    {
    public:
    };
    //////////////////////////////////////////////////////////////////////////
	class Movie2SubComposition
        : public Factorable
		, public Eventable
        , public Animatable
        , public BaseAnimation
	{
        DECLARE_ANIMATABLE()

        EVENT_RECEIVER( Movie2SubCompositionEventReceiver );
        
    public:
        Movie2SubComposition();
        ~Movie2SubComposition() override;

    public:
        void setMovie( const Movie2Ptr & _movie );
        const Movie2Ptr & getMovie() const;

    public:
        void setSubMovieComposition( const aeMovieComposition * _composition, const ConstString & _name );
        
    protected:
        bool _play( uint32_t _enumerator, float _time ) override;
        bool _restart( uint32_t _enumerator, float _time ) override;
        void _pause( uint32_t _enumerator ) override;
        void _resume( uint32_t _enumerator, float _time ) override;
        void _stop( uint32_t _enumerator ) override;
        void _end( uint32_t _enumerator ) override;
        bool _interrupt( uint32_t _enumerator ) override;

    protected:
        void _setLoop( bool _value ) override;

    protected:
        Movie2Ptr m_movie;

        const aeMovieComposition * m_composition;
        const aeMovieSubComposition * m_subcomposition;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie2SubComposition> Movie2SubCompositionPtr;
    //////////////////////////////////////////////////////////////////////////
}

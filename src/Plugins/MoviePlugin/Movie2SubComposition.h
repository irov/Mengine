#pragma once

#include "Kernel/Scriptable.h"
#include "Kernel/Animatable.h"
#include "Kernel/AnimationEventReceiver.h"
#include "Kernel/BaseAnimation.h"
#include "Kernel/Eventable.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/Factorable.h"

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
        , public BaseEventation
        , public Animatable
        , public BaseAnimation
        , public Scriptable
    {
        DECLARE_ANIMATABLE()
        DECLARE_EVENTABLE( Movie2SubCompositionEventReceiver );

    public:
        Movie2SubComposition();
        ~Movie2SubComposition() override;

    public:
        void setMovie( const Movie2Ptr & _movie );
        const Movie2Ptr & getMovie() const;

    public:
        void setSubMovieCompositionName( const ConstString & _subcompositionName );
        const ConstString & getSubMovieCompositionName() const;

	public:
        void setEnable( bool _enable );
        bool getEnable() const;

    public:
        bool initialize( const aeMovieComposition * _composition );

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
        ConstString m_subcompositionName;

        const aeMovieComposition * m_composition;
        const aeMovieSubComposition * m_subcomposition;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie2SubComposition> Movie2SubCompositionPtr;
    //////////////////////////////////////////////////////////////////////////
}

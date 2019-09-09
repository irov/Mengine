#pragma once

#include "Kernel/Identity.h"
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
        : public AnimationEventReceiverInterface
    {
    public:
    };
    //////////////////////////////////////////////////////////////////////////
    class Movie2SubComposition
        : public Factorable
        , public Identity
        , public Eventable
        , public BaseEventation
        , public Animatable
        , public BaseAnimation
        , public Scriptable
    {
        DECLARE_ANIMATABLE();
        DECLARE_EVENTABLE();

    public:
        Movie2SubComposition();
        ~Movie2SubComposition() override;

    public:
        void setMovie( const Movie2 * _movie );
        const Movie2 * getMovie() const;

        void setDuration( float _duration );


        void setFrameDuration( float _frameDuration );
        float getFrameDuration() const;

    public:
        void setEnable( bool _enable );
        bool getEnable() const;

    protected:
        void updateEnable_();

    public:
        bool initialize( const aeMovieComposition * _composition );
        void finalize();

    protected:
        bool _play( uint32_t _playId, float _time ) override;
        bool _restart( uint32_t _playId, float _time ) override;
        void _pause( uint32_t _playId ) override;
        void _resume( uint32_t _playId, float _time ) override;
        bool _stop( uint32_t _playId ) override;
        void _end( uint32_t _playId ) override;
        bool _interrupt( uint32_t _playId ) override;

    protected:
        void _setLoop( bool _value ) override;
        void _setTime( float _time ) override;
        float _getTime() const override;
        float _getDuration() const override;
        void _setFirstFrame() override;
        void _setLastFrame() override;

    protected:
        void updateLoop_();
        void updateTime_();

    protected:
        const Movie2 * m_movie;

        float m_duration;
        float m_frameDuration;

        const aeMovieComposition * m_composition;
        const aeMovieSubComposition * m_subcomposition;

        float m_startTime;

        bool m_enable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie2SubComposition> Movie2SubCompositionPtr;
    //////////////////////////////////////////////////////////////////////////
}

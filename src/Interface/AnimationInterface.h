#pragma once

#include "Interface/Interface.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Animatable;
    //////////////////////////////////////////////////////////////////////////
    class AnimationInterface
        : public Interface
    {
    public:
        virtual Animatable * getAnimatable() = 0;

    public:
        virtual void setRelationAnimation( const AnimationInterface * _relationAnimation ) = 0;
        virtual const AnimationInterface * getRelationAnimation() const = 0;

        virtual void setRelationIncessantly( bool _relationIncessantly ) = 0;
        virtual bool getRelationIncessantly() const = 0;

    public:
        virtual void setLoop( bool _value ) = 0;
        virtual bool isLoop() const = 0;

        virtual void setAnimationSpeedFactor( float _factor ) = 0;
        virtual float getAnimationSpeedFactor() const = 0;

        virtual void setIntervalStart( float _startInterval ) = 0;
        virtual float getIntervalStart() const = 0;

        virtual void setInterval( float _begin, float _end ) = 0;
        virtual float getIntervalBegin() const = 0;
        virtual float getIntervalEnd() const = 0;

        virtual void setFirstFrame() = 0;
        virtual void setLastFrame() = 0;

        virtual float getAdaptTime( float _time ) const = 0;

        virtual void setTime( float _time ) = 0;
        virtual float getTime() const = 0;

        virtual float getDuration() const = 0;

        virtual void setPlayCount( uint32_t _count ) = 0;
        virtual uint32_t getPlayCount() const = 0;

        virtual uint32_t getPlayIterator() const = 0;

        virtual void setAutoPlay( bool _autoPlay ) = 0;
        virtual bool isAutoPlay() const = 0;

        virtual void setStretch( float _stretch ) = 0;
        virtual float getStretch() const = 0;

    public:
        virtual UniqueId play( float _time ) = 0;
        virtual bool stop() = 0;
        virtual bool pause() = 0;
        virtual void resume( float _time ) = 0;
        virtual bool interrupt() = 0;
        virtual void end() = 0;

    public:
        virtual bool isPlay() const = 0;
        virtual bool isPause() const = 0;

    public:
        virtual bool isInterrupt() const = 0;

    public:
        virtual bool isLastCicle() const = 0;

    public:
        virtual UniqueId getPlayId() const = 0;
        virtual float getPlayTime() const = 0;

    public:
        virtual bool calcTotalLoop() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnimationInterface> AnimationInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
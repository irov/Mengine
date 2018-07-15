#pragma once

#include "Core/Mixin.h"
#include "Core/Pointer.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnimationInterface
        : public Mixin
    {
    public:
        virtual void setLoop( bool _value ) = 0;
        virtual bool getLoop() const = 0;

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

        virtual void setPlayCount( uint32_t _count ) = 0;
        virtual uint32_t getPlayCount() const = 0;

        virtual uint32_t getPlayIterator() const = 0;

        virtual void setAutoPlay( bool _autoPlay ) = 0;
        virtual bool getAutoPlay() const = 0;

        virtual void setStretch( float _scretch ) = 0;
        virtual float getStretch() const = 0;

    public:
        virtual uint32_t play( float _time ) = 0;
        virtual bool stop() = 0;
        virtual bool pause() = 0;
        virtual void resume( float _time ) = 0;
        virtual bool interrupt() = 0;

    public:
        virtual bool isPlay() const = 0;
        virtual bool isPause() const = 0;

    public:
        virtual bool isInterrupt() const = 0;

    public:
        virtual bool isLastCicle() const = 0;

    public:
        virtual uint32_t getPlayId() const = 0;
        virtual float getPlayTime() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnimationInterface> AnimationInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
#pragma once

#include "Interface/AnimationInterface.h"

#include "Kernel/UpdateContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BaseAnimation
        : public AnimationInterface
    {
    public:
        BaseAnimation();
        ~BaseAnimation() override;

    public:
        void setRelationAnimation( const AnimationInterface * _relationAnimation ) override final;
        const AnimationInterface * getRelationAnimation() const override final;

        void setRelationIncessantly( bool _relationIncessantly ) override final;
        bool getRelationIncessantly() const override final;

    public:
        void setLoop( bool _value ) override final;
        bool isLoop() const override final;

        void setAnimationSpeedFactor( float _factor ) override final;
        float getAnimationSpeedFactor() const override final;

        void setIntervalStart( float _startInterval ) override final;
        float getIntervalStart() const override final;

        void setInterval( float _begin, float _end ) override final;
        float getIntervalBegin() const override final;
        float getIntervalEnd() const override final;

        void setFirstFrame() override final;
        void setLastFrame() override final;

        float getAdaptTime( float _time ) const override final;

        void setTime( float _time ) override final;
        float getTime() const override final;

        float getDuration() const override final;

        void setPlayCount( uint32_t _count ) override final;
        uint32_t getPlayCount() const override final;

        uint32_t getPlayIterator() const override final;

        void setAutoPlay( bool _autoPlay ) override final;
        bool isAutoPlay() const override final;

        void setStretch( float _stretch ) override final;
        float getStretch() const override final;

    public:
        UniqueId play( float _time ) override final;
        bool stop() override final;
        bool pause() override final;
        void resume( float _time ) override final;
        bool interrupt() override final;
        void end() override final;

    public:
        bool isPlay() const override final;
        bool isPause() const override final;

    public:
        bool isInterrupt() const override final;

    public:
        bool isLastCicle() const override final;

    public:
        UniqueId getPlayId() const override final;
        float getPlayTime() const override final;

    public:
        bool calcTotalLoop() const override final;

    protected:
        float calcTotalTime( const UpdateContext * _context ) const;        

    protected:
        virtual void _setLoop( bool _value );
        virtual void _setTime( float _time );
        virtual float _getTime() const;
        virtual float _getDuration() const;

        virtual void _setAnimationSpeedFactor( float _factor );
        virtual void _setIntervalStart( float _startInterval );
        virtual void _setInterval( float _begin, float _end );
        virtual void _setFirstFrame();
        virtual void _setLastFrame();
        virtual void _setPlayCount( uint32_t _count );
        virtual void _setAutoPlay( bool _autoPlay );
        virtual void _setStretch( float _stretch );

    protected:
        virtual bool _play( UniqueId _enumerator, float _time ) = 0;
        virtual bool _restart( UniqueId _enumerator, float _time ) = 0;
        virtual void _pause( UniqueId _enumerator ) = 0;
        virtual void _resume( UniqueId _enumerator, float _time ) = 0;
        virtual bool _stop( UniqueId _enumerator ) = 0;
        virtual void _end( UniqueId _enumerator ) = 0;
        virtual bool _interrupt( UniqueId _enumerator ) = 0;

    protected:
        const AnimationInterface * m_relationAnimation;

        UniqueId m_playId;

        float m_playTime;

        float m_animationSpeedFactor;
        float m_intervalStart;
        float m_intervalBegin;
        float m_intervalEnd;

        uint32_t m_playCount;
        uint32_t m_playIterator;
        float m_stretch;

        bool m_relationIncessantly;
        bool m_autoPlay;

        bool m_play;
        bool m_pause;
        bool m_interrupt;

        bool m_loop;        
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BaseAnimation::isPlay() const
    {
        return m_play == true && m_pause == false;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BaseAnimation::isPause() const
    {
        return m_pause;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BaseAnimation::isLoop() const
    {
        return m_loop;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE float BaseAnimation::getAnimationSpeedFactor() const
    {
        return m_animationSpeedFactor;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t BaseAnimation::getPlayCount() const
    {
        return m_playCount;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t BaseAnimation::getPlayIterator() const
    {
        return m_playIterator;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE float BaseAnimation::getStretch() const
    {
        return m_stretch;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BaseAnimation::isInterrupt() const
    {
        return m_interrupt;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BaseAnimation::isLastCicle() const
    {
        if( m_interrupt == true )
        {
            return true;
        }

        if( m_loop == true )
        {
            return false;
        }

        if( m_playIterator > 1 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE UniqueId BaseAnimation::getPlayId() const
    {
        return m_playId;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE float BaseAnimation::getPlayTime() const
    {
        return m_playTime;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE float BaseAnimation::getIntervalStart() const
    {
        return m_intervalStart;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE float BaseAnimation::getIntervalBegin() const
    {
        return m_intervalBegin;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE float BaseAnimation::getIntervalEnd() const
    {
        return m_intervalEnd;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BaseAnimation::isAutoPlay() const
    {
        return m_autoPlay;
    }
    //////////////////////////////////////////////////////////////////////////
}
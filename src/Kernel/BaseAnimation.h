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
        void setLoop( bool _value ) override;
        bool isLoop() const override;

        void setAnimationSpeedFactor( float _factor ) override;
        float getAnimationSpeedFactor() const override;

        void setIntervalStart( float _startInterval ) override;
        float getIntervalStart() const override;

        void setInterval( float _begin, float _end ) override;
        float getIntervalBegin() const override;
        float getIntervalEnd() const override;

        void setFirstFrame() override;
        void setLastFrame() override;

        float getAdaptTime( float _time ) const override;

        void setTime( float _time ) override;
        float getTime() const override;

        float getDuration() const override;

        void setPlayCount( uint32_t _count ) override;
        uint32_t getPlayCount() const override;

        uint32_t getPlayIterator() const override;

        void setAutoPlay( bool _autoPlay ) override;
        bool isAutoPlay() const override;

        void setStretch( float _scretch ) override;
        float getStretch() const override;

    public:
        uint32_t play( float _time ) override;
        bool stop() override;
        bool pause() override;
        void resume( float _time ) override;
        bool interrupt() override;

    public:
        bool isPlay() const override;
        bool isPause() const override;

    public:
        bool isInterrupt() const override;

    public:
        bool isLastCicle() const override;

    public:
        uint32_t getPlayId() const override;
        float getPlayTime() const override;

    protected:
        float calcTotalTime( const UpdateContext * _context ) const;

    public:
        void end();

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
        virtual void _setStretch( float _scretch );

    protected:
        virtual bool _play( uint32_t _enumerator, float _time ) = 0;
        virtual bool _restart( uint32_t _enumerator, float _time ) = 0;
        virtual void _pause( uint32_t _enumerator ) = 0;
        virtual void _resume( uint32_t _enumerator, float _time ) = 0;
        virtual bool _stop( uint32_t _enumerator ) = 0;
        virtual void _end( uint32_t _enumerator ) = 0;
        virtual bool _interrupt( uint32_t _enumerator ) = 0;

    protected:
        uint32_t m_playId;

        float m_playTime;

        float m_animationSpeedFactor;
        float m_intervalStart;
        float m_intervalBegin;
        float m_intervalEnd;

        uint32_t m_playCount;
        uint32_t m_playIterator;
        float m_stretch;

        bool m_autoPlay;

        bool m_play;
        bool m_pause;
        bool m_interrupt;

        bool m_loop;
    };
    //////////////////////////////////////////////////////////////////////////
    inline bool BaseAnimation::isPlay() const
    {
        return m_play == true && m_pause == false;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool BaseAnimation::isPause() const
    {
        return m_pause;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool BaseAnimation::isLoop() const
    {
        return m_loop;
    }
    //////////////////////////////////////////////////////////////////////////
    inline float BaseAnimation::getAnimationSpeedFactor() const
    {
        return m_animationSpeedFactor;
    }
    //////////////////////////////////////////////////////////////////////////
    inline uint32_t BaseAnimation::getPlayCount() const
    {
        return m_playCount;
    }
    //////////////////////////////////////////////////////////////////////////
    inline uint32_t BaseAnimation::getPlayIterator() const
    {
        return m_playIterator;
    }
    //////////////////////////////////////////////////////////////////////////
    inline float BaseAnimation::getStretch() const
    {
        return m_stretch;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool BaseAnimation::isInterrupt() const
    {
        return m_interrupt;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool BaseAnimation::isLastCicle() const
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
    inline uint32_t BaseAnimation::getPlayId() const
    {
        return m_playId;
    }
    //////////////////////////////////////////////////////////////////////////
    inline float BaseAnimation::getPlayTime() const
    {
        return m_playTime;
    }
    //////////////////////////////////////////////////////////////////////////
    inline float BaseAnimation::getIntervalStart() const
    {
        return m_intervalStart;
    }
    //////////////////////////////////////////////////////////////////////////
    inline float BaseAnimation::getIntervalBegin() const
    {
        return m_intervalBegin;
    }
    //////////////////////////////////////////////////////////////////////////
    inline float BaseAnimation::getIntervalEnd() const
    {
        return m_intervalEnd;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool BaseAnimation::isAutoPlay() const
    {
        return m_autoPlay;
    }
}
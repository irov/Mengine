#include "Kernel/BaseAnimation.h"

#include "Interface/UpdationInterface.h"

#include "math/utils.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseAnimation::BaseAnimation()
        : m_enumerator( 0 )
        , m_playTime( 0.f )
        , m_animationSpeedFactor( 1.f )
        , m_intervalStart( 0.f )
        , m_intervalBegin( 0.f )
        , m_intervalEnd( -1.f )
        , m_playCount( 1 )
        , m_playIterator( 0 )
        , m_stretch( 1.f )
        , m_autoPlay( false )
        , m_play( false )
        , m_pause( false )
        , m_interrupt( false )
        , m_loop( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BaseAnimation::~BaseAnimation()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::setLoop( bool _value )
    {
        if( m_loop == _value )
        {
            return;
        }

        m_loop = _value;

        this->_setLoop( _value );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::setAnimationSpeedFactor( float _factor )
    {
        if( mt::equal_f_f( m_animationSpeedFactor, _factor ) == true )
        {
            return;
        }

        m_animationSpeedFactor = _factor;

        this->_setAnimationSpeedFactor( _factor );
    }
    //////////////////////////////////////////////////////////////////////////
    float BaseAnimation::getAdaptTime( float _time ) const
    {
        float adaptTime = m_intervalStart + m_intervalBegin + _time;

        return adaptTime;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::setTime( float _time )
    {
        float adaptTime = this->getAdaptTime( _time );

        this->_setTime( adaptTime );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::_setTime( float _time )
    {
        (void)_time;
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::setLastFrame()
    {
        this->_setLastFrame();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::_setLastFrame()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::setFirstFrame()
    {
        this->_setFirstFrame();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::_setFirstFrame()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    float BaseAnimation::getTime() const
    {
        float time = this->_getTime();

        return time;
    }
    //////////////////////////////////////////////////////////////////////////
    float BaseAnimation::_getTime() const
    {
        return 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::setPlayCount( uint32_t _count )
    {
        if( m_playCount == _count )
        {
            return;
        }

        m_playCount = _count;

        this->_setPlayCount( _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::_setPlayCount( uint32_t _count )
    {
        (void)_count;
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::setAutoPlay( bool _autoPlay )
    {
        if( m_autoPlay == _autoPlay )
        {
            return;
        }

        m_autoPlay = _autoPlay;

        this->_setAutoPlay( _autoPlay );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::_setAutoPlay( bool _autoPlay )
    {
        (void)_autoPlay;
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::setStretch( float _scretch )
    {
        if( mt::equal_f_f( m_stretch, _scretch ) == true )
        {
            return;
        }

        m_stretch = _scretch;

        this->_setStretch( _scretch );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::_setStretch( float _scretch )
    {
        (void)_scretch;
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t BaseAnimation::play( float _time )
    {
        if( m_pause == true )
        {
            return 0;
        }

        m_playIterator = m_playCount;

        uint32_t id = ++m_enumerator;

        if( m_play == true )
        {
            if( this->_restart( m_enumerator, _time ) == false )
            {
                return 0;
            }
        }
        else
        {
            if( this->_play( id, _time ) == false )
            {
                return 0;
            }
        }

        m_interrupt = false;

        m_playTime = _time;

        m_play = true;

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseAnimation::stop()
    {
        if( m_play == false )
        {
            return false;
        }

        m_playTime = 0.f;
        m_play = false;
        m_pause = false;
        m_interrupt = false;

        this->_stop( m_enumerator );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseAnimation::pause()
    {
        if( m_play == false )
        {
            return false;
        }

        m_pause = true;

        this->_pause( m_enumerator );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::resume( float _time )
    {
        if( m_play == true )
        {
            return;
        }

        if( m_pause == false )
        {
            return;
        }

        m_pause = false;

        this->_resume( m_enumerator, _time );

        m_playTime = _time;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseAnimation::interrupt()
    {
        if( m_play == false )
        {
            return false;
        }

        if( m_interrupt == true )
        {
            return true;
        }

        m_interrupt = true;

        if( this->_interrupt( m_enumerator ) == false )
        {
            m_interrupt = false;

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float BaseAnimation::calcTotalTime( const UpdateContext * _context ) const
    {
        float totalTime = _context->time;
        if( m_playTime > _context->current )
        {
            float deltha = m_playTime - _context->current;
            totalTime -= deltha;
        }

        float speedFactor = this->getAnimationSpeedFactor();
        float scretch = this->getStretch();

        totalTime *= speedFactor / scretch;

        return totalTime;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::end()
    {
        if( m_play == false )
        {
            return;
        }

        m_play = false;
        m_interrupt = false;

        this->_end( m_enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::_setLoop( bool _value )
    {
        (void)_value;
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::_setAnimationSpeedFactor( float _factor )
    {
        (void)_factor;
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::setIntervalStart( float _startInterval )
    {
        if( mt::equal_f_f( m_intervalStart, _startInterval ) == true )
        {
            return;
        }

        m_intervalStart = _startInterval;

        this->_setIntervalStart( _startInterval );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::_setIntervalStart( float _startInterval )
    {
        (void)_startInterval;
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::setInterval( float _begin, float _end )
    {
        if( mt::equal_f_f( m_intervalBegin, _begin ) == true && mt::equal_f_f( m_intervalEnd, _end ) == true )
        {
            return;
        }

        m_intervalBegin = _begin;
        m_intervalEnd = _end;

        this->_setInterval( _begin, _end );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::_setInterval( float _begin, float _end )
    {
        (void)_begin;
        (void)_end;
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
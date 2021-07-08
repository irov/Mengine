#include "Kernel/BaseAnimation.h"

#include "Interface/UpdationInterface.h"
#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/MixinDebug.h"

#include "math/utils.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseAnimation::BaseAnimation()
        : m_playId( 0 )
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
        MENGINE_UNUSED( _time );

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
    float BaseAnimation::getDuration() const
    {
        float duration = this->_getDuration();

        return duration;
    }
    //////////////////////////////////////////////////////////////////////////
    float BaseAnimation::_getDuration() const
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
        MENGINE_UNUSED( _count );

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
        MENGINE_UNUSED( _autoPlay );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::setStretch( float _stretch )
    {
        if( mt::equal_f_f( m_stretch, _stretch ) == true )
        {
            return;
        }

        m_stretch = _stretch;

        this->_setStretch( _stretch );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::_setStretch( float _stretch )
    {
        MENGINE_UNUSED( _stretch );

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

        UniqueId id = ENUMERATOR_SERVICE()
            ->generateUniqueIdentity();

        m_playId = id;

        m_interrupt = false;
        m_playTime = _time;

        bool old_play = m_play;
        m_play = true;

        if( old_play == true )
        {
            if( this->_restart( m_playId, _time ) == false )
            {
                m_play = false;

                return 0;
            }
        }
        else
        {
            if( this->_play( m_playId, _time ) == false )
            {
                m_play = false;

                return 0;
            }
        }

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

        if( this->_stop( m_playId ) == false )
        {
            return false;
        }

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

        this->_pause( m_playId );

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
        m_playTime = _time;

        this->_resume( m_playId, _time );
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

        if( this->_interrupt( m_playId ) == false )
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

            MENGINE_ASSERTION_FATAL( totalTime >= 0.f, "totalTime %f < 0.f ['%s:%s']"
                , totalTime
                , MENGINE_MIXIN_DEBUG_NAME( this )
                , MENGINE_MIXIN_DEBUG_TYPE( this )
            );
        }

        float speedFactor = this->getAnimationSpeedFactor();
        float stretch = this->getStretch();

        totalTime *= speedFactor / stretch;

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

        this->_end( m_playId );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::_setLoop( bool _value )
    {
        MENGINE_UNUSED( _value );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseAnimation::_setAnimationSpeedFactor( float _factor )
    {
        MENGINE_UNUSED( _factor );

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
        MENGINE_UNUSED( _startInterval );

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
        MENGINE_UNUSED( _begin );
        MENGINE_UNUSED( _end );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
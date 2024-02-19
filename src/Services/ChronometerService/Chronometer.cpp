#include "Chronometer.h"

#include "Kernel/TimestampHelper.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Chronometer::Chronometer()
        : m_timeBegin( 0 )
        , m_timePauseBegin( 0 )
        , m_timePauseCollect( 0 )
        , m_oldTime( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Chronometer::~Chronometer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Chronometer::initialize( const LambdaChronometer & _lambda )
    {
        m_lambda = _lambda;
    }
    //////////////////////////////////////////////////////////////////////////
    void Chronometer::update( Timestamp _currentTime )
    {
        if( m_timePauseBegin != 0 )
        {
            return;
        }

        Timestamp chronometerTime = _currentTime - m_timeBegin - m_timePauseCollect;

        if( m_oldTime == 0 || chronometerTime - m_oldTime >= 1000 )
        {
            m_lambda( chronometerTime );

            m_oldTime = chronometerTime - chronometerTime % 1000;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Chronometer::pause()
    {
        if( m_timePauseBegin != 0 )
        {
            return false;
        }

        m_timePauseBegin = Helper::getSystemTimestamp();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Chronometer::resume()
    {
        if( m_timePauseBegin == 0 )
        {
            return false;
        }

        Timestamp timePause = Helper::getSystemTimestamp() - m_timePauseBegin;

        m_timePauseCollect += timePause;
        m_timePauseBegin = 0;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp Chronometer::getTime() const
    {
        if( m_timePauseBegin != 0 )
        {
            Timestamp pausedTime = m_timePauseBegin - m_timeBegin - m_timePauseCollect;

            return pausedTime;
        }

        Timestamp currentTime = Helper::getSystemTimestamp();

        Timestamp time = currentTime - m_timeBegin - m_timePauseCollect;

        return time;
    }
    //////////////////////////////////////////////////////////////////////////
}

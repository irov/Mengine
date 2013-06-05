#   include "MarmaladeTimer.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    MarmaladeTimer::MarmaladeTimer() 
        : m_time(0)
        , m_supportNanoseconds(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MarmaladeTimer::~MarmaladeTimer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeTimer::initialize()
    {
        int64 ns = s3eTimerGetUSTNanoseconds();

        if( ns != -1 )
        {
            m_supportNanoseconds = true;
        }

        this->reset();
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeTimer::reset()
    {
        uint64 time = s3eTimerGetMs();

        m_time = time;
    }
    //////////////////////////////////////////////////////////////////////////
    float MarmaladeTimer::getDeltaTime() const
    {
        uint64 curTime = s3eTimerGetMs();
        uint64 delta = curTime - m_time;

        m_time = curTime;

        return static_cast<float>(delta);
    }
    //////////////////////////////////////////////////////////////////////////
    unsigned long MarmaladeTimer::getMilliseconds()
    {
        uint64 ms = s3eTimerGetMs();
        unsigned long ul_ms = static_cast<unsigned long>(ms);

        return ul_ms;
    }
    //////////////////////////////////////////////////////////////////////////
    unsigned long MarmaladeTimer::getMicroseconds()
    {
        if( m_supportNanoseconds == false )
        {
            unsigned long ms = this->getMilliseconds();
            unsigned long mcs = ms * 1000;

            return mcs;
        }
        
        int64 ns = s3eTimerGetUSTNanoseconds();
        uint64 ms = ns / 1000;

        unsigned long ul_ms = static_cast<unsigned long>(ms);
        
        return ul_ms;
    }
    //////////////////////////////////////////////////////////////////////////
    unsigned long MarmaladeTimer::getMillisecondsCPU()
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    unsigned long MarmaladeTimer::getMicrosecondsCPU()
    {
        return 0;
    }
}
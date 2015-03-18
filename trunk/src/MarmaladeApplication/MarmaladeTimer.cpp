#   include "MarmaladeTimer.h"

#	include <ctime>

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
	uint64_t MarmaladeTimer::getUnixTime() const
	{
		uint64_t t = std::time(nullptr);

		return t;
	}
    //////////////////////////////////////////////////////////////////////////
    uint64_t MarmaladeTimer::getMilliseconds()
    {
        uint64 ms = s3eTimerGetMs();
        uint64_t ul_ms = static_cast<uint64_t>(ms);

        return ul_ms;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t MarmaladeTimer::getMicroseconds()
    {
        if( m_supportNanoseconds == false )
        {
            uint64_t ms = this->getMilliseconds();
            uint64_t mcs = ms * 1000;

            return mcs;
        }
        
        int64 ns = s3eTimerGetUSTNanoseconds();
        uint64 ms = ns / 1000;

        uint64_t ul_ms = static_cast<uint64_t>(ms);
        
        return ul_ms;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t MarmaladeTimer::getMillisecondsCPU()
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t MarmaladeTimer::getMicrosecondsCPU()
    {
        return 0;
    }
}